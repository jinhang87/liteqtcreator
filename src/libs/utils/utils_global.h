

#pragma once

#include <qglobal.h>

#if defined(CREATOR_UTILS_LIB)
#  define CREATOR_UTILS_EXPORT Q_DECL_EXPORT
#elif  defined(CREATOR_UTILS_STATIC_LIB) // Abuse single files for manual tests
#  define CREATOR_UTILS_EXPORT
#else
#  define CREATOR_UTILS_EXPORT Q_DECL_IMPORT
#endif
