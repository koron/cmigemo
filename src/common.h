// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// common.h - To inject shared code into all .c files, such as for debugging
//            purposes.

#pragma once

// Example for heap debugging on Windows. You can enable it by changing the '0'
// below to '1'.
#if 0
# if defined(_WIN32) && defined(_DEBUG)
#  define _CRTDBG_MAP_ALLOC
#  include <crtdbg.h>
#  include <stdlib.h>
# endif
#endif
