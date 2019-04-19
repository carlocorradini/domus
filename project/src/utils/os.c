
#include "utils/os.h"

static bool win_32_os = false;
static bool win_64_os = false;
static bool mac_os = false;
static bool linux_os = false;

#ifdef _WIN32
#ifdef _WIN64
win_64_os = true;
#else
win_32_os = true;
#endif
#elif __APPLE__
mac_os = true;
#elif __linux__
linux_os = true;
#endif

bool os_windows(void) {
    return win_32_os || win_64_os;
}

bool os_windows_32(void) {
    return win_32_os;
}

bool os_windows_64(void) {
    return win_64_os;
}

bool os_mac(void) {
    return mac_os;
}

bool os_linux(void) {
    return linux_os;
}