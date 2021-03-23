#include "avl.hpp"

void AVLTree::rebalance(std::shared_ptr<AVLNode> n) {
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

AVLTree::AVLTree(void) : root(nullptr), direction(std::nullopt) {}

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
default: return;
}
}
return;
}

