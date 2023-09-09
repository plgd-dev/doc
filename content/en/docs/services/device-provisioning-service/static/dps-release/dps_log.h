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

#include "plgd/dps_compiler.h"

#include <oc_log.h>
#include <util/oc_compiler.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Log component determines the source of the message.
 */
typedef enum {
  PLGD_DPS_LOG_COMPONENT_DEFAULT = 1 << 0, ///< default, non-specific component
} plgd_dps_log_component_t;

/**
 * @brief Custom logging function
 *
 * @param level log level of the message
 * @param component log component of the message
 * @param file file of the log message call
 * @param line line of the log message call in \p file
 * @param func_name function name in which the log message call is invoked
 * @param format format of the log message
 */
typedef void (*plgd_dps_print_log_fn_t)(oc_log_level_t level, plgd_dps_log_component_t component, const char *file,
                                        int line, const char *func_name, const char *format, ...) OC_PRINTF_FORMAT(6, 7)
  OC_NONNULL();

/// @brief Set global logging function
DPS_EXPORT
void plgd_dps_set_log_fn(plgd_dps_print_log_fn_t log_fn);

/// @brief Get global logging function
DPS_EXPORT
plgd_dps_print_log_fn_t plgd_dps_get_log_fn(void) OC_RETURNS_NONNULL;

/**
 * @brief Set log level of the global logger, logs with lower importance will be
 * ignored. It is thread safe.
 *
 * @param level Log level
 * @note If log level is not set, the default log level is OC_LOG_LEVEL_INFO.
 */
DPS_EXPORT
void plgd_dps_log_set_level(oc_log_level_t level);

/**
 * @brief Get log level of the global logger. It is thread safe.
 *
 * @return Log level
 */
DPS_EXPORT
oc_log_level_t plgd_dps_log_get_level(void);

/**
 * @brief Set enabled log components of the global logger. It is thread safe.
 *
 * @param components mask of enabled log components
 */
DPS_EXPORT
void plgd_dps_log_set_components(uint32_t components);

/**
 * @brief Get enabled log components of the global logger. It is thread safe.
 *
 * @return uint32_t mask of enabled log components
 */
DPS_EXPORT
uint32_t plgd_dps_log_get_components(void);

#ifdef __cplusplus
}
#endif

#endif /* PLGD_DPS_LOG_H */
