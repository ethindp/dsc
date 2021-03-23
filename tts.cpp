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
SPDConnection *sockfd;
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
sockfd = spd_open("mapclient", NULL, NULL, SPD_MODE_THREADED);
if (sockfd==0) {
throw SPDInitException("Speech Dispatcher initialization failed.");
}
#endif
}

void shutdown() {
#if (BOOST_OS_WINDOWS==1)
if (!Tolk_IsLoaded()) {
throw TolkNotLoadedException();
}
#else
spd_close(sockfd);
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
if (!interrupt) {
if (text.empty())
return;
int ret = spd_say(sockfd, SPD_MESSAGE, text.c_str());
if (ret==-1) {
throw SPDSpeakException("An internal Speech Dispatcher error has occured. Please ensure that speech dispatcher is configured correctly.");
}
} else {
if (text.empty())
return;
int ret = spd_stop(sockfd);
if (ret==-1) {
throw SPDSpeakException("An internal Speech Dispatcher error has occured. Please ensure that speech dispatcher is configured correctly.");
}
ret = spd_say(sockfd, SPD_MESSAGE, text.c_str());
if (ret==-1) {
throw SPDSpeakException("An internal Speech Dispatcher error has occured. Please ensure that speech dispatcher is configured correctly.");
}
}
#endif
}
}
