#include "expression_parser.h"

#include "defs.h"
#include "tokenizer.h"
#include "print.h"

namespace klp {

void Node::add_last_child(Node* child) {
    children.push_back(child);
}

void Node::add_middle_child(const Token& oper, Node* child) {
    operators.push_back(oper);
    children.push_back(child);
}

bool Node::is_value() const {
    return children.empty();
}

bool Node::is_unary_operator() const {
    return !is_value() && children[0] == nullptr;
}

int get_operator_priority(const Token& token);

std::string Node::to_string() const {
    if (is_value())
        return std::visit([] (const auto& x) {
            return format("{}", x);
        }, value);
    std::string result = "(";
    if (children[0] != nullptr) {
        result += children[0]->to_string();
    }
    for (size_t i = 0; i < operators.size(); ++i) {
        result += format(" {} ", get_priority());
        result += children[i + 1]->to_string();
    }
    result += ")";
    return result;
}

Token TokenizerView::get_cur_token() const {
    return cur_token;
}

Token TokenizerView::move_to_next_token() {
    return cur_token = tokenizer.next();
}

int get_unary_operator_priority(const Token& token) {
    switch(token.type) {
        case Token::Type::Not           : return 3;
        case Token::Type::Add           : return 11;
        case Token::Type::Sub           : return 11;
        // case Token::Type::BitwiseNot    : return 12;
        default                         : return -1;
    }
}

bool can_be_unary_operator(const Token& token) {
    return get_unary_operator_priority(token) != -1;
}

int get_binary_operator_priority(const Token& token) {
    // or -1 if the given token is not an operator
    switch (token.type) {
        case Token::Type::Or            : return 1;
        case Token::Type::And           : return 2;
        case Token::Type::In            : return 4;
        case Token::Type::Equal         : return 4;
        case Token::Type::NotEqual      : return 4;
        case Token::Type::Less          : return 4;
        case Token::Type::LessEq        : return 4;
        case Token::Type::Greater       : return 4;
        case Token::Type::GreaterEq     : return 4;
        // case Token::Type::BitwiseOr     : return 5;
        case Token::Type::Xor           : return 6;
        // case Token::Type::BitwiseAnd    : return 7;
        // case Token::Type::BitwiseLShift : return 8;
        // case Token::Type::BitwiseRShift : return 8;
        case Token::Type::Add           : return 9;
        case Token::Type::Sub           : return 9;
        case Token::Type::Mul           : return 10;
        case Token::Type::Div           : return 10;
        case Token::Type::IntDiv        : return 10;
        // case Token::Type::Remainder     : return 10;
        case Token::Type::Pow           : return 13;
        case Token::Type::Dot           : return 14;
        case Token::Type::LeftBracket   : return 15;
        case Token::Type::RightBracket  : return 15;
        case Token::Type::LeftParen     : return 18;
        case Token::Type::RightParen    : return 18;
        case Token::Type::LeftBrace     : return 18;
        case Token::Type::RightBrace    : return 18;
        // case Token::Type::Identifier    : return 19;
        default                         : return -1;
    }
}

bool can_be_binary_operator(const Token& token) {
    return get_binary_operator_priority(token) != -1;
}

int get_operator_priority(const Token& token, Node* lhs) {
    if (lhs == nullptr)
        return get_unary_operator_priority(token);
    return get_binary_operator_priority(token);
}

bool is_operator(const Token& token) {
    return get_binary_operator_priority(token) != -1
        || get_unary_operator_priority(token) != -1;
}

bool is_opening_bracket(const Token& token) {
    switch(token.type) {
        case Token::Type::LeftParen    : return true;
        case Token::Type::LeftBracket  : return true;
        case Token::Type::LeftBrace    : return true;
        default                        : return false;
    }
}

bool is_closing_bracket(const Token& token) {
    switch(token.type) {
        case Token::Type::RightParen    : return true;
        case Token::Type::RightBracket  : return true;
        case Token::Type::RightBrace    : return true;
        default                         : return false;
    }
}

bool can_be_part_of_rvalue(const Token& token) {
    return is_operator(token)
        || token.type == Token::Type::Identifier
        || is_opening_bracket(token)
        || is_closing_bracket(token);
}

int Node::get_priority() const {
    if (operators.empty())
        return -1;
    return get_operator_priority(operators[0], children[0]);
}

Node* parse_expression(Tokenizer& tokenizer) {
    TokenizerView tokenizer_view(tokenizer);
    std::vector<Node*> available_expression_parts;
    Token cur_token = tokenizer_view.get_cur_token();
    while (can_be_part_of_rvalue(cur_token)) {
        // TODO: parse brackets
        Node* cur_identifier = nullptr;
        if (cur_token.type == Token::Type::Identifier) {
            cur_identifier = new Node(cur_token.value);
            tokenizer_view.move_to_next_token();
        }
        cur_token = tokenizer_view.get_cur_token();
        if (!can_be_part_of_rvalue(cur_token)) {
            available_expression_parts.push_back(cur_identifier);
            break;
        }
        if (cur_identifier == nullptr
                && !can_be_unary_operator(cur_token))
            return nullptr;
        while (!available_expression_parts.empty()
                && available_expression_parts.back()->get_priority()
                > get_operator_priority(cur_token, cur_identifier)) {
            if (cur_identifier == nullptr)
                return nullptr;
            Node* cur_rhs = cur_identifier;
            cur_identifier = available_expression_parts.back();
            cur_identifier->add_last_child(cur_rhs);
            available_expression_parts.pop_back();
        }
        if (!available_expression_parts.empty()
            && !available_expression_parts.back()->is_unary_operator()
            && available_expression_parts.back()->get_priority()
                == get_operator_priority(cur_token, cur_identifier)) {
            available_expression_parts.back()->add_middle_child(
                cur_token, cur_identifier);
        } else {
            Node* current_node = new Node();
            current_node->add_middle_child(cur_token, cur_identifier);
            available_expression_parts.push_back(current_node);
        }
        cur_token = tokenizer_view.move_to_next_token();
    }
    if (available_expression_parts.empty())
        return nullptr;
    if (available_expression_parts.back() == nullptr)
        return nullptr;
    while (available_expression_parts.size() != 1) {
        Node* cur_expression_part = available_expression_parts.back();
        available_expression_parts.pop_back();
        available_expression_parts.back()->add_last_child(cur_expression_part);
    }
    return available_expression_parts.back();
}

}

