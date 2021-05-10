#include <string>
#include <cstddef>
#include <cstdlib>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <random>
#include <functional>
#include <tuple>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <plog/Log.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <glad/glad.h>
#ifdef _WINDOWS_
#include <windows.h>
#endif
#include "tts.h"
#include "common.hpp"
#include "avl.hpp"
#include "btree.hpp"
#include "rbtree.hpp"
#include "tinyfiledialogs.h"

SDL_Window *WINDOW;
using namespace std;

void APIENTRY opengl_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
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
LOGI << "Loading OpenGL";
if (SDL_GL_LoadLibrary(nullptr)<0) {
LOGF << "Can't load OpenGL: " << SDL_GetError();
tinyfd_messageBox("Critical error", SDL_GetError(), "ok", "error", 1);
return 1;
}
LOGI << "Enabling accelerated visuals";
if (SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1)<0) {
LOGF << "Can't enable hardware acceleration: " << SDL_GetError();
tinyfd_messageBox("Critical error", SDL_GetError(), "ok", "error", 1);
return 1;
}
LOGI << "Enabling double buffering";
if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)<0) {
LOGF << "Can't enable double buffering: " << SDL_GetError();
tinyfd_messageBox("critical error", "This systems OpenGL driver does not support double buffering", "ok", "error", 1);
return 1;
}
LOGI << "Acquiring window mutex";
WINDOW_MUTEX.lock();
LOGI << "Creating window";
WINDOW = SDL_CreateWindow("Data Structure Analyzer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL|SDL_WINDOW_ALLOW_HIGHDPI);
if (!WINDOW) {
LOGF << "Can't create window: " << SDL_GetError();
std::stringstream ss;
ss << "Can't create window: " << SDL_GetError();
tinyfd_messageBox("Critical error", ss.str().c_str(), "ok", "error", 1);
return 1;
}
LOGI << "Creating OpenGL context";
auto gl_context = SDL_GL_CreateContext(WINDOW);
if (!gl_context) {
LOGF << "Can't create OpenGL context: " << SDL_GetError();
tinyfd_messageBox("Can't create OpenGL context", SDL_GetError(), "ok", "error", 1);
return 1;
}
LOGI << "Setting new context to current";
if (SDL_GL_MakeCurrent(WINDOW, gl_context)<0) {
LOGF << "Can't set GLcontext to current: " << SDL_GetError();
return tinyfd_messageBox("Error", "There was an error while creating the application window. Check the log for more information.", "ok", "error", 1);
}
LOGI << "Loading OpenGL extensions";
if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
LOGW << "No extensions could be loaded";
}
int major = 0;
int minor = 0;
SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
LOGI << "Initialized OpenGL v. " << major << "." << minor;
if (SDL_GL_ExtensionSupported("KHR_debug")||SDL_GL_ExtensionSupported("GL_KHR_debug")) {
LOGI << "Context debugging supported; enabling synchronous output";
glEnable(GL_DEBUG_OUTPUT);
glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
glDebugMessageCallback(opengl_callback, nullptr);
glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
} else {
LOGW << "This OpenGL context does not support the KHR_debug or GL_KHR_debug extension. Moving error checking to internal loop.";
}
LOGI << "Enabling framebuffer acceleration";
SDL_SetHint("SDL_HINT_FRAMEBUFFER_ACCELERATION", "1");
LOGI << "Enabling IME internal editing";
SDL_SetHint("SDL_HINT_IME_INTERNAL_EDITING", "1");
LOGI << "Disabling background joystick events";
SDL_SetHint("SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS", "0");
LOGI << "Setting render scale quality to 1";
SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "1");
LOGI << "Disabling close on alt-f4 on Windows";
SDL_SetHint("SDL_HINT_WINDOWS_NO_CLOSE_ON_ALT_F4", "1");
LOGI << "Disabling XInput";
SDL_SetHint("SDL_HINT_XINPUT_ENABLED", "0");
LOGI << "Enabling OpenGL shader rendering";
SDL_SetHint("SDL_HINT_RENDER_OPENGL_SHADERS", "1");
LOGI << "Setting OpenGL swap interval to 1";
SDL_GL_SetSwapInterval(1);
int w = 0;
int h = 0;
SDL_GetWindowSize(WINDOW, &w, &h);
LOGI << "Window size: " << h << "x" << w;
LOGI << "Releasing window mutex";
WINDOW_MUTEX.unlock();
LOGI << "Setting OpenGL viewport to window dimensions";
glViewport(0, 0, w, h);
LOGI << "Clearing color to black";
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
LOGI << "Requesting immediate command processing for enqueued OpenGL commands";
glFinish();
LOGI << "Initializing TTS engine";
tts::initialize();
LOGI << "Initializing TTF engine";
if (TTF_Init()<0) {
LOGF << "Failed to initialize TTF subsystem: " << SDL_GetError();
return tinyfd_messageBox("Error", "There was an error while initializing the font subsystem. Check the log for more information.", "ok", "error", 1);
}
LOGI << "Creating sample tree";
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
if (TTF_WasInit())
TTF_Quit();

if (SDL_WasInit(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS))
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
uniform_real_distribution<double> distribution(0.0, 1000.0);
AVLTree tree;
for (int i = 0; i < 10; ++i)
tree.insert(distribution(rng));

tts::say(fmt::format("Tree created, 10 items. Height: {}", tree.height()));
constexpr uint8_t TICKS_PER_SECOND = 60;
constexpr uint8_t SKIP_TICKS = 1000 / TICKS_PER_SECOND;
uint64_t next_game_tick = SDL_GetPerformanceCounter();
uint64_t sleep_time = SKIP_TICKS;
bool done = false;
while (!done) {
sleep_time = next_game_tick - SDL_GetPerformanceCounter();
if (sleep_time > 0)
std::this_thread::sleep_for(std::chrono::duration<long long, std::milli>(sleep_time));
if (!SDL_GL_ExtensionSupported("KHR_debug") || !SDL_GL_ExtensionSupported("GL_KHR_debug")) {
auto gl_err = glGetError();
while (gl_err != GL_NO_ERROR) {
if (gl_err==GL_INVALID_ENUM)
LOGW << "GL: unacceptable enumerated value; ignored";
else if (gl_err == GL_INVALID_VALUE)
LOGW << "GL: out-of-range argument; ignored";
else if (gl_err == GL_INVALID_OPERATION)
LOGW << "GL: disallowed argument in current state; ignored";
else if (gl_err == GL_INVALID_FRAMEBUFFER_OPERATION)
LOGW << "GL: framebuffer object incomplete; command ignored";
else if (gl_err == GL_OUT_OF_MEMORY) {
LOGF << "GL: out of memory";
tinyfd_messageBox("Critical error", "Not enough memory to continue", "ok", "error", 1);
std::terminate();
} else if (gl_err == GL_STACK_UNDERFLOW)
LOGW << "GL: attempt to cause internal stack underflow";
else if (gl_err == GL_STACK_OVERFLOW)
LOGW << "GL: attempt to cause internal stack overflow";
else
LOGW << "GL: error unknown, code " << gl_err;
gl_err = glGetError();
}
}
tree.draw();
SDL_Event e;
if (SDL_PollEvent(&e))
switch (e.type) {
case SDL_CONTROLLERAXISMOTION:
tree.on_controller_axis_motion(std::make_tuple(e.caxis.which, e.caxis.axis, e.caxis.value));
break;
case SDL_CONTROLLERBUTTONDOWN:
case SDL_CONTROLLERBUTTONUP:
tree.on_controller_button(std::make_tuple(e.cbutton.type, e.cbutton.which, e.cbutton.button, e.cbutton.state));
break;
case SDL_DOLLARGESTURE:
case SDL_DOLLARRECORD:
tree.on_complex_gesture(std::make_tuple(e.dgesture.type, e.dgesture.touchId, e.dgesture.gestureId, e.dgesture.numFingers, e.dgesture.error, e.dgesture.x, e.dgesture.y));
break;
case SDL_FINGERMOTION:
case SDL_FINGERDOWN:
case SDL_FINGERUP:
tree.on_finger_touch(std::make_tuple(e.tfinger.type, e.tfinger.touchId, e.tfinger.fingerId, e.tfinger.x, e.tfinger.y, e.tfinger.dx, e.tfinger.dy, e.tfinger.pressure));
break;
case SDL_KEYDOWN:
case SDL_KEYUP:
tree.on_keyboard(std::make_tuple(e.key.state, e.key.repeat, e.key.keysym.sym, e.key.keysym.mod));
break;
case SDL_JOYAXISMOTION:
tree.on_joystick_axis_motion(std::make_tuple(e.jaxis.which, e.jaxis.axis, e.jaxis.value));
break;
case SDL_JOYBALLMOTION:
tree.on_joystick_trackball_motion(std::make_tuple(e.jball.which, e.jball.ball, e.jball.xrel, e.jball.yrel));
break;
case SDL_JOYHATMOTION:
tree.on_joystick_hat_motion(std::make_tuple(e.jhat.which, e.jhat.hat, e.jhat.value));
break;
case SDL_JOYBUTTONDOWN:
case SDL_JOYBUTTONUP:
tree.on_joystick_button(std::make_tuple(e.jbutton.which, e.jbutton.button, e.jbutton.state));
break;
case SDL_MOUSEMOTION:
tree.on_mouse_motion(std::make_tuple(e.motion.which, e.motion.state, e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel));
break;
case SDL_MOUSEBUTTONDOWN:
case SDL_MOUSEBUTTONUP:
tree.on_mouse_button(std::make_tuple(e.button.which, e.button.button, e.button.state, e.button.clicks, e.button.x, e.button.y));
break;
case SDL_MOUSEWHEEL:
tree.on_mouse_wheel(std::make_tuple(e.wheel.which, e.wheel.x, e.wheel.y, e.wheel.direction));
break;
case SDL_MULTIGESTURE:
tree.on_multi_gesture(std::make_tuple(e.mgesture.touchId, e.mgesture.dTheta, e.mgesture.dDist, e.mgesture.x, e.mgesture.y, e.mgesture.numFingers));
break;
case SDL_QUIT:
std::exit(0);
break;
default:
break;
}
}
}

void APIENTRY opengl_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
if (severity==GL_DEBUG_SEVERITY_HIGH) {
if (type==GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) {
LOGF << "Undefined behavior! Source: " << source << ", ID: " << id << ": " << message;
tinyfd_messageBox("Critical alert!", "The graphics subsystem has entered an unstable state. To avoid corruption or dangerous behavior that may be caused due to the instability of the graphics subsystem after this point, this program must terminate. Click OK to terminate the application.", "ok", "error", 1);
std::exit(1);
}
LOGE << "OpenGL: (" << id << "): " << message;
} else if (severity==GL_DEBUG_SEVERITY_MEDIUM) {
LOGW << "OpenGL: (" << id << "): " << message;
} else if (severity==GL_DEBUG_SEVERITY_LOW) {
LOGW << "OpenGL (low): (" << id << "): " << message;
} else if (severity==GL_DEBUG_SEVERITY_NOTIFICATION) {
LOGD << "OpenGL: (" << id << "): " << message;
}
}
