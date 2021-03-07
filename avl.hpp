#pragma once
#include <algorithm>
#include <memory>
#include <cstdint>
#include <variant>
#include <SDL2/SDL.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include "common.hpp"
#include "tts.h"

class AVLNode {
public:
    DsData key;
    int balance;
    std::shared_ptr<AVLNode> left, right, parent;

    AVLNode(const DsData key, const std::shared_ptr<AVLNode> parent) : key(key), balance(0), parent(parent), left(nullptr), right(nullptr) {}
};

class AVLTree {
public:
    AVLTree(void);
    bool insert(const DsData key);
    void remove(const DsData key);
    std::shared_ptr<AVLNode> rotateLeft          (std::shared_ptr<AVLNode> a);
    std::shared_ptr<AVLNode> rotateRight         (std::shared_ptr<AVLNode> a);
    std::shared_ptr<AVLNode> rotateLeftThenRight (std::shared_ptr<AVLNode> n);
    std::shared_ptr<AVLNode> rotateRightThenLeft (std::shared_ptr<AVLNode> n);
    void rebalance                  (std::shared_ptr<AVLNode> n);
    int height                      (std::shared_ptr<AVLNode> n);
    int height();
    void setBalance                 (std::shared_ptr<AVLNode> n);
    void balance               (std::shared_ptr<AVLNode> n);
    std::shared_ptr<AVLNode> getRoot() const;
    bool processEvents();
private:
    std::shared_ptr<AVLNode> root;
    const std::shared_ptr<AVLNode> rootNode;
};
