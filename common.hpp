#pragma once
#include <variant>
#include <cstdint>
#include <functional>
#include <tuple>
#include <mutex>
#include <SDL2/SDL.h>
#include <fmt/core.h>
#include <fmt/format.h>

extern SDL_Window *WINDOW;
static std::mutex WINDOW_MUTEX;

typedef std::variant<std::int8_t, std::int16_t, std::int32_t, std::int64_t, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, signed char, unsigned char, float, double, long double> DsData;

enum class ViewMode {
Graphical,
Textual
};

class DataStructure {
public:
virtual bool insert(const DsData key) { return false; }
virtual void remove(const DsData key)  { }
// Event handlers
// For the values of each parameter (excluding timestamp and window ID) please see
// https://wiki.libsdl.org/SDL_Event and https://wiki.libsdl.org/SDL_EventType (and their
// related fields)
// Note: the SDL_QUIT, SDL_APP_TERMINATING, SDL_APP_LOWMEMORY, SDL_APP_WILLENTERBACKGROUND,
// SDL_APP_DIDENTERBACKGROUND, SDL_APP_WILLENTERFOREGROUND, SDL_APP_DIDENTERFOREGROUND,
// SDL_WINDOWEVENT, SDL_SYSWMEVENT, SDL_TEXTEDITING, SDL_TEXTINPUT, SDL_KEYMAPCHANGED,
// SDL_JOYDEVICEADDED, SDL_JOYDEVICEREMOVED, SDL_CONTROLLERDEVICEADDED,
// SDL_CONTROLLERDEVICEREMOVED, SDL_CONTROLLERDEVICEREMAPPED, SDL_CLIPBOARDUPDATE,
// SDL_DROPFILE, SDL_DROPTEXT, SDL_DROPBEGIN, SDL_DROPCOMPLETE, SDL_AUDIODEVICEADDED,
// SDL_AUDIODEVICEREMOVED, SDL_RENDER_TARGETS_RESET, SDL_RENDER_DEVICE_RESET, and
// SDL_USEREVENT events are not handled by data structures. They will be handled within the
// main application.
// On controller axis motion event. Params: https://wiki.libsdl.org/SDL_ControllerAxisEvent
virtual void on_controller_axis_motion(const std::tuple<const SDL_JoystickID, const std::uint8_t, const std::int16_t>) { }
// Controller button event (either button up or down). Params (type included): https://wiki.libsdl.org/SDL_ControllerButtonEvent
virtual void on_controller_button(const std::tuple<const std::uint32_t, const SDL_JoystickID, const std::uint8_t, const std::uint8_t>) { }
// Complex gesture event. Params (including type): https://wiki.libsdl.org/SDL_DollarGestureEvent
virtual void on_complex_gesture(const std::tuple<const std::uint32_t, const SDL_TouchID, const SDL_GestureID, const std::uint32_t, const float, const float, const float>) { }
// Touch finger event. Params (including type): https://wiki.libsdl.org/SDL_TouchFingerEvent
virtual void on_finger_touch(const std::tuple<const std::uint32_t, const SDL_TouchID, const SDL_FingerID, const float, const float, const float, const float, const float>) { }
// Multi-gesture event. Params: https://wiki.libsdl.org/SDL_MultiGestureEvent
virtual void on_multi_gesture(const std::tuple<const SDL_TouchID, const float, const float, const float, const float, const std::uint16_t>) { }
// Keyboard event. Params (including type): https://wiki.libsdl.org/SDL_KeyboardEvent. Keycode list: https://wiki.libsdl.org/SDL_Keysym
virtual void on_keyboard(const std::tuple<const std::uint8_t, const std::uint8_t, const SDL_Keycode, const std::uint16_t>) { }
// Mouse motion event. Params: https://wiki.libsdl.org/SDL_MouseMotionEvent
virtual void on_mouse_motion(const std::tuple<const std::uint32_t, const std::uint32_t, const std::int32_t, const std::int32_t, const std::int32_t, const std::int32_t>) { }
// Mouse button event. Params (including type): https://wiki.libsdl.org/SDL_MouseButtonEvent
virtual void on_mouse_button(const std::tuple<const std::uint32_t, const std::uint8_t, const std::uint8_t, const std::uint8_t, const std::int32_t, const std::int32_t>) { }
// Mouse wheel event. Params: https://wiki.libsdl.org/SDL_MouseWheelEvent
virtual void on_mouse_wheel(const std::tuple<const std::uint32_t, const std::int32_t, const std::int32_t, const std::uint32_t>) { }
// Joystick axis motion event. Params: https://wiki.libsdl.org/SDL_JoyAxisEvent
virtual void on_joystick_axis_motion(const std::tuple<const SDL_JoystickID, const std::uint8_t, const std::int16_t>) { }
// Joy ball motion event. Params: https://wiki.libsdl.org/SDL_JoyBallEvent
virtual void on_joystick_trackball_motion(const std::tuple<const SDL_JoystickID, const std::uint8_t, const std::int16_t, const std::int16_t>) { }
// Joystick hat motion event. Params: https://wiki.libsdl.org/SDL_JoyHatEvent
virtual void on_joystick_hat_motion(const std::tuple<const SDL_JoystickID, const std::uint8_t, const std::uint8_t>) { }
// Joystick button event. Params (including type): https://wiki.libsdl.org/SDL_JoyButtonEvent
virtual void on_joystick_button(const std::tuple<const SDL_JoystickID, const std::uint8_t, const std::uint8_t>) { }
// Draws the tree
void draw();
};

inline const char* get_node_type_str(const DsData d) {
switch (d.index()) {
case 0: return "Signed 8-bit integer";
case 1: return "Signed 16-bit integer";
case 2: return "Signed 32-bit integer";
case 3: return "Signed 64-bit integer";
case 4: return "Unsigned 8-bit integer";
case 5: return "Unsigned 16-bit integer";
case 6: return "Unsigned 32-bit integer";
case 7: return "Unsigned 64-bit integer";
case 8: return "Signed character";
case 9: return "Unsigned character";
case 10: return "Single-precision floating point";
case 11: return "Double-precision floating point";
case 12: return "Extended-precision floating point";
default: return "Unknown";
}
}

inline std::string get_node_data(const DsData d) {
fmt::memory_buffer buf;
switch (d.index()) {
case 0:
fmt::format_to(buf, "{:d}", std::get<0>(d));
break;
case 1:
fmt::format_to(buf, "{:d}", std::get<1>(d));
break;
case 2:
fmt::format_to(buf, "{:d}", std::get<2>(d));
break;
case 3:
fmt::format_to(buf, "{:d}", std::get<3>(d));
break;
case 4:
fmt::format_to(buf, "{:d}", std::get<4>(d));
break;
case 5:
fmt::format_to(buf, "{:d}", std::get<5>(d));
break;
case 6:
fmt::format_to(buf, "{:d}", std::get<6>(d));
break;
case 7:
fmt::format_to(buf, "{:d}", std::get<7>(d));
break;
case 8:
fmt::format_to(buf, "{}", std::get<8>(d));
break;
case 9:
fmt::format_to(buf, "{}", std::get<9>(d));
break;
case 10:
fmt::format_to(buf, "{:F}", std::get<10>(d));
break;
case 11:
fmt::format_to(buf, "{:F}", std::get<11>(d));
break;
case 12:
fmt::format_to(buf, "{:F}", std::get<12>(d));
break;
default:
fmt::format_to(buf, "Unknown");
break;
}
return buf.data();
};
