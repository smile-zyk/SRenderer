#ifdef _WIN32

#ifdef SRENDERERCORE_LIBRARY
#define SRENDERERCORE_EXPORT __declspec(dllexport)
#else
#define SRENDERERCORE_EXPORT __declspec(dllimport)
#endif

#else
#define SRENDERERCORE_EXPORT

#endif
