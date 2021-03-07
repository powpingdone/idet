#ifndef COMMON_H
#define COMMON_H

#include <chrono>
#include <cstdio>

extern const std::chrono::time_point<std::chrono::system_clock> pt;

#define LOG(message, args...)                                                                                       \
    printf("[%f] %s:L%d->%s()# " message "\n",                                                                      \
        std::chrono::duration<double>(std::chrono::system_clock::now() - pt).count(), __FILE__, __LINE__, __func__, \
        ##args)

#endif
