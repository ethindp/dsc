#pragma once
#include <string_view>
#include <boost/predef.h>
#if (BOOST_OS_WINDOWS==1)
#include "tolk.h"
#else
#include <cstdlib>
#include <speechd_types.h>
#include <libspeechd.h>
#endif
#include <exception>

namespace tts {
#if (BOOST_OS_WINDOWS==1)
class TolkLoadException : public std::exception {
public:
const char * what () const throw ();
};

class TolkNoOutputAvailableException : public std::exception {
public:
const char * what () const throw ();
};

class TolkNotLoadedException : public std::exception {
public:
const char * what () const throw ();
};
#else
extern SPDConnection *sockfd;
class SPDInitException : public std::exception {
public:
explicit SPDInitException (char* msg) : std::exception(msg) { }
};

class SPDSpeakException : public std::exception {
public:
explicit SPDSpeakException(char* message) : std::exception(message) { }
};
#endif
void initialize();
void shutdown();
void say(const std::string_view &text, const bool &interrupt = true);
}
