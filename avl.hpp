#pragma once
#include <algorithm>
#include <memory>
#include <cstdint>
#include <variant>
#include <tuple>
#include <optional>
#include <stdexcept>
#include <vector>
#include <functional>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <plog/Log.h>
#include "common.hpp"
#include "tts.h"

enum class NodeDirection {
None,
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
    ~AVLTree();
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
    void draw();
private:
void generate_points(const std::shared_ptr<AVLNode> node, const std::function<void(const std::shared_ptr<AVLNode>, const NodeDirection)>);
    std::shared_ptr<AVLNode> root;
    std::optional<NodeDirection> direction;
    SDL_Renderer *renderer;
    std::vector<SDL_Point> points;
    int cur_x;
    int cur_y;
TTF_Font *font;
int wwidth = 0;
int wheight = 0;
ViewMode view_mode = ViewMode::Graphical;
};
