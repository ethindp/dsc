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
        this->rootNode = root;
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

AVLTree::AVLTree(void) : root(nullptr), rootNode(nullptr) {}

bool AVLTree::insert(const DsData key) {
    if (!root) {
        this->root = std::make_shared<AVLNode>(AVLNode(key, nullptr));
        this->rootNode = root;
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
            rootNode = root;
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

bool AVLTree::processEvents() {
auto tree_root = this->root;
this->curNode = this->root;
bool expanded_left = false, expanded_right = false;
unsigned long long level = 0;
SDL_Event e;
if (SDL_PollEvent(&e)) {
if (e.type==SDL_KEYDOWN) {
if (e.key.keysym.sym == SDLK_RIGHT) {
if (root->right) {
expanded_right = true;
TTS::say("Right branch open");
++level;
TTS::say(fmt::format("Level {}", level), false);
} else
TTS::say("There is no right branch on this node");
}
if (e.key.keysym.sym == SDLK_LEFT) {
if (root->left) {
expanded_left = true;
TTS::say("Left branch open");
++level;
TTS::say(fmt::format("Level {}", level), false);
} else
TTS::say("There is no left branch on this node");
}

if (e.key.keysym.sym == SDLK_d)
switch (tree_root->key.index()) {
case 0:
TTS::say(fmt::format("Node key: type signed 8-bit integer, value {}", std::get<0>(tree_root->key)));
break;
case 1:
TTS::say(fmt::format("Node key: type signed 16-bit integer, value {}", std::get<1>(tree_root->key)));
break;
case 2:
TTS::say(fmt::format("Node key: type signed 32-bit integer, value {}", std::get<2>(tree_root->key)));
break;
case 3:
TTS::say(fmt::format("Node key: type signed 64-bit integer, value {}", std::get<3>(tree_root->key)));
break;
case 4:
TTS::say(fmt::format("Node key: type unsigned 8-bit integer, value {}", std::get<4>(tree_root->key)));
break;
case 5:
TTS::say(fmt::format("Node key: type unsigned 16-bit integer, value {}", std::get<5>(tree_root->key)));
break;
case 6:
TTS::say(fmt::format("Node key: type unsigned 32-bit integer, value {}", std::get<6>(tree_root->key)));
break;
case 7:
TTS::say(fmt::format("Node key: type unsigned 64-bit integer, value {}", std::get<7>(tree_root->key)));
break;
case 8:
TTS::say(fmt::format("Node key: type signed character, value {:d}", std::get<8>(tree_root->key)));
break;
case 9:
TTS::say(fmt::format("Node key: type unsigned character, value {:d}", std::get<9>(tree_root->key)));
break;
case 10:
TTS::say(fmt::format("Node key: type single 32-bit float, value {:F}", std::get<10>(tree_root->key)));
break;
case 11:
TTS::say(fmt::format("Node key: type double 64-bit float, value {:F}", std::get<11>(tree_root->key)));
break;
case 12:
TTS::say(fmt::format("Node key: type extended float, value {:F}", std::get<12>(tree_root->key)));
break;
default:
TTS::say("Node key has unknown type; data unknown");
break;
}

if (e.key.keysym.sym == SDLK_ESCAPE)
return false;
}
}
return true;
}
