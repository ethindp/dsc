#include "glad/gl.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <mutex>
#include <plog/Log.h>
#include <random>
#include <string>
#include <thread>
#include <tuple>
#ifdef _WINDOWS_
#include <windows.h>
#endif
#include "avl.hpp"
#include "btree.hpp"
#include "common.hpp"
#include "rbtree.hpp"
#include "tinyfiledialogs.h"
#include "tts.h"

void handleNormalExit();
void createSampleTree(sf::RenderWindow &window, tgui::Gui &gui);
#ifdef _WINDOWS_
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nCmdShow) {
#else
int main(int argc, char *argv[]) {
#endif
  try {
#ifdef _WINDOWS_
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT |
                 SEM_NOGPFAULTERRORBOX);
#endif
    if (std::filesystem::exists("./dsc.log")) {
      std::filesystem::remove("./dsc.log");
    }
    plog::init(plog::debug, "dsc.log");
    LOGI << "Data structure analyzer";
    LOGI << "Compiled on " << __DATE__ << " " << __TIME__;
    LOGI << "Setting exit handler";
    atexit(handleNormalExit);
    LOGI << "Creating window";
    sf::ContextSettings settings;
    settings.majorVersion = 3;
    settings.minorVersion = 2;
    settings.attributeFlags = sf::ContextSettings::Debug;
    sf::RenderWindow window(sf::VideoMode::getFullscreenModes().front(),
                            "Data Structure Analyzer", sf::Style::Fullscreen,
                            settings);
    LOGI << "Loading OpenGL";
    auto version = gladLoaderLoadGL();
    LOGI << "Loaded OpenGL version: " << GLAD_VERSION_MAJOR(version) << "."
         << GLAD_VERSION_MINOR(version);
    LOGI << "Enabling VSync";
    window.setVerticalSyncEnabled(true);
    LOGI << "Enabling mouse cursor";
    window.setMouseCursorVisible(true);
    LOGI << "Setting framerate";
    window.setFramerateLimit(60);
    LOGI << "Initializing GUI";
    tgui::Gui gui(window);
    gui.setTabKeyUsageEnabled(true);
    LOGI << "Initializing TTS engine";
    tts::initialize();
    LOGI << "Creating sample tree";
    createSampleTree(window, gui);
    return 0;
  } catch (std::exception &ex) {
    LOGF << "Critical error: " << ex.what();
    tinyfd_messageBox("Critical error", ex.what(), "ok", "error", 1);
    return 1;
  } catch (...) {
    LOGF << "Can't continue: unknown error";
    tinyfd_messageBox("Critical error",
                      "The application has encountered an unspecified error "
                      "and must abort execution.",
                      "ok", "error", 1);
    return 1;
  }
}

void handleNormalExit() { gladLoaderUnloadGL(); }

void createSampleTree(sf::RenderWindow &window, tgui::Gui &gui) {
  // For this prototype, we just create an AVL tree.
  std::mt19937 rng;
  std::seed_seq::result_type data[std::mt19937::state_size];
  std::random_device entropy_source;
  std::generate_n(data, std::mt19937::state_size, std::ref(entropy_source));
  std::seed_seq prng_seed(data, data + std::mt19937::state_size);
  rng.seed(prng_seed);
  std::uniform_int_distribution<DsData> distribution(0, 255);
  AVLTree tree;
  for (int i = 0; i < 10; ++i)
    tree.insert(distribution(rng));

  tts::say(fmt::format("Tree created, 10 items. Height: {}", tree.height()));
  bool done = false;
  while (!done) {
    sf::Event event;
    while (window.pollEvent(event)) {
      gui.handleEvent(event);
      if (event.type == sf::Event::Closed)
        done = true;
    }
    tree.draw(gui);
    window.clear();
    gui.draw();
    window.display();
  }
}
