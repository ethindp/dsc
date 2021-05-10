#include "tts.h"

namespace tts {
#if (BOOST_OS_WINDOWS==1)
const char * TolkLoadException::what() const throw () {
return "Tolk load error. Accessibility library not loaded.";
}

const char* TolkNoOutputAvailableException::what() const throw() {
return "No TTS output method is available.";
}

const char* TolkNotLoadedException::what() const throw() {
return "Tolk not loaded.";
}
#else
QTextToSpeech* tts;
#endif
void initialize() {
#if (BOOST_OS_WINDOWS==1)
Tolk_Load();
if (!Tolk_IsLoaded()) {
throw TolkLoadException();
}
Tolk_TrySAPI(true);
if (!Tolk_DetectScreenReader()) {
throw TolkNoOutputAvailableException();
}
#else
tts = new QTextToSpeech(nullptr);
#endif
}

void shutdown() {
#if (BOOST_OS_WINDOWS==1)
if (!Tolk_IsLoaded()) {
throw TolkNotLoadedException();
}
#else
delete tts;
#endif
}

void say(const std::string_view &text, const bool &interrupt) {
#if (BOOST_OS_WINDOWS == 1)
if (Tolk_IsLoaded()) {
std::wstring str;
str.assign(text.begin(), text.end());
if (Tolk_HasSpeech()&&Tolk_HasBraille()) {
if (text.empty())
return;
Tolk_Output(str.c_str(), interrupt);
Tolk_Braille(str.c_str());
} else if (Tolk_HasSpeech()) {
if (text.empty())
return;
Tolk_Output(str.c_str(), interrupt);
} else if (Tolk_HasBraille()) {
if (text.empty())
return;
Tolk_Braille(str.c_str());
}
}
#else
if (interrupt)
tts->stop();

tts->say(text.data());
#endif
}
}
