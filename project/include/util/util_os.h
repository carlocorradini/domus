
#ifndef _OS_H
#define _OS_H

#include <stdbool.h>

#define WIN_32_OS false
#define WIN_64_OS false
#define MAC_OS_OS false
#define LINUX_OS false
#define UNIX_OS false

#ifdef _WIN32
#ifdef _WIN64
#undef WIN_32_OS
#define WIN_32_OS true
#else
#undef WIN_64_OS
#define WIN_64_OS true
#endif
#elif __APPLE__
#undef MAC_OS_OS
#define MAC_OS_OS true
#elif __linux__
#undef LINUX_OS
#define LINUX_OS true
#elif __unix__
#undef UNIX_OS
#define UNIX_OS true
#endif

/**
 * Check if Operating System is Windows
 * @return true if windows, false otherwise
 */
bool os_windows(void);

/**
 * Check if Operating System is Windows 32bit
 * @return true if windows 32bit, false otherwise
 */
bool os_windows_32(void);

/**
 * Check if Operating System is Windows 64bit
 * @return true if windows 64bit, false otherwise
 */
bool os_windows_64(void);

/**
 * Check if Operating System is macOS
 * @return true if mac, false otherwise
 */
bool os_mac_os(void);

/**
 * Check if Operating System is Linux
 * @return true if linux, false otherwise
 */
bool os_linux(void);

/**
 * Check if Operating System is Unix
 * @return true if Unix, false otherwise
 */
bool os_unix(void);

#endif
