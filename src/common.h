#include <cstdio>

#define LOG(message, args...) \
    printf("%s:%s(L%d)# \n" message, __FILE__, __func__, __LINE__, ##args)
