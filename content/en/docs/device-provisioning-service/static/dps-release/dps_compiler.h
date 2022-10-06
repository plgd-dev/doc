/****************************************************************************
 *
 * Copyright (C) 2022 plgd.dev, s.r.o. - All Rights Reserved
 * Unauthorized distribution of this library is strictly prohibited
 * Proprietary and confidential
 *
 ****************************************************************************/

#ifndef PLGD_DPS_COMPILER_H
#define PLGD_DPS_COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define DPS_EXPORT __declspec(dllexport)
#define DPS_IMPORT __declspec(dllimport)
#elif (defined __GNUC__ && __GNUC__ >= 4)
#define DPS_EXPORT __attribute__((visibility("default")))
#define DPS_IMPORT
#else /* !__GNUC__ || __GNUC__ < 4 */
#warning "Shared libraries not supported"
#define DPS_EXPORT
#define DPS_IMPORT
#endif /* _WIN32 */

#ifdef DPS_SHARED_LIBRARY
#ifdef DPS_SHARED_LIBRARY_EXPORT
#define DPS_API OC_EXPORT
#else /* !DPS_SHARED_LIBRARY_EXPORT*/
#define DPS_API OC_IMPORT
#endif /* DPS_SHARED_LIBRARY_EXPORT */
#else  /* !DPS_SHARED_LIBRARY */
#define DPS_API
#endif /* DPS_SHARED_LIBRARY */

#if defined(__GNUC__) || defined(__clang__)
#define DPS_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define DPS_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#define DPS_UNREACHABLE __builtin_unreachable()
#else /* !GNUC && !__clang__ */
#define DPS_NONNULL(...)
#define DPS_WARN_UNUSED_RESULT
#define DPS_UNREACHABLE
#endif /* GNUC || __clang__ */

#ifdef __cplusplus
}
#endif

#endif /* PLGD_DPS_COMPILER_H */
