#pragma once
#include <algorithm>
#include <memory>
#include <cstdint>
#include <variant>
#include <tuple>
#include <optional>
#include <SDL2/SDL.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <vulkan/vulkan.hpp>
#include "common.hpp"
#include "tts.h"

enum class NodeDirection {
Left,
Right
};

class AVLNode {
public:
    DsData key;
    int balance;
    std::shared_ptr<AVLNode> left, right, parent;

    AVLNode(const DsData key, const std::shared_ptr<AVLNode> parent) : key(key), balance(0), parent(parent), left(nullptr), right(nullptr) {}
};

class AVLTree : public DataStructure {
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
    void on_keyboard(const std::tuple<const std::uint8_t, const std::uint8_t, const SDL_Keycode, const std::uint16_t>);
private:
    std::shared_ptr<AVLNode> root;
    std::optional<NodeDirection> direction;
};
