#pragma once
#include <memory>
#include <variant>
#include <functional>
#include "common.hpp"

enum class Color {
Red,
Black,
DoubleBlack
};

struct RbNode {
    DsData data;
    Color color;
    std::shared_ptr<RbNode> left;
    std::shared_ptr<RbNode> right;
    std::shared_ptr<RbNode> parent;

    explicit RbNode(const DsData);
};

class RBTree {
    private:
        std::shared_ptr<RbNode> root;
    public:
        void rotateLeft(std::shared_ptr<RbNode>);
        void rotateRight(std::shared_ptr<RbNode>);
        void fixInsert(std::shared_ptr<RbNode>);
        void fixDelete(std::shared_ptr<RbNode>);
        void inOrder(const std::shared_ptr<RbNode>, const std::function<void(const std::shared_ptr<RbNode>)>);
        void preOrder(const std::shared_ptr<RbNode>, const std::function<void(const std::shared_ptr<RbNode>)>);
        Color getColor(const std::shared_ptr<RbNode>);
        void setColor(std::shared_ptr<RbNode>, const Color);
        std::shared_ptr<RbNode>minValueNode(const std::shared_ptr<RbNode>);
        std::shared_ptr<RbNode>maxValueNode(const std::shared_ptr<RbNode>);
        std::shared_ptr<RbNode> insertNode(const std::shared_ptr<RbNode>, const std::shared_ptr<RbNode>);
        std::shared_ptr<RbNode> deleteNode(std::shared_ptr<RbNode>, const DsData);
        int getBlackHeight(std::shared_ptr<RbNode>);
        RBTree();
        void insertValue(const DsData);
        void deleteValue(const DsData);
        void merge(RBTree);
        void inOrder(const std::function<void(const std::shared_ptr<RbNode>)>);
        void preOrder(const std::function<void(const std::shared_ptr<RbNode>)>);
};

