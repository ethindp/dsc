#pragma once
#include <string_view>
#include <boost/predef.h>
#if (BOOST_OS_WINDOWS==1)
#include "tolk.h"
#include <exception>
#else
#include <QTextToSpeech>
#endif

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
extern QTextToSpeech* tts;
#endif
void initialize();
void shutdown();
void say(const std::string_view &text, const bool &interrupt = true);
}
