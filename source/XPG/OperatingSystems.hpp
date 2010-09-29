/**
 *  The OS-specific macro names are random, irregular, and annoying to deal
 *  with. This file is an attempt to unify all those macros.
 */

#ifndef XPGH_OPERATINGSYSTEMS
#define XPGH_OPERATINGSYSTEMS

#include <cstdlib>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   define XPG_OS_STRING "Windows"
#   define XPG_OS_WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <windowsx.h>
#else
#   define XPG_OS_UNIX
#   define XPG_UNIX_HOME getenv("HOME")
#   if defined(__APPLE__) || defined(__APPLE_CC__)
#       define XPG_OS_STRING "OSX"
#       define XPG_OS_OSX
#   else
#       define XPG_OS_STRING "Linux"
#       define XPG_OS_LINUX
#   endif
#endif

#endif
