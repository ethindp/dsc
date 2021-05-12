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

std::shared_ptr<AVLNode>
AVLTree::rotateLeftThenRight(std::shared_ptr<AVLNode> n) {
  n->left = this->rotateLeft(n->left);
  return this->rotateRight(n);
}

std::shared_ptr<AVLNode>
AVLTree::rotateRightThenLeft(std::shared_ptr<AVLNode> n) {
  n->right = this->rotateRight(n->right);
  return this->rotateLeft(n);
}

int AVLTree::height(std::shared_ptr<AVLNode> n) {
  if (!n)
    return -1;
  return 1 + std::max(height(n->left), height(n->right));
}

int AVLTree::height() { return this->height(this->root); }

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

  std::shared_ptr<AVLNode> n = root;
  std::shared_ptr<AVLNode> parent = root;
  std::shared_ptr<AVLNode> delNode = nullptr;
  std::shared_ptr<AVLNode> child = root;

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

void AVLTree::draw(tgui::Gui &gui) {
  auto hlayout =
      std::dynamic_pointer_cast<tgui::HorizontalWrap>(gui.get("layout"));
  if (!hlayout) {
    hlayout = tgui::HorizontalWrap::create();
    auto tview =
        std::dynamic_pointer_cast<tgui::TreeView>(hlayout->get("tview"));
    if (!tview) {
      tview = tgui::TreeView::create();
      tview->onFocus.connect([&]() { tts::say("Tree nodes tree"); });
      tview->onItemSelect.connect([&](const std::string item) {
        tts::say(fmt::format("{}, selected", item));
        bool stop = false;
        generate(this->root, [&](const auto n, const auto) {
          if (stop)
            return;
          auto c = static_cast<DsData>(std::strtol(item.data(), nullptr, 10));
          if (n->key == c) {
            stop = true;
            this->root = n;
          }
        });
      });
      tview->onExpand.connect([&](const std::string item) {
        tts::say(fmt::format("{}, expanded", item));
      });
      tview->onCollapse.connect([&](const std::string item) {
        tts::say(fmt::format("{}, expanded", item));
      });
      if (tview->getNodes().empty()) {
        std::vector<sf::String> nodes;
        generate(this->root, [&](const auto node, const auto) {
          nodes.push_back(fmt::format("{}", node->key).data());
        });
        if (!tview->addItem(nodes))
          throw std::runtime_error("Cannot add nodes to tree");
      }
      hlayout->add(tview, "tview");
    }
    auto del_btn =
        std::dynamic_pointer_cast<tgui::Button>(hlayout->get("delbtn"));
    if (!del_btn) {
      del_btn = tgui::Button::create("Delete selected node...");
      del_btn->onFocus.connect(
          []() { tts::say("Delete selected node button"); });
      del_btn->onPress.connect([&](const sf::String) {
        auto f = sf::String(fmt::format("{}", this->root->key).data());
        std::dynamic_pointer_cast<tgui::TreeView>(hlayout->get("tview"))
            ->removeItem({f});
        this->remove(this->root->key);
      });
      hlayout->add(del_btn, "delbtn");
    }
    auto rebalance_btn =
        std::dynamic_pointer_cast<tgui::Button>(hlayout->get("Rebalance tree"));
    if (!rebalance_btn)
      rebalance_btn = tgui::Button::create("Rebalance tree");
    rebalance_btn->onFocus.connect([]() { tts::say("Rebalance tree"); });
    rebalance_btn->onPress.connect([&](const sf::String) {
      if (!this->root) {
        tts::say("No item selected or root node unknown");
        return;
      } else {
        this->rebalance(this->root);
        auto ptr =
            std::dynamic_pointer_cast<tgui::TreeView>(hlayout->get("tview"));
        ptr->removeAllItems();
        std::vector<sf::String> nodes;
        generate(this->root, [&](const auto node, const auto) {
          nodes.push_back(fmt::format("{}", node->key).data());
        });
        if (!ptr->addItem(nodes))
          throw std::runtime_error("Cannot add nodes to tree");

        tts::say("Tree rebalanced");
      }
    });
    hlayout->add(rebalance_btn);
    auto close_btn =
        std::dynamic_pointer_cast<tgui::Button>(hlayout->get("closebtn"));
    if (!close_btn) {
      close_btn = tgui::Button::create("Close and exit");
      close_btn->onFocus.connect([]() { tts::say("Close and exit button"); });
      close_btn->onPress.connect([](const sf::String) { std::exit(0); });
      hlayout->add(close_btn, "closebtn");
    }
    gui.add(hlayout, "layout");
  }
}

void AVLTree::generate(const std::shared_ptr<AVLNode> node,
                       const std::function<void(const std::shared_ptr<AVLNode>,
                                                const NodeDirection)>
                           func) {
  if (node)
    func(node, NodeDirection::None);

  if (node->left)
    func(node->left, NodeDirection::Left);

  if (node->right)
    func(node->right, NodeDirection::Right);

  if (node->left)
    generate(node->left, func);

  if (node->right)
    generate(node->right, func);
}

AVLTree::~AVLTree() {}
