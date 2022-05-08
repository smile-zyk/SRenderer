#ifdef SRENDERERCORE_LIBRARY
#define SRENDERERCORE_EXPORT __declspec(dllexport)
#else
#define SRENDERERCORE_EXPORT __declspec(dllimport)
#endif