#ifndef CUSTOM_LIMITS_H
#define CUSTOM_LIMITS_H

    // Define max line length
    #ifndef MAX_LINE_LEN
        #include <limits.h>
        #define MAX_LINE_LEN UCHAR_MAX
    #endif

#endif // CUSTOM_LIMITS_H
