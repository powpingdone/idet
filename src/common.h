#include <cstdio>

#define LOG(message, args...) \
    printf("%s:L%d->%s()# " message "\n", __FILE__, __LINE__, __func__, ##args)
