/**
 * @file dps.h
 *
 * Device provisioning service API functions.
 *
 *
 * Copyright (C) 2022 plgd.dev, s.r.o. - All Rights Reserved
 *
 * Unauthorized distribution of this library is strictly prohibited
 *
 * Proprietary and confidential
 */

#ifndef PLGD_DPS_H
#define PLGD_DPS_H

#include "oc_config.h"

#ifndef OC_SECURITY
#error "OC_SECURITY must be defined"
#endif

#ifndef OC_PKI
#error "OC_PKI must be defined"
#endif

#ifndef OC_CLOUD
#error "OC_CLOUD must be defined"
#endif

#ifndef OC_IPV4
#error "OC_IPV4 must be defined"
#endif

#ifndef OC_STORAGE
#error "OC_STORAGE must be defined"
#endif

#include "dps_compiler.h"
#include "oc_client_state.h"
#include "oc_cloud.h"
#include "oc_ri.h"
#include "oc_session_events.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <mbedtls/md.h>

#ifdef __cplusplus
extern "C" {
#endif

struct plgd_dps_context_t;
typedef struct plgd_dps_context_t plgd_dps_context_t;

/**
 * @brief DPS provisioning status flags.
 */
typedef enum {
  /* UNINITIALIZED = 0 */
  PLGD_DPS_INITIALIZED = 1 << 0,
  PLGD_DPS_GET_CREDENTIALS = 1 << 1,
  PLGD_DPS_HAS_CREDENTIALS = 1 << 2,
  PLGD_DPS_GET_ACLS = 1 << 3,
  PLGD_DPS_HAS_ACLS = 1 << 4,
  PLGD_DPS_GET_CLOUD = 1 << 6,
  PLGD_DPS_HAS_CLOUD = 1 << 7,
  PLGD_DPS_CLOUD_STARTED = 1 << 8,
  PLGD_DPS_RENEW_CREDENTIALS = 1 << 9,
  PLGD_DPS_GET_OWNER = 1 << 10,
  PLGD_DPS_HAS_OWNER = 1 << 11,
  PLGD_DPS_GET_TIME = 1 << 12,
  PLGD_DPS_HAS_TIME = 1 << 13,
  PLGD_DPS_TRANSIENT_FAILURE = 1 << 29,
  PLGD_DPS_FAILURE = 1 << 30,
} plgd_dps_status_t;

/**
 * @brief DPS errors.
 */
typedef enum {
  PLGD_DPS_OK = 0,
  PLGD_DPS_ERROR_RESPONSE = 1,
  PLGD_DPS_ERROR_CONNECT = 2,
  PLGD_DPS_ERROR_GET_CREDENTIALS = 3,
  PLGD_DPS_ERROR_GET_ACLS = 4,
  PLGD_DPS_ERROR_SET_CLOUD = 5,
  PLGD_DPS_ERROR_START_CLOUD = 6,
  PLGD_DPS_ERROR_GET_OWNER = 7,
  PLGD_DPS_ERROR_GET_TIME = 8,
} plgd_dps_error_t;

/**
  @brief A function pointer for handling the dps status.
  @param ctx dps context
  @param status Current status of the dps.
  @param data user data provided to the callback
*/
typedef void (*plgd_dps_on_status_change_cb_t)(plgd_dps_context_t *ctx, plgd_dps_status_t status, void *data);

/**
 * @brief Allocate and initialize data.
 *
 * @return int  0   on success
 *              <0  on failure
 */
DPS_EXPORT
int plgd_dps_init(void);

/**
 * @brief Stop all devices and deallocate data.
 */
DPS_EXPORT
void plgd_dps_shutdown(void);

/// Get context for given device
DPS_EXPORT
plgd_dps_context_t *plgd_dps_get_context(size_t device);

/**
 * @brief Get device from context.
 *
 * @param ctx dps context (cannot be NULL)
 *
 * @return size_t index of device
 */
DPS_EXPORT
size_t plgd_dps_get_device(const plgd_dps_context_t *ctx) DPS_NONNULL();

typedef struct
{
  plgd_dps_on_status_change_cb_t on_status_change; ///< callback executed on DPS status change
  void *on_status_change_data;                     ///< user data provided to DPS status change callback
  oc_cloud_cb_t on_cloud_status_change;            ///< callback executed when cloud status change
  void *on_cloud_status_change_data;               ///< user data provided to cloud status change callback
} plgd_dps_manager_callbacks_t;

/**
 * @brief Set DPS manager callbacks.
 *
 * @param ctx dps context (cannot be NULL)
 * @param callbacks callbacks with data
 *
 * Example of plgd_dps_on_status_change_cb_t function:
 * @code{.c}
 * static void
 * on_change_cb(plgd_dps_context_t *ctx, plgd_dps_status_t status, void *on_change_data) {
 *   printf("DPS Manager Status:\n");
 *   if (status & PLGD_DPS_INITIALIZED) {
 *     printf("\t-Initialized\n");
 *   }
 *   ...
 * }
 * @endcode
 *
 * Example of oc_cloud_cb_t function:
 * @code{.c}
 * static void
 * on_cloud_change_cb(oc_cloud_context_t *ctx, oc_cloud_status_t status, void *on_cloud_change_data) {
 *   printf("Cloud Manager Status:\n");
 *   if (status & OC_CLOUD_REGISTERED) {
 *     printf("\t-Registered\n");
 *   }
 *   ...
 * }
 * @endcode
 */
DPS_EXPORT
void plgd_dps_set_manager_callbacks(plgd_dps_context_t *ctx, plgd_dps_manager_callbacks_t callbacks) DPS_NONNULL(1);

/**
 * @brief Start DPS manager to provision device.
 *
 * Setup context, global session handlers and start DPS manager.
 *
 * Starting DPS also starts the retry mechanism, which will remain active until the device is successfully provisioned.
 * If a provisioning step fails, it will be tried again after a time interval. The time interval depends on the retry
 * counter (which is incremented on each retry) and uses the following values [ 10, 20, 40, 80, 120 ] in seconds.
 * Meaning that the first retry is scheduled after 10 seconds after a failure, the second retry after 20 seconds, etc.
 * After the interval reaches the maximal value (120 seconds) it resets back to the first value (10 seconds).
 *
 * @note Before starting the DPS manager, an endpoint must be set using plgd_dps_set_endpoint. Without an endpoint set
 * the provisioning will not start.
 *
 * @note The function examines the state of storage and some provisioning steps might be skipped if the stored data is
 * evaluated as still valid. To force full reprovisioning call plgd_force_reprovision before this function. At the end
 * of this call forced reprovisioning is disabled.
 * @see plgd_force_reprovision
 *
 * @param ctx dps context (cannot be NULL)
 * @return 0 on success
 * @return -1 on failure
 */
DPS_EXPORT
int plgd_dps_manager_start(plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Check whether DPS manager has been started.
 *
 * @param ctx dps context (cannot be NULL)
 * @return true DPS manager has been started
 * @return false DPS manager has not been started
 *
 * @see plgd_dps_manager_start
 */
DPS_EXPORT
bool plgd_dps_manager_is_started(const plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Stop DPS manager.
 *
 * Deregister handlers, clear context, stop DPS manager, close connection to DPS endpoint and remove identity
 * certificates retrieved from DPS endpoint.
 *
 * @param ctx dps context (cannot be NULL)
 */
DPS_EXPORT
void plgd_dps_manager_stop(plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Restart DPS manager to provision device by given server.
 *
 * A convenience function equivalent to calling plgd_dps_manager_stop and plgd_dps_manager_start.
 *
 * @param ctx dps context (cannot be NULL)
 * @return 0 on success
 * @return -1 on failure
 *
 * @see plgd_dps_manager_start
 * @see plgd_dps_manager_stop
 */
DPS_EXPORT
int plgd_dps_manager_restart(plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Start cloud manager with previously set server and callbacks.
 *
 * @param ctx dps context (cannot be NULL)
 * @return true on success
 * @return false otherwise
 */
DPS_EXPORT
bool plgd_cloud_manager_start(const plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Clean-up of DPS provisioning on factory reset.
 *
 * The function must be called from the factory reset handler to clean-up data that has been invalidated by a factory
 * reset.
 * The clean-up includes:
 *   - stopping of DPS provisioning and resetting the provisioning status
 *   - disconnecting from DPS endpoint and resetting the endpoint address
 *   - resetting data in storage and committing the empty data to storage files
 *   - removing identifiers of identity certificates that have been deleted by factory reset
 *
 * @param ctx dps context (cannot be NULL)
 * @return 0 on success
 * @return -1 on failure
 */
DPS_EXPORT
int plgd_dps_on_factory_reset(plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Controls whether a dps client verifies the device provision service's certificate chain against trust anchor
 * in the device. To set skip verify, it must be called before plgd_dps_manager_start.
 *
 * @param ctx dps context (cannot be NULL)
 * @param skip_verify skip verification of the DPS service
 */
DPS_EXPORT
void plgd_dps_set_skip_verify(plgd_dps_context_t *ctx, bool skip_verify) DPS_NONNULL();

/**
 * @brief Get `skip verify` value from context.
 *
 * @param ctx dps context (cannot be NULL)
 * @return true `skip verify` is enabled
 * @return false `skip verify` is disabled
 *
 * @see plgd_dps_set_skip_verify
 */
DPS_EXPORT
bool plgd_dps_get_skip_verify(const plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Set endpoint of the DPS service.
 *
 * Expected format of the endpoint is "coaps+tcp://${HOST}:${PORT}". For example: coaps+tcp://localhost:40030
 *
 * @param ctx dps context (cannot be NULL)
 * @param endpoint endpoint of the provisioning server (cannot be NULL)
 */
DPS_EXPORT
void plgd_dps_set_endpoint(plgd_dps_context_t *ctx, const char *endpoint) DPS_NONNULL();

/**
 * @brief Copy endpoint of the DPS service to output buffer.
 *
 * @param ctx dps context (cannot be NULL)
 * @param[out] buffer output buffer (cannot be NULL and must be large enough to contain the endpoint in a string format)
 * @param buffer_size size of output buffer
 * @return >0 on success, number of copied bytes to buffer
 * @return 0 endpoint is not set, thus nothing was copied
 * @return <0 on error
 */
DPS_EXPORT
int plgd_dps_get_endpoint(const plgd_dps_context_t *ctx, char *buffer, size_t buffer_size) DPS_NONNULL();

/**
 * @brief Check if the value of the DPS service endpoint is an empty string.
 *
 * @param ctx dps context (cannot be NULL)
 * @return true if the endpoint value is an empty string
 * @return false otherwise
 */
DPS_EXPORT
bool plgd_dps_endpoint_is_empty(const plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Force all steps of the provisioning process to be executed.
 *
 * A step that was successfully executed stores data in the storage and on the next start this data is still valid the
 * step would be automatically skipped.
 *
 * @param ctx dps context (cannot be NULL)
 *
 * @see plgd_dps_manager_start
 */
DPS_EXPORT
void plgd_dps_force_reprovision(plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Check if force reprovision flag is set.
 *
 * @param ctx dps context (cannot be NULL)
 * @return true force reprovision is set
 * @return false force reprovision is not set
 */
DPS_EXPORT
bool plgd_dps_has_forced_reprovision(const plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Configuration resource
 *
 * Description:
 *  - Resource type: x.plgd.dps.conf
 *  - Resource structure in json format:
 *    {
 *      endpoint: string;
 *      lastErrorCode: int;
 *      provisionStatus: string;
 *      forceReprovision: bool;
 *    }
 */
#define PLGD_DPS_URI "/plgd/dps"

/**
 * @brief Controls whether a dps client creates configuration resource for managing dps client via COAPs API.
 *
 * @param ctx dps context (cannot be NULL)
 * @param create set true for creating resource. set false to free memory of created resource.
 */
DPS_EXPORT
void plgd_dps_set_configuration_resource(plgd_dps_context_t *ctx, bool create) DPS_NONNULL();

/**
 * @brief Maximal size of the retry configuration array
 */
#define PLGD_DPS_MAX_RETRY_VALUES_SIZE (8)

/**
 * @brief Configure retry counter.
 *
 * @param ctx dps context (cannot be NULL)
 * @param cfg array with new timeout values (must have [1, PLGD_DPS_MAX_RETRY_VALUES_SIZE> number of non-zero values)
 * @param cfg_size size of the array with timeout values
 * @return true on success
 * @return false on failure
 */
DPS_EXPORT
bool plgd_dps_set_retry_configuration(plgd_dps_context_t *ctx, const uint8_t cfg[], size_t cfg_size) DPS_NONNULL(1);

/**
 * @brief Get retry counter configuration.
 *
 * @param ctx dps context (cannot be NULL)
 * @param[out] buffer output buffer into which the configuration will be copied (cannot be NULL, and must be large
 * enough to contain the current configuration)
 * @param buffer_size size of the output buffer
 * @return >0 the size of the configuration array copied to buffer
 * @return <0 on failure
 */
DPS_EXPORT
int plgs_dps_get_retry_configuration(const plgd_dps_context_t *ctx, uint8_t *buffer, size_t buffer_size) DPS_NONNULL();

/**
 * @brief Get last provisioning error.
 *
 * @param ctx dps context (cannot be NULL)
 * @return plgd_dps_error_t last provisioning error
 */
DPS_EXPORT
plgd_dps_error_t plgd_dps_get_last_error(const plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Get provision status.
 *
 * @param ctx dps context (cannot be NULL)
 * @return uint16_t current provision status
 */
DPS_EXPORT
uint32_t plgd_dps_get_provision_status(const plgd_dps_context_t *ctx) DPS_NONNULL();

typedef struct
{
  uint8_t max_count;  ///< the maximal number of retries before retrying is stopped and client is fully reprovisioned
                      ///< (default: 30)
  uint8_t interval_s; ///< retry interval in seconds (default: 1)
} plgd_cloud_status_observer_configuration_t;

/**
 * @brief Configure cloud observer.
 *
 * @param ctx dps context (cannot be NULL)
 * @param max_retry_count maximal number of retries, set to 0 to disable cloud status observer
 * @param retry_interval_s retry interval in seconds (must be >0)
 * @return true on success
 * @return false on error caused by invalid parameters
 */
DPS_EXPORT
bool plgd_dps_set_cloud_observer_configuration(plgd_dps_context_t *ctx, uint8_t max_retry_count,
                                               uint8_t retry_interval_s) DPS_NONNULL();

/**
 * @brief Get cloud observer configuration
 *
 * @param ctx dps context (cannot be NULL)
 * @return plgd_cloud_status_observer_configuration_t current cloud observer configuration
 */
DPS_EXPORT
plgd_cloud_status_observer_configuration_t plgd_dps_get_cloud_observer_configuration(const plgd_dps_context_t *ctx)
  DPS_NONNULL();

/**
 * @brief Set expiring-in limit of DPS certificates.
 *
 * If a certificate's valid-to timestamp is within the expiring-in limit (current time < valid_to and current time +
 * expiring-in limit > valid_to) then the certificate is considered as expiring.
 * Expiring certificates are not accepted during the get credentials step of DPS provisioning. If a expiring
 * certificates is received then the step is retried to receive a newer certificate with longer expiration.
 *
 * @param ctx dps context (cannot be NULL)
 * @param expiring_limit limit value in seconds
 */
DPS_EXPORT
void plgd_dps_pki_set_expiring_limit(plgd_dps_context_t *ctx, uint16_t expiring_limit) DPS_NONNULL();

/**
 * @brief Get expiring-in limit of DPS certificates
 *
 * @param ctx dps context (cannot be NULL)
 * @return expiring-in limit in seconds
 */
DPS_EXPORT
uint16_t plgd_dps_pki_get_expiring_limit(const plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Set certificate fingerprint of the provisioning server.
 *
 * If the fingerprint is set then the DPS client
 * will verify the fingerprint of the provisioning server certificate during the TLS handshake. If any certificate
 * matching the fingerprint in the chain is found then the handshake is successful.
 *
 * @param ctx dps context (cannot be NULL)
 * @param md_type hash algorithm used for fingerprint
 * @param fingerprint fingerprint of the provisioning server certificate
 * @param size size of the fingerprint
 * @return true on success
 */
DPS_EXPORT
bool plgd_dps_set_certificate_fingerprint(plgd_dps_context_t *ctx, mbedtls_md_type_t md_type,
                                          const uint8_t *fingerprint, size_t size) DPS_NONNULL(1);

/**
 * @brief Copy certificate fingerprint of the DPS service to output buffer.
 *
 * @param ctx dps context (cannot be NULL)
 * @param[out] md_type hash algorithm used for fingerprint
 * @param[out] buffer output buffer (cannot be NULL and must be large enough to contain the endpoint in a string format)
 * @param buffer_size size of output buffer
 * @return >0 on success, number of copied bytes to buffer
 * @return 0 endpoint is not set, thus nothing was copied
 * @return <0 on error
 */
DPS_EXPORT
int plgd_dps_get_certificate_fingerprint(const plgd_dps_context_t *ctx, mbedtls_md_type_t *md_type, uint8_t *buffer,
                                         size_t buffer_size) DPS_NONNULL();

/**
 * @brief Set the vendor encapsulated option code for the DPS endpoint. Used during call
 * plgd_dps_set_dhcp_vendor_encapsulated_option_code_dps_certificate_fingerprint.
 *
 * @param ctx dps context (cannot be NULL)
 * @param code vendor encapsulated option code for the DPS endpoint
 */
DPS_EXPORT
void plgd_dps_dhcp_set_vendor_encapsulated_option_code_dps_endpoint(plgd_dps_context_t *ctx, uint8_t code)
  DPS_NONNULL();

/**
 * @brief Get the vendor encapsulated option code for the DPS endpoint. Used during call
 * plgd_dps_set_dhcp_vendor_encapsulated_option_code_dps_certificate_fingerprint.
 *
 * @param ctx dps context (cannot be NULL)
 * @return uint8_t vendor encapsulated option code for the DPS endpoint
 */
DPS_EXPORT
uint8_t plgd_dps_dhcp_get_vendor_encapsulated_option_code_dps_endpoint(const plgd_dps_context_t *ctx) DPS_NONNULL();

/**
 * @brief Set the vendor encapsulated option code for the DPS certificate fingerprint. Used during call
 * plgd_dps_set_dhcp_vendor_encapsulated_option_code_dps_certificate_fingerprint.
 *
 * @param ctx dps context (cannot be NULL)
 * @param code vendor encapsulated option code for the DPS certificate fingerprint.
 */
DPS_EXPORT
void plgd_dps_dhcp_set_vendor_encapsulated_option_code_dps_certificate_fingerprint(plgd_dps_context_t *ctx,
                                                                                   uint8_t code) DPS_NONNULL();

/**
 * @brief Get the vendor encapsulated option code for the DPS certificate fingerprint. Used during call
 * plgd_dps_set_dhcp_vendor_encapsulated_option_code_dps_certificate_fingerprint.
 *
 * @param ctx dps context (cannot be NULL)
 * @return uint8_t vendor encapsulated option code for the DPS certificate fingerprint.
 */
DPS_EXPORT
uint8_t plgd_dps_dhcp_get_vendor_encapsulated_option_code_dps_certificate_fingerprint(const plgd_dps_context_t *ctx)
  DPS_NONNULL();

/**
 * @brief Convert isc-dhcp leases file vendor encapsulated options to byte array.
 *
 * @param hex_string input hex string (cannot be NULL) in format "01:a:3:14" or "010a0314"
 * @param hex_string_size vendor encapsulated options size in dhcp leases file.
 * @param buffer output buffer into which the byte array will be copied or NULL to get the needed size
 * @param buffer_size size of the output buffer
 * @return >0 the size of used or needed to copy to buffer, -1 on error
 */
DPS_EXPORT
ssize_t plgd_dps_hex_string_to_bytes(const char *hex_string, size_t hex_string_size, uint8_t *buffer,
                                     size_t buffer_size) DPS_NONNULL(1);

/**
 * @brief DPS dhcp plgd_dps_dhcp_set_values_from_vendor_encapsulated_options return values.
 */
typedef enum {
  PLGD_DPS_DHCP_SET_VALUES_ERROR = -1,           // error or parsing values failed
  PLGD_DPS_DHCP_SET_VALUES_NOT_CHANGED = 0,      // nothing changed
  PLGD_DPS_DHCP_SET_VALUES_UPDATED = 1,          // just updated
  PLGD_DPS_DHCP_SET_VALUES_NEED_REPROVISION = 2, // need to force reprovision with restart manager
} plgd_dps_dhcp_set_values_t;

/**
 * @brief Set DPS endpoint and certificate fingerprint that will be used in establishment of secure connection.
 *
 * @param ctx dps context (cannot be NULL)
 * @param vendor_encapsulated_options vendor encapsulated options in byte array
 * @param vendor_encapsulated_options_size vendor encapsulated options size in byte array
 * @return one of plgd_dps_dhcp_set_values_t
 */
DPS_EXPORT
plgd_dps_dhcp_set_values_t plgd_dps_dhcp_set_values_from_vendor_encapsulated_options(
  plgd_dps_context_t *ctx, const uint8_t *vendor_encapsulated_options, size_t vendor_encapsulated_options_size)
  DPS_NONNULL();

#ifdef __cplusplus
}
#endif

#endif /* PLGD_DPS_H */
