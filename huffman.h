#pragma once
#include <map>
#include <cstddef>

class Node
{
public:
    Node(): left_(0), right_(0){};
    Node(unsigned char value, int sum): sum_(sum), value_(value), left_(0), right_(0){};
    Node(Node *left, Node *right): sum_(left->sum() + right->sum()), value_(0), left_(left), right_(right){};
    int sum() const {return sum_;}
    unsigned char value() const {return value_;}
    Node* left() const {return left_;}
    Node* right() const {return right_;}
private:
    int sum_;
public:
    unsigned char value_;
    Node *left_, *right_;
};

void encrypt(const char *inputfile, const char *outputfile);
void decrypt(const char *inputfile, const char *outputfile);
