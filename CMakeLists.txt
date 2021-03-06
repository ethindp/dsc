cmake_minimum_required(VERSION 3.19)
project ("DSC")
include(FetchContent)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_LIST_DIR}/cmake")
file(GLOB SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/gl.c")
if(WIN32)
add_executable(dsc WIN32 ${SRC})
else()
add_executable(dsc ${SRC})
endif()
if(WIN32)
message(STATUS "Building for windows, linking with Tolk")
target_link_libraries(dsc PRIVATE tolk.lib)
else()
message(STATUS "Building for non-windows platform, looking for QTSpeech")
message(STATUS "Searching for QT core...")
find_package(Qt5Core CONFIG REQUIRED)
target_link_libraries(dsc PRIVATE Qt::Core Qt5::Core Qt::CorePrivate Qt5::CorePrivate)
message(STATUS "Searching for QT TTS...")
find_package(Qt5TextToSpeech CONFIG REQUIRED)
target_link_libraries(dsc PRIVATE Qt::TextToSpeech Qt5::TextToSpeech Qt::TextToSpeechPrivate Qt5::TextToSpeechPrivate)
endif()
if(WIN32)
target_compile_definitions(dsc PRIVATE _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING _CRT_SECURE_NO_WARNINGS _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS _UNICODE HAVE_SNPRINTF)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
target_compile_options(dsc PRIVATE -Wall -Wextra -fopenmp -march=native -mtune=native)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
target_compile_options(dsc PRIVATE /openmp /W4 /wd4100 /bigobj /MP /arch:AVX2 /fp:strict /GF /Qpar /Qspectre /Zf /utf-8)
if(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo" OR CMAKE_BUILD_TYPE MATCHES "Debug")
target_compile_options(dsc PRIVATE /GS /Gs /GT /guard:cf /Zf /Zi /sdl)
endif()
endif()
message(STATUS "Searching for OpenGL...")
find_package(OpenGL COMPONENTS OpenGL  REQUIRED)
target_link_libraries(dsc PRIVATE OpenGL::GL OpenGL::OpenGL)
message(STATUS "Searching for fmt...")
find_package(fmt CONFIG REQUIRED)
target_link_libraries(dsc PRIVATE fmt::fmt)
message(STATUS "Searching for freetype...")
find_package(Freetype REQUIRED FATAL_ERROR)
target_link_libraries(dsc PRIVATE Freetype::Freetype)
message(STATUS "Searching for plog...")
find_path(PLOG_INCLUDE_DIRS "plog/Appenders/AndroidAppender.h")
target_include_directories(dsc PRIVATE ${PLOG_INCLUDE_DIRS})
message(STATUS "Searching for SFML...")
find_package(SFML COMPONENTS system window network audio graphics CONFIG REQUIRED)
target_link_libraries(dsc PRIVATE sfml-system sfml-network sfml-graphics sfml-window sfml-audio)
message(STATUS "Searching for tgui...")
set(TGUI_STATIC_LIBRARIES TRUE)
find_package(TGUI CONFIG REQUIRED)
target_link_libraries(dsc PRIVATE tgui)

