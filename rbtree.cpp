#include "rbtree.hpp"

RbNode::RbNode(const DsData data) {
    this->data = data;
    color = Color::Red;
    left = right = parent = nullptr;
}

RBTree::RBTree() {
    this->root = nullptr;
}

Color RBTree::getColor(const std::shared_ptr<RbNode> node) {
    if (!node)
        return Color::Black;

    return node->color;
}

void RBTree::setColor(std::shared_ptr<RbNode> node, const Color color) {
    if (!node)
        return;

    node->color = color;
}

std::shared_ptr<RbNode> RBTree::insertNode(std::shared_ptr<RbNode> root, std::shared_ptr<RbNode> ptr) {
    if (!root)
        return ptr;

    if (root->data < ptr->data) {
        root->left = insertNode(root->left, ptr);
        root->left->parent = root;
    } else if (root->data > ptr->data) {
        root->right = insertNode(root->right, ptr);
        root->right->parent = root;
    }

    return root;
}

void RBTree::insertValue(const DsData val) {
    auto node = std::make_shared<RbNode>(val);
    root = insertNode(root, node);
    fixInsert(node);
}

void RBTree::rotateLeft(std::shared_ptr<RbNode> ptr) {
    auto right_child = ptr->right;
    ptr->right = right_child->left;

    if (ptr->right)
        ptr->right->parent = ptr;

    right_child->parent = ptr->parent;

    if (!ptr->parent)
        root = right_child;
    else if (ptr == ptr->parent->left)
        ptr->parent->left = right_child;
    else
        ptr->parent->right = right_child;

    right_child->left = ptr;
    ptr->parent = right_child;
}

void RBTree::rotateRight(std::shared_ptr<RbNode>ptr) {
    auto left_child = ptr->left;
    ptr->left = left_child->right;

    if (ptr->left)
        ptr->left->parent = ptr;

    left_child->parent = ptr->parent;

    if (!ptr->parent)
        root = left_child;
    else if (ptr == ptr->parent->left)
        ptr->parent->left = left_child;
    else
        ptr->parent->right = left_child;

    left_child->right = ptr;
    ptr->parent = left_child;
}

void RBTree::fixInsert(std::shared_ptr<RbNode>ptr) {
    std::shared_ptr<RbNode>parent = nullptr;
    std::shared_ptr<RbNode>grandparent = nullptr;
    while (ptr != root && getColor(ptr) == Color::Red && getColor(ptr->parent) == Color::Red) {
        parent = ptr->parent;
        grandparent = parent->parent;
        if (parent == grandparent->left) {
            auto uncle = grandparent->right;
            if (getColor(uncle) == Color::Red) {
                setColor(uncle, Color::Black);
                setColor(parent, Color::Black);
                setColor(grandparent, Color::Red);
                ptr = grandparent;
            } else {
                if (ptr == parent->right) {
                    rotateLeft(parent);
                    ptr = parent;
                    parent = ptr->parent;
                }
                rotateRight(grandparent);
                std::swap(parent->color, grandparent->color);
                ptr = parent;
            }
        } else {
            auto uncle = grandparent->left;
            if (getColor(uncle) == Color::Red) {
                setColor(uncle, Color::Black);
                setColor(parent, Color::Black);
                setColor(grandparent, Color::Red);
                ptr = grandparent;
            } else {
                if (ptr == parent->left) {
                    rotateRight(parent);
                    ptr = parent;
                    parent = ptr->parent;
                }
                rotateLeft(grandparent);
                std::swap(parent->color, grandparent->color);
                ptr = parent;
            }
        }
    }
    setColor(root, Color::Black);
}

void RBTree::fixDelete(std::shared_ptr<RbNode>node) {
    if (!node)
        return;

    if (node == root) {
        root = nullptr;
        return;
    }

    if (getColor(node) == Color::Red || getColor(node->left) == Color::Red || getColor(node->right) == Color::Red) {
        auto child = node->left ? node->left : node->right;

        if (node == node->parent->left) {
            node->parent->left = child;
            if (child)
                child->parent = node->parent;
            setColor(child, Color::Black);
        } else {
            node->parent->right = child;
            if (child)
                child->parent = node->parent;
            setColor(child, Color::Black);
            }
    } else {
        std::shared_ptr<RbNode>sibling = nullptr;
        std::shared_ptr<RbNode>parent = nullptr;
        auto ptr = node;
        setColor(ptr, Color::DoubleBlack);
        while (ptr != root && getColor(ptr) == Color::DoubleBlack) {
            parent = ptr->parent;
            if (ptr == parent->left) {
                sibling = parent->right;
                if (getColor(sibling) == Color::Red) {
                    setColor(sibling, Color::Black);
                    setColor(parent, Color::Red);
                    rotateLeft(parent);
                } else {
                    if (getColor(sibling->left) == Color::Black && getColor(sibling->right) == Color::Black) {
                        setColor(sibling, Color::Red);
                        if(getColor(parent) == Color::Red)
                            setColor(parent, Color::Black);
                        else
                            setColor(parent, Color::DoubleBlack);
                        ptr = parent;
                    } else {
                        if (getColor(sibling->right) == Color::Black) {
                            setColor(sibling->left, Color::Black);
                            setColor(sibling, Color::Red);
                            rotateRight(sibling);
                            sibling = parent->right;
                        }
                        setColor(sibling, parent->color);
                        setColor(parent, Color::Black);
                        setColor(sibling->right, Color::Black);
                        rotateLeft(parent);
                        break;
                    }
                }
            } else {
                sibling = parent->left;
                if (getColor(sibling) == Color::Red) {
                    setColor(sibling, Color::Black);
                    setColor(parent, Color::Red);
                    rotateRight(parent);
                } else {
                    if (getColor(sibling->left) == Color::Black && getColor(sibling->right) == Color::Black) {
                        setColor(sibling, Color::Red);
                        if (getColor(parent) == Color::Red)
                            setColor(parent, Color::Black);
                        else
                            setColor(parent, Color::DoubleBlack);
                        ptr = parent;
                    } else {
                        if (getColor(sibling->left) == Color::Black) {
                            setColor(sibling->right, Color::Black);
                            setColor(sibling, Color::Red);
                            rotateLeft(sibling);
                            sibling = parent->left;
                        }
                        setColor(sibling, parent->color);
                        setColor(parent, Color::Black);
                        setColor(sibling->left, Color::Black);
                        rotateRight(parent);
                        break;
                    }
                }
            }
        }
        setColor(root, Color::Black);
    }
}

std::shared_ptr<RbNode> RBTree::deleteNode(std::shared_ptr<RbNode> root, const DsData data) {
    if (!root)
        return root;

    if (data < root->data)
        return deleteNode(root->left, data);

    if (data > root->data)
        return deleteNode(root->right, data);

    if (!root->left || !root->right)
        return root;

    auto temp = minValueNode(root->right);
    root->data = temp->data;
    return deleteNode(root->right, temp->data);
}

void RBTree::deleteValue(const DsData data) {
    auto node = deleteNode(root, data);
    fixDelete(node);
}

void RBTree::inOrder(const std::shared_ptr<RbNode>ptr, const std::function<void(const std::shared_ptr<RbNode>)> func) {
    if (!ptr)
        return;

    inOrder(ptr->left, func);
    func(ptr);
    inOrder(ptr->right, func);
}

void RBTree::inOrder(const std::function<void(const std::shared_ptr<RbNode>)> func) {
    inOrder(root, func);
}

void RBTree::preOrder(const std::shared_ptr<RbNode>ptr, const std::function<void(const std::shared_ptr<RbNode>)> func) {
    if (!ptr)
        return;

    func(ptr);
    preOrder(ptr->left, func);
    preOrder(ptr->right, func);
}

void RBTree::preOrder(const std::function<void(const std::shared_ptr<RbNode>)> func) {
    preOrder(root, func);
}

std::shared_ptr<RbNode>RBTree::minValueNode(const std::shared_ptr<RbNode> node) {
    auto ptr = node;
    while (ptr->left)
        ptr = ptr->left;

    return ptr;
}

std::shared_ptr<RbNode> RBTree::maxValueNode(const std::shared_ptr<RbNode> node) {
    auto ptr = node;
    while (ptr->right)
        ptr = ptr->right;

    return ptr;
}

int RBTree::getBlackHeight(std::shared_ptr<RbNode>node) {
    int blackheight = 0;
    while (node) {
        if (getColor(node) == Color::Black)
            blackheight++;
        node = node->left;
    }
    return blackheight;
}

void RBTree::merge(RBTree tree) {
    DsData temp;
    std::shared_ptr<RbNode>c, temp_ptr;
    auto root1 = root;
    auto root2 = tree.root;
    const int initialblackheight1 = getBlackHeight(root1);
    const int initialblackheight2 = getBlackHeight(root2);
    if (initialblackheight1 > initialblackheight2) {
        c = maxValueNode(root1);
        temp = c->data;
        deleteValue(c->data);
        root1 = root;
    } else if (initialblackheight2 > initialblackheight1) {
        c = minValueNode(root2);
        temp = c->data;
        tree.deleteValue(c->data);
        root2 = tree.root;
    } else {
        c = minValueNode(root2);
        temp = c->data;
        tree.deleteValue(c->data);
        root2 = tree.root;
        if (initialblackheight1 != getBlackHeight(root2)) {
            tree.insertValue(c->data);
            root2 = tree.root;
            c = maxValueNode(root1);
            temp = c->data;
            deleteValue(c->data);
            root1 = root;
        }
    }
    setColor(c, Color::Red);
    const int finalblackheight1 = getBlackHeight(root1);
    const int finalblackheight2 = getBlackHeight(root2);
    if (finalblackheight1 == finalblackheight2) {
        c->left = root1;
        root1->parent = c;
        c->right = root2;
        root2->parent = c;
        setColor(c, Color::Black);
        c->data = temp;
        root = c;
    } else if (finalblackheight2 > finalblackheight1) {
        auto ptr = root2;
        while (finalblackheight1 != getBlackHeight(ptr)) {
            temp_ptr = ptr;
            ptr = ptr->left;
        }
        std::shared_ptr<RbNode>ptr_parent;
        if (!ptr)
            ptr_parent = temp_ptr;
        else
            ptr_parent = ptr->parent;
        c->left = root1;
        if (root1)
            root1->parent = c;
        c->right = ptr;
        if (ptr)
            ptr->parent = c;
        ptr_parent->left = c;
        c->parent = ptr_parent;
        if (getColor(ptr_parent) == Color::Red)
            fixInsert(c);
        else if (getColor(ptr) == Color::Red)
            fixInsert(ptr);

        c->data = temp;
        root = root2;
    } else {
        auto ptr = root1;
        while (finalblackheight2 != getBlackHeight(ptr))
            ptr = ptr->right;

        auto ptr_parent = ptr->parent;
        c->right = root2;
        root2->parent = c;
        c->left = ptr;
        ptr->parent = c;
        ptr_parent->right = c;
        c->parent = ptr_parent;
        if (getColor(ptr_parent) == Color::Red)
            fixInsert(c);
        else if (getColor(ptr) == Color::Red)
            fixInsert(ptr);

        c->data = temp;
        root = root1;
    }
    return;
}
