#pragma once
#include <memory>
#include <variant>
#include <functional>
#include "common.hpp"

enum class OrderMethod {
InOrder,
PostOrder,
PreOrder
};

struct BinaryNode {
std::shared_ptr<BinaryNode> left;
std::shared_ptr<BinaryNode> right;
DsData data;
};

class BinaryTree {
private:
std::shared_ptr<BinaryNode> root;
public:

BinaryTree() {
root = nullptr;
}

inline bool isEmpty() {
return !root;
}

inline bool insert(const DsData item) {
std::shared_ptr<BinaryNode> new_node = std::make_shared<BinaryNode>();
std::shared_ptr<BinaryNode> parent = nullptr;
new_node->left = nullptr;
new_node->right= nullptr;
new_node->data = item;
if (isEmpty())
root = new_node;
else{
std::shared_ptr<BinaryNode> ptr = root;
while (ptr) {
parent = ptr;
if (item > ptr->data)
ptr = ptr->right;
else if(item < ptr->data)
ptr = ptr->left;
else
return false;
}

if(item > parent->data)
parent->right = new_node;
else if(item < parent->data)
parent->left = new_node;
}
return true;
}

inline std::shared_ptr<BinaryNode> getRoot(){
return root;
}

inline void loop(const OrderMethod method, std::shared_ptr<BinaryNode> troot, const std::function<void(const std::shared_ptr<BinaryNode>)> func) {
switch(method) {
case OrderMethod::InOrder: {
if(troot) {
loop(method, troot->left, func);
func(troot);
loop(method, troot->right, func);
}
}
break;
case OrderMethod::PostOrder: {
if(troot) {
loop(method, troot->left, func);
loop(method, troot->right, func);
func(troot);
}
}
break;
case OrderMethod::PreOrder: {
if(troot) {
func(troot);
loop(method, troot->left, func);
loop(method, troot->right, func);
}
}
break;
}
}
};
