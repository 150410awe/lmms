#ifndef LMNS_INCLUDE_LMNS_EXPORT_HPP
#define LMNS_INCLUDE_LMNS_EXPORT_HPP

#if defined(_WIN32) && defined(LMNS_SHARED_LIBRARY)
#if defined(LMNS_BUILDING_LIBRARY)
#define LMNS_API __declspec(dllexport)
#else
#define LMNS_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) && defined(LMNS_SHARED_LIBRARY)
#define LMNS_API __attribute__((visibility("default")))
#else
#define LMNS_API
#endif

#endif

