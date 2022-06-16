/****************************************************************************
 *
 * Copyright (C) 2022 plgd.dev, s.r.o. - All Rights Reserved
 * Unauthorized distribution of this library is strictly prohibited
 * Proprietary and confidential
 *
 ****************************************************************************/

#ifndef PLGD_DPS_LOG_H
#define PLGD_DPS_LOG_H

#include "dps_export.h"
#include <stdio.h>
#include <time.h>

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
void plgd_dps_set_log_fn(plgd_dps_print_log_fn_t fn);

/// @brief Get global logging function
DPS_EXPORT
plgd_dps_print_log_fn_t plgd_dps_get_log_fn(void);

#define DPS_LOG(log_level, ...)                                                                                        \
  do {                                                                                                                 \
    plgd_dps_print_log_fn_t _dps_log_fn = plgd_dps_get_log_fn();                                                       \
    if (_dps_log_fn != NULL) {                                                                                         \
      _dps_log_fn((log_level), __VA_ARGS__);                                                                           \
    } else {                                                                                                           \
      time_t ltime = time(NULL);                                                                                       \
      printf("[DPS:%ld] %s <%s:%d>: ", (long)ltime, __FILENAME__, __func__, __LINE__);                                 \
      printf(__VA_ARGS__);                                                                                             \
      printf("\n");                                                                                                    \
    }                                                                                                                  \
  } while (0)

#ifdef DEBUG
#define DPS_DBG(...) DPS_LOG(DPS_LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define DPS_DBG(...)
#endif
#define DPS_INFO(...) DPS_LOG(DPS_LOG_LEVEL_INFO, __VA_ARGS__)
#define DPS_WRN(...) DPS_LOG(DPS_LOG_LEVEL_WARNING, __VA_ARGS__)
#define DPS_ERR(...) DPS_LOG(DPS_LOG_LEVEL_ERROR, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* PLGD_DPS_LOG_H */
