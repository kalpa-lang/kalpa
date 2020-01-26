#include "expression_parser.h"

#include <utility>

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

bool Node::is_complete() const {
    return is_value()
        || children.size() == operators.size() + 1;
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
    if (!is_complete()) {
        return "INCOMPLETE";
    }
    for (size_t i = 0; i < operators.size(); ++i) {
        result += format(" {} ", get_priority());
        Node* cur_child = children[i + 1];
        result += cur_child ? cur_child->to_string() : "NULL";
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

bool is_l_bracket(const Token& token) {
    switch(token.type) {
        case Token::Type::LeftParen    : return true;
        case Token::Type::LeftBracket  : return true;
        case Token::Type::LeftBrace    : return true;
        default                        : return false;
    }
}

bool is_r_bracket(const Token& token) {
    return token.type == Token::Type::RightParen
        || token.type == Token::Type::RightBracket
        || token.type == Token::Type::RightBrace;
}

bool is_l_paren(const Token& token) {
    return token.type == Token::Type::LeftParen;
}

bool is_r_paren(const Token& token) {
    return token.type == Token::Type::RightParen;
}

bool is_identifier(const Token& token) {
    return token.type == Token::Type::Identifier;
}

bool can_be_part_of_rvalue(const Token& token) {
    return is_operator(token)
        || token.type == Token::Type::Identifier
        || is_l_bracket(token)
        || is_r_bracket(token);
}

bool can_be_r_expression_separator(const Token& token) {
    return is_r_paren(token)
        || token.type == Token::Type::Comma;
}

int Node::get_priority() const {
    if (operators.empty())
        return -1;
    return get_operator_priority(operators[0], children[0]);
}

Node* parse_expression(TokenizerView& tokenizer_view);

std::pair<bool, Node*> parse_element_expression(TokenizerView& tokenizer_view) {
    Token cur_token = tokenizer_view.get_cur_token();
    if (!can_be_part_of_rvalue(cur_token))
        return std::make_pair(false, nullptr);
    if (is_identifier(cur_token)) {
        tokenizer_view.move_to_next_token();
        // TODO: parse function call
        return std::make_pair(true, new Node(cur_token.value));
    }
    // TODO: parse tuple/list/set
    if (is_l_paren(cur_token)) {
        tokenizer_view.move_to_next_token();
        Node* expression = parse_expression(tokenizer_view);
        cur_token = tokenizer_view.get_cur_token();
        if (!is_r_paren(cur_token))
            return std::make_pair(false, nullptr);
        tokenizer_view.move_to_next_token();
        return std::make_pair(true, expression);
    }
    return std::make_pair(true, nullptr);
}

Node* parse_expression(TokenizerView& tokenizer_view) {
    std::vector<Node*> available_expression_parts;
    Token cur_token = tokenizer_view.get_cur_token();
    while (can_be_part_of_rvalue(cur_token)) {
        // TODO: parse brackets
        std::pair<bool, Node*> cur_element_parse_res = parse_element_expression(tokenizer_view);
        if (!cur_element_parse_res.first)
            return nullptr;
        Node* cur_element = cur_element_parse_res.second;
        cur_token = tokenizer_view.get_cur_token();
        if (!can_be_part_of_rvalue(cur_token)
                || can_be_r_expression_separator(cur_token)) {
            available_expression_parts.push_back(cur_element);
            break;
        }
        if (cur_element == nullptr
                && !can_be_unary_operator(cur_token))
            return nullptr;
        while (!available_expression_parts.empty()
                && available_expression_parts.back()->get_priority()
                > get_operator_priority(cur_token, cur_element)) {
            if (cur_element == nullptr)
                return nullptr;
            Node* cur_rhs = cur_element;
            cur_element = available_expression_parts.back();
            cur_element->add_last_child(cur_rhs);
            available_expression_parts.pop_back();
        }
        if (!available_expression_parts.empty()
            && !available_expression_parts.back()->is_unary_operator()
            && available_expression_parts.back()->get_priority()
                == get_operator_priority(cur_token, cur_element)) {
            available_expression_parts.back()->add_middle_child(
                cur_token, cur_element);
        } else {
            Node* current_node = new Node();
            current_node->add_middle_child(cur_token, cur_element);
            available_expression_parts.push_back(current_node);
        }
        cur_token = tokenizer_view.move_to_next_token();
    }
    if (available_expression_parts.empty())
        return nullptr;
    if (available_expression_parts.back() == nullptr)
        return nullptr;
    if (!available_expression_parts.back()->is_complete())
        return nullptr;
    while (available_expression_parts.size() != 1) {
        Node* cur_expression_part = available_expression_parts.back();
        available_expression_parts.pop_back();
        available_expression_parts.back()->add_last_child(cur_expression_part);
    }
    return available_expression_parts.back();
}

Node* parse_expression(Tokenizer& tokenizer) {
    TokenizerView tokenizer_view(tokenizer);
    Node* expression_tree = parse_expression(tokenizer_view);
    return expression_tree;
    // return can_be_part_of_rvalue(tokenizer_view.get_cur_token())
    //     ? expression_tree : nullptr;
}

}

