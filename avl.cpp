#include "avl.hpp"

void AVLTree::rebalance(std::shared_ptr<AVLNode> n) {
this->points.clear();
    this->setBalance(n);

    if (n->balance == -2) {
        if (this->height(n->left->left) >= this->height(n->left->right))
            n = this->rotateRight(n);
        else
            n = this->rotateLeftThenRight(n);
    } else if (n->balance == 2) {
        if (this->height(n->right->right) >= this->height(n->right->left))
            n = this->rotateLeft(n);
        else
            n = this->rotateRightThenLeft(n);
    }

    if (n->parent) {
        this->rebalance(n->parent);
    } else {
        this->root = n;
    }
}

std::shared_ptr<AVLNode> AVLTree::rotateLeft(std::shared_ptr<AVLNode> a) {
    std::shared_ptr<AVLNode> b = a->right;
    b->parent = a->parent;
    a->right = b->left;

    if (a->right)
        a->right->parent = a;

    b->left = a;
    a->parent = b;

    if (b->parent)
        if (b->parent->right == a)
            b->parent->right = b;
        else
            b->parent->left = b;

this->setBalance(a);
    this->setBalance(b);
    return b;
}

std::shared_ptr<AVLNode> AVLTree::rotateRight(std::shared_ptr<AVLNode> a) {
    std::shared_ptr<AVLNode> b = a->left;
    b->parent = a->parent;
    a->left = b->right;

    if (a->left)
        a->left->parent = a;

    b->right = a;
    a->parent = b;

    if (b->parent)
        if (b->parent->right == a)
            b->parent->right = b;
        else
            b->parent->left = b;

    this->setBalance(a);
    this->setBalance(b);
    return b;
}

std::shared_ptr<AVLNode> AVLTree::rotateLeftThenRight(std::shared_ptr<AVLNode> n) {
    n->left = this->rotateLeft(n->left);
    return this->rotateRight(n);
}

std::shared_ptr<AVLNode> AVLTree::rotateRightThenLeft(std::shared_ptr<AVLNode> n) {
    n->right = this->rotateRight(n->right);
    return this->rotateLeft(n);
}

int AVLTree::height(std::shared_ptr<AVLNode> n) {
    if (!n)
        return -1;
    return 1 + std::max(height(n->left), height(n->right));
}

int AVLTree::height() {
return this->height(this->root);
}

void AVLTree::setBalance(std::shared_ptr<AVLNode> n) {
    n->balance = this->height(n->right) - this->height(n->left);
}

AVLTree::AVLTree(void) : root(nullptr), direction(std::nullopt) {
WINDOW_MUTEX.lock();
this->renderer = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
SDL_GetWindowSize(WINDOW, &this->wwidth, &this->wheight);
WINDOW_MUTEX.unlock();
if (!this->renderer) {
fmt::memory_buffer buf;
fmt::format_to(buf, "Could not create renderer: {}", SDL_GetError());
throw std::runtime_error(buf.data());
}
if (SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 0xFF)!=0) {
fmt::memory_buffer buf;
fmt::format_to(buf, "Could not set render draw color: {}", SDL_GetError());
throw std::runtime_error(buf.data());
}
auto rwops = SDL_RWFromFile("Inter.ttf", "rb");
if (!rwops) {
fmt::memory_buffer buf;
fmt::format_to(buf, "Cannot open font for UI: {}", SDL_GetError());
throw std::runtime_error(buf.data());
}
this->font = TTF_OpenFontRW(rwops, true, 14);
if (!this->font) {
fmt::memory_buffer buf;
fmt::format_to(buf, "Cannot load font for UI: {}", SDL_GetError());
throw std::runtime_error(buf.data());
}
LOGI << "Loaded font with face family: " << TTF_FontFaceFamilyName(this->font);
LOGI << "Setting font attributes";
TTF_SetFontStyle(this->font, TTF_STYLE_NORMAL);
TTF_SetFontHinting(this->font, TTF_HINTING_NORMAL);
LOGI << "Font height: " << TTF_FontHeight(this->font);
LOGI << "Font ascent: " << TTF_FontAscent(this->font);
LOGI << "Font descent: " << TTF_FontDescent(this->font);
LOGI << "Font line skip: " << TTF_FontLineSkip(this->font) << " line(s)";
if (TTF_GetFontKerning(this->font))
LOGI << "Loaded font supports kerning";
else
LOGI << "Kerning for loaded font not supported";

LOGI << "Font has " << TTF_FontFaces(this->font) << " faces";
LOGI << "Loaded font style: " << TTF_FontFaceStyleName(this->font);
}

bool AVLTree::insert(const DsData key) {
    if (!root) {
        this->root = std::make_shared<AVLNode>(AVLNode(key, nullptr));
    } else {
        std::shared_ptr<AVLNode> n = root, parent;

        while (true) {
            if (n->key == key)
                return false;

            parent = n;

            bool goLeft = n->key > key;
            n = goLeft ? n->left : n->right;

            if (!n) {
                if (goLeft)
                    parent->left = std::make_shared<AVLNode>(key, parent);
                else
                    parent->right = std::make_shared<AVLNode>(key, parent);

                this->rebalance(parent);
                break;
            }
        }
    }

    return true;
}

void AVLTree::remove(const DsData key) {
    if (!root)
        return;

    std::shared_ptr<AVLNode> n       = root;
    std::shared_ptr<AVLNode> parent  = root;
    std::shared_ptr<AVLNode> delNode = nullptr;
    std::shared_ptr<AVLNode> child   = root;

    while (child) {
        parent = n;
        n = child;
        child = delNode->key >= n->key ? n->right : n->left;
        if (delNode->key == n->key)
            delNode = n;
    }

    if (delNode) {
        delNode->key = n->key;

        child = n->left ? n->left : n->right;

        if (root->key == delNode->key) {
            root = child;
        } else {
            if (parent->left == n) {
                parent->left = child;
            } else {
                parent->right = child;
            }

            this->rebalance(parent);
        }
    }
}

void AVLTree::on_keyboard(const std::tuple<const std::uint8_t, const std::uint8_t, const SDL_Keycode, const std::uint16_t> data) {
auto [state, repeat, key, mod] = data;
if (state == SDL_PRESSED && !repeat) {
switch (key) 
case SDLK_v: {
tts::say(fmt::format("Current node: {}, balance of {} with value {}. {}, {}, {}", get_node_type_str(this->root->key), this->root->balance, get_node_data(this->root->key), this->root->parent ? "Has parent" : "At root of tree", this->root->left ? "has left branch" : "no left branch", this->root->right ? "has right branch" : "no right branch"), true);
break;
case SDLK_UP:
if (this->root->parent) {
this->root = this->root->parent;
tts::say("At previous tree level", true);
} else
tts::say("At top of tree", true);
break;
case SDLK_LEFT: {
tts::say("Set left node to next node. Press down arrow to expand.", true);
this->direction = NodeDirection::Left;
}
break;
case SDLK_RIGHT: {
tts::say("Set right node to next node. Press down arrow to expand.", true);
this->direction = NodeDirection::Right;
}
break;
case SDLK_DOWN: {
if (this->direction)
if (this->direction == NodeDirection::Left) {
tts::say("Left branch expanded", true);
this->root = this->root->left;
} else {
tts::say("Right branch expanded", true);
this->root = this->root->right;
}
else
tts::say("No node direction", true);
}
break;
case SDLK_TAB:
if (this->view_mode == ViewMode::Graphical)
this->view_mode = ViewMode::Textual;
else
this->view_mode = ViewMode::Graphical;

break;
default: return;
}
}
return;
}

void AVLTree::draw() {
if (SDL_RenderClear(this->renderer) != 0) {
fmt::memory_buffer buf;
fmt::format_to(buf, "Could not clear render target: {}", SDL_GetError());
throw std::runtime_error(buf.data());
}
if (this->view_mode == ViewMode::Graphical) {
this->points.clear();
this->cur_x = 0;
this->cur_y = 0;
this->generate_points(this->root, [&](const auto node, const auto dir) {
if (dir == NodeDirection::Left)
cur_y -= 5;
else if (dir == NodeDirection::Right)
cur_y += 5;

points.push_back({cur_x, cur_y});
points.push_back({cur_x, cur_y + 5});
});
if (SDL_RenderDrawLines(this->renderer, this->points.data(), this->points.size())!=0) {
fmt::memory_buffer buf;
fmt::format_to(buf, "Could not draw lines to renderer: {}", SDL_GetError());
throw std::runtime_error(buf.data());
}
} else if (this->view_mode == ViewMode::Textual) {
fmt::memory_buffer buf;
fmt::format_to(buf, "Current node: {}, balance of {} with value {}. {}, {}, {}", get_node_type_str(this->root->key), this->root->balance, get_node_data(this->root->key), this->root->parent ? "Has parent" : "At root of tree", this->root->left ? "has left branch" : "no left branch", this->root->right ? "has right branch" : "no right branch");
auto surface = TTF_RenderText_Blended(this->font, buf.data(), {0xFF, 0xFF, 0xFF, 0xFF});
if (!surface) {
fmt::memory_buffer buf;
fmt::format_to(buf, "Could not render text: {}", TTF_GetError());
throw std::runtime_error(buf.data());
}
auto texture = SDL_CreateTextureFromSurface(this->renderer, surface);
if (!texture) {
fmt::memory_buffer buf;
fmt::format_to(buf, "Cannot create renderer texture: {}", SDL_GetError());
throw std::runtime_error(buf.data());
}
SDL_FreeSurface(surface);
SDL_Rect rect;
rect.x = this->wwidth/2;
rect.y = this->wheight/2;
if (TTF_SizeText(this->font, buf.data(), &rect.w, &rect.h) != 0) {
throw std::runtime_error(fmt::format("Could not get width of text string for display: {}", SDL_GetError()).data());
}
if (SDL_RenderCopy(this->renderer, texture, nullptr, &rect) != 0) {
throw std::runtime_error(fmt::format("Cannot copy surface data to renderer: {}", SDL_GetError()).data());
}
}
SDL_RenderPresent(this->renderer);
}

void AVLTree::generate_points(const std::shared_ptr<AVLNode> node, const std::function<void(const std::shared_ptr<AVLNode>, const NodeDirection)> func) {
if (node)
func(node, NodeDirection::None);

this->cur_x += 5;
if (node->left)
func(node->left, NodeDirection::Left);

if (node->right)
func(node->right, NodeDirection::Right);

if (node->left)
generate_points(node->left, func);

if (node->right)
generate_points(node->right, func);
}

AVLTree::~AVLTree() {
SDL_DestroyRenderer(this->renderer);
TTF_CloseFont(this->font);
}

