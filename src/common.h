#ifndef COMMON_H
#define COMMON_H

#include <chrono>
#include <cstdio>

extern const std::chrono::time_point<std::chrono::system_clock> pt; // point since startup of program exec
extern bool                                                     enableDebug; // --debug

// current format: "[time] file:LLineNumber->function() message\n"
#define LOG(message, args...)                                                                                       \
    printf("[%f] %s:L%d->%s()# " message "\n",                                                                      \
        std::chrono::duration<double>(std::chrono::system_clock::now() - pt).count(), __FILE__, __LINE__, __func__, \
        ##args)

#define DLOG(message, args...) \
    if(enableDebug)            \
    LOG("[DEBUG] " message, ##args)

#endif
