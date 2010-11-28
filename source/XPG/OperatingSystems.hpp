/**
 *  The OS-specific macro names are random/irregular and annoying to deal
 *  with. XPG unifies them all here and offers up a consistent naming
 *  convention.
 */

#ifndef XPGH_OPERATINGSYSTEMS
#define XPGH_OPERATINGSYSTEMS

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   define XPG_OS_STRING "Windows"
#   define XPG_OS_WINDOWS
#else
#   define XPG_OS_UNIX
#   define XPG_UNIX_HOME getenv("HOME")
#   if defined(__APPLE__) || defined(__APPLE_CC__)
#       define XPG_OS_STRING "OSX"
#       define XPG_OS_OSX
#   elif defined(__ANDROID__) || defined(ANDROID)
#       define XPG_OS_STRING "Android"
#       define XPG_OS_MOBILE
#       define XPG_OS_ANDROID
#   else
#       define XPG_OS_STRING "Linux"
#       define XPG_OS_LINUX
#   endif
#endif

#ifdef XPG_OS_ANDROID
#   include <stdlib.h>
#else
#   include <cstdlib>
#endif

#endif
