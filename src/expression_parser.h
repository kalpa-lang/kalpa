#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <string>
#include <vector>

#include "defs.h"
#include "tokenizer.h"

namespace klp {
class Node {
public:
    Node() :
        operators(std::vector<Token>()),
        children(std::vector<Node*>()) {}
    Node(const Value& value) :
        value(value),
        operators(std::vector<Token>()),
        children(std::vector<Node*>()) {}

    void add_last_child(Node* child);
    void add_middle_child(const Token& oper, Node* child);

    bool is_value() const;

    int get_priority() const;

    std::string to_string() const;

private:
    std::vector<Token> operators;
    std::vector<Node*> children;
    
    Value value;
};

class TokenizerView {
public:
    TokenizerView(Tokenizer& tokenizer) :
        tokenizer(tokenizer), 
        cur_token(tokenizer.next()) {}
    
    Token get_cur_token() const;
    Token move_to_next_token();
private:
    Token cur_token;
    Tokenizer& tokenizer;
};

Node* parse_expression(Tokenizer& tokenizer);
}

#endif
