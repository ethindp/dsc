#pragma once
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <functional>
#include <mutex>
#include <tuple>
#include <variant>

typedef std::uint8_t DsData;

class DataStructure {
public:
  virtual bool insert(const DsData) { return false; }
  virtual void remove(const DsData) {}
  // Draws the tree
  void draw();
};
