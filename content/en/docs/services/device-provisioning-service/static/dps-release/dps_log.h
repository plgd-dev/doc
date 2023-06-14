/**
 * @file dps_log.h
 *
 * Global logging functions.
 *
 *
 * Copyright (C) 2022 plgd.dev, s.r.o. - All Rights Reserved
 *
 * Unauthorized distribution of this library is strictly prohibited
 *
 * Proprietary and confidential
 */

#ifndef PLGD_DPS_LOG_H
#define PLGD_DPS_LOG_H

#include "dps_compiler.h"
#include "oc_clock_util.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#ifndef __FILENAME__
#ifdef WIN32
// NOLINTNEXTLINE(bugprone-reserved-identifier)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
// NOLINTNEXTLINE(bugprone-reserved-identifier)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#endif /* !__FILENAME__ */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  DPS_LOG_LEVEL_DEBUG,
  DPS_LOG_LEVEL_WARNING,
  DPS_LOG_LEVEL_INFO,
  DPS_LOG_LEVEL_ERROR,
} plgd_dps_log_level_t;

typedef void (*plgd_dps_print_log_fn_t)(plgd_dps_log_level_t log_level, const char *format, ...);

/// @brief Set global logging function
DPS_EXPORT
void plgd_dps_set_log_fn(plgd_dps_print_log_fn_t log_fn);

/// @brief Get global logging function
DPS_EXPORT
plgd_dps_print_log_fn_t plgd_dps_get_log_fn(void);

#define DPS_LOG(log_level, ...)                                                                                        \
  do {                                                                                                                 \
    plgd_dps_print_log_fn_t _dps_log_fn = plgd_dps_get_log_fn();                                                       \
    if (_dps_log_fn != NULL) {                                                                                         \
      _dps_log_fn((log_level), __VA_ARGS__);                                                                           \
    } else {                                                                                                           \
      char dps_log_fn_buf[64] = { 0 };                                                                                 \
      oc_clock_time_rfc3339(dps_log_fn_buf, sizeof(dps_log_fn_buf));                                                   \
      const char *log_level_str = "D";                                                                                 \
      switch (log_level) {                                                                                             \
      case DPS_LOG_LEVEL_DEBUG:                                                                                        \
        log_level_str = "D";                                                                                           \
        break;                                                                                                         \
      case DPS_LOG_LEVEL_INFO:                                                                                         \
        log_level_str = "I";                                                                                           \
        break;                                                                                                         \
      case DPS_LOG_LEVEL_WARNING:                                                                                      \
        log_level_str = "W";                                                                                           \
        break;                                                                                                         \
      case DPS_LOG_LEVEL_ERROR:                                                                                        \
        log_level_str = "E";                                                                                           \
        break;                                                                                                         \
      }                                                                                                                \
      printf("[DPS %s] %s: %s <%s:%d>: ", dps_log_fn_buf, log_level_str, __FILENAME__, __func__, __LINE__);            \
      printf(__VA_ARGS__);                                                                                             \
      printf("\n");                                                                                                    \
    }                                                                                                                  \
  } while (0)

#ifdef DPS_DBG_ENABLED
#define DPS_DBG(...) DPS_LOG(DPS_LOG_LEVEL_DEBUG, __VA_ARGS__)
#else /* !DPS_DBG_ENABLED */
#define DPS_DBG(...)
#endif /* DPS_DBG_ENABLED */
#define DPS_INFO(...) DPS_LOG(DPS_LOG_LEVEL_INFO, __VA_ARGS__)
#define DPS_WRN(...) DPS_LOG(DPS_LOG_LEVEL_WARNING, __VA_ARGS__)
#define DPS_ERR(...) DPS_LOG(DPS_LOG_LEVEL_ERROR, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* PLGD_DPS_LOG_H */
