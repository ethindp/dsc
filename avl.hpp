#pragma once
#include "common.hpp"
#include "tts.h"
#include <TGUI/TGUI.hpp>
#include <algorithm>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <functional>
#include <memory>
#include <optional>
#include <plog/Log.h>
#include <stdexcept>
#include <tuple>
#include <variant>
#include <vector>

enum class NodeDirection { None, Left, Right };

class AVLNode {
public:
  DsData key;
  int balance;
  std::shared_ptr<AVLNode> left, right, parent;

  AVLNode(const DsData key, const std::shared_ptr<AVLNode> parent)
      : key(key), balance(0), parent(parent), left(nullptr), right(nullptr) {}
};

class AVLTree {
public:
  AVLTree(void);
  ~AVLTree();
  bool insert(const DsData key);
  void remove(const DsData key);
  std::shared_ptr<AVLNode> rotateLeft(std::shared_ptr<AVLNode> a);
  std::shared_ptr<AVLNode> rotateRight(std::shared_ptr<AVLNode> a);
  std::shared_ptr<AVLNode> rotateLeftThenRight(std::shared_ptr<AVLNode> n);
  std::shared_ptr<AVLNode> rotateRightThenLeft(std::shared_ptr<AVLNode> n);
  void rebalance(std::shared_ptr<AVLNode> n);
  int height(std::shared_ptr<AVLNode> n);
  int height();
  void setBalance(std::shared_ptr<AVLNode> n);
  void balance(std::shared_ptr<AVLNode> n);
  std::shared_ptr<AVLNode> getRoot() const;
  void draw(tgui::Gui &);

private:
  void generate(const std::shared_ptr<AVLNode> node,
                const std::function<void(const std::shared_ptr<AVLNode>,
                                         const NodeDirection)>);
  std::shared_ptr<AVLNode> root;
  std::optional<NodeDirection> direction;
};
