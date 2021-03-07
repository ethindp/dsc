#include <string>
#include <cstddef>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <random>
#include <functional>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <plog/Log.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <vulkan/vulkan.hpp>
#ifdef _WINDOWS_
#include <windows.h>
#endif
#include "tts.h"
#include "avl.hpp"
#include "btree.hpp"
#include "rbtree.hpp"
#include "tinyfiledialogs.h"

using namespace std;

static vk::Instance INSTANCE;
static vk::PhysicalDevice PHYS_DEV;
static vk::Device DEVICE;
static vk::CommandPool CMDPOOL;
static mutex INSTANCE_MUTEX;
static mutex PHYS_DEV_MUTEX;
static mutex DEV_MUTEX;
static mutex POOL_MUTEX;

void handleNormalExit();
void createSampleTree();

#ifdef _WINDOWS_
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#else
int main(int argc, char* argv[]) {
#endif
try {
#ifdef _WINDOWS_
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOGPFAULTERRORBOX);
#endif
if (filesystem::exists("./dsc.log")) {
filesystem::remove("./dsc.log");
}
plog::init(plog::debug, "dsc.log");
LOGI << "Initializing SDL2";
if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) < 0) {
LOGF << "Could not initialize SDL! Reason: " << SDL_GetError();
stringstream ss;
ss << "Cannot initialize sdl! Reason: " << SDL_GetError();
tinyfd_messageBox("Critical error", ss.str().c_str(), "ok", "error", 1);
return 1;
}
atexit(handleNormalExit);
auto window = SDL_CreateWindow("Data Structure Analyzer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_VULKAN|SDL_WINDOW_ALLOW_HIGHDPI);
if (!window) {
LOGF << "Can't create window: " << SDL_GetError();
std::stringstream ss;
ss << "Can't create window: " << SDL_GetError();
tinyfd_messageBox("Critical error", ss.str().c_str(), "ok", "error", 1);
return 1;
}
SDL_SetHint("SDL_HINT_FRAMEBUFFER_ACCELERATION", "1");
SDL_SetHint("SDL_HINT_IME_INTERNAL_EDITING", "1");
SDL_SetHint("SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS", "0");
SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "1");
SDL_SetHint("SDL_HINT_WINDOWS_NO_CLOSE_ON_ALT_F4", "1");
SDL_SetHint("SDL_HINT_XINPUT_ENABLED", "0");
TTS::initialize();
vk::ApplicationInfo app_info("Data Structure Analyzer", VK_MAKE_VERSION(0, 1, 0), "DSC", VK_MAKE_VERSION(0, 1, 0), VK_API_VERSION_1_2);
std::vector<const char *> layerNames;
#if !defined(NDEBUG)
layerNames.push_back("VK_LAYER_LUNARG_standard_validation");
#endif
uint32_t extensionCount;
SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
std::vector<const char *> extensionNames(extensionCount);
SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensionNames.data());
vk::InstanceCreateInfo create_info({}, &app_info, static_cast<uint32_t>(layerNames.size()), layerNames.data(), extensionCount, extensionNames.data());
{
scoped_lock<mutex> instance_lock(INSTANCE_MUTEX);
INSTANCE = vk::createInstance(create_info);
VkSurfaceKHR rawSurface;
if (!SDL_Vulkan_CreateSurface(window, INSTANCE, &rawSurface)) {
LOGF << "Cannot create instance surface: " << SDL_GetError();
stringstream ss;
ss << "Cannot create instance surface: " << SDL_GetError();
tinyfd_messageBox("Critical error", ss.str().c_str(), "ok", "error", 1);
return 1;
}
vk::SurfaceKHR surface (rawSurface);
{
scoped_lock<mutex> phys_lock(PHYS_DEV_MUTEX);
PHYS_DEV = INSTANCE.enumeratePhysicalDevices().front();
auto queueFamilyPropertiesList = PHYS_DEV.getQueueFamilyProperties();
auto queueIndex = distance(queueFamilyPropertiesList.begin(), find_if(queueFamilyPropertiesList.begin(), queueFamilyPropertiesList.end(), [](const auto& qfp) {
return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
}));
if (queueIndex > queueFamilyPropertiesList.size()) {
LOGF << "The primary graphics device does not support graphics operations";
tinyfd_messageBox("Critical error", "The primary GPU on this system has no available graphics queues.", "ok", "error", 1);
return 1;
}
auto presentQueueIndex = PHYS_DEV.getSurfaceSupportKHR(static_cast<uint32_t>(queueIndex), surface) ? queueIndex : queueFamilyPropertiesList.size();
if (presentQueueIndex == queueFamilyPropertiesList.size()) {
for (size_t i = 0; i < queueFamilyPropertiesList.size(); i++) {
if ((queueFamilyPropertiesList[i].queueFlags & vk::QueueFlagBits::eGraphics) && PHYS_DEV.getSurfaceSupportKHR(static_cast<uint32_t>( i ), surface)) {
queueIndex = static_cast<uint32_t>(i);
presentQueueIndex = i;
break;
}
}
if (presentQueueIndex == queueFamilyPropertiesList.size()) {
for (size_t i = 0; i < queueFamilyPropertiesList.size(); i++) {
if (PHYS_DEV.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface)) {
presentQueueIndex = i;
break;
}
}
}
}
if ((queueIndex == queueFamilyPropertiesList.size()) || (presentQueueIndex == queueFamilyPropertiesList.size())) {
LOGF << "Cannot find a queue family that supports graphics and presentation";
tinyfd_messageBox("Critical error", "This GPU does not support any queue families that support both graphics and presentation modes", "ok", "error", 1);
return 1;
}
auto queuePriority = 0.5f;
vk::DeviceQueueCreateInfo deviceQueueCreateInfo({}, static_cast<uint32_t>(queueIndex), 1, &queuePriority);
{
scoped_lock<mutex> dev_lock(DEV_MUTEX);
DEVICE = PHYS_DEV.createDevice(vk::DeviceCreateInfo( {}, deviceQueueCreateInfo ) );
{
scoped_lock<mutex> pool_lock(POOL_MUTEX);
CMDPOOL = DEVICE.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), queueIndex));
}
}
}
}
createSampleTree();
return 0;
} catch (std::exception &ex) {
LOGF << "Critical error: " << ex.what();
tinyfd_messageBox("Critical error", ex.what(), "ok", "error", 1);
return 1;
} catch (...) {
LOGF << "Can't continue: unknown error";
tinyfd_messageBox("Critical error", "The application has encountered an unspecified error and must abort execution.", "ok", "error", 1);
return 1;
}
}

void handleNormalExit() {
SDL_Quit();
}

void createSampleTree() {
// For this prototype, we just create an AVL tree.
mt19937 rng;
seed_seq::result_type data[mt19937::state_size];
random_device entropy_source;
generate_n(data, mt19937::state_size, ref(entropy_source));
seed_seq prng_seed(data, data + mt19937::state_size);
rng.seed(prng_seed);
uniform_int_distribution<uint16_t> distribution(0, 65535);
AVLTree tree;
for (int i = 0; i < 100; ++i)
tree.insert(distribution(rng));

TTS::say(fmt::format("Tree created, 65536 items. Height: {}", tree.height()));
constexpr uint8_t TICKS_PER_SECOND = 60;
constexpr uint8_t SKIP_TICKS = 1000 / TICKS_PER_SECOND;
uint64_t next_game_tick = SDL_GetPerformanceCounter();
uint64_t sleep_time = 0;
bool done = false;
while (!done) {
sleep_time = next_game_tick - SDL_GetPerformanceCounter();
if (sleep_time>=0)
std::this_thread::sleep_for(std::chrono::duration<long long, std::milli>(sleep_time));

if (!tree.processEvents())
break;
}
}
