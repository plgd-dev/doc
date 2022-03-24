---
title: 'Client Library'
description: 'How to create a client of plgd Device Provisioning Service?'
date: '2021-02-15'
lastmod: '2021-02-15'
categories: [zero-touch, provisioning]
keywords: [provisioning]
menu:
  docs:
    parent: device-provisioning-service
    weight: 50
toc: true
---

## Get Package

The Device Provisioning Service is distributed as a `tar.gz` package, which contains the dps shared library, public C headers and an example application.

Latest version: 0.0.1
Supported platforms: linux/amd64, linux/arm64, linux/arm/v7

{{% note %}}
Please examine the contents of the provided pkg-config (`.pc`) file and install required dependencies.
{{% /note %}}

## DPS Client API

The API is defined in the public header files provided in the distributed package.

`dps.h`:

```C
...

/**
 * @brief Allocate and initialize data.
 *
 * @return int  0   on success
 *              <0  on failure
 */
int plgd_dps_init(void);

/**
 * @brief Stop all devices and deallocate data.
 */
void plgd_dps_shutdown(void);

/// Get context for given device
plgd_dps_context_t *plgd_dps_get_context(size_t device);

/**
 * @brief Get device from context.
 *
 * @param ctx dps context (cannot be NULL)
 *
 * @return size_t index of device
 */
size_t plgd_dps_get_device(plgd_dps_context_t *ctx);

/**
 * @brief Set DPS manager callbacks.
 *
 * @param ctx dps context (cannot be NULL)
 * @param on_change_cb callback invoked on provisioning status change
 * @param on_change_data user data provided to on_change_cb invocation
 * @param on_cloud_change_cb callback invoked on cloud status change
 * @param on_cloud_change_data user data provided to on_cloud_change_cb invocation
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
void plgd_dps_set_manager_callbacks(plgd_dps_context_t *ctx, plgd_dps_on_status_change_cb_t on_change_cb,
                                    void *on_change_data, oc_cloud_cb_t on_cloud_change_cb, void *on_cloud_change_data);

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
int plgd_dps_manager_start(plgd_dps_context_t *ctx);

/**
 * @brief Check whether DPS manager has been started.
 *
 * @param ctx dps context (cannot be NULL)
 * @return true DPS manager has been started
 * @return false DPS manager has not been started
 *
 * @see plgd_dps_manager_start
 */
bool plgd_dps_manager_is_started(const plgd_dps_context_t *ctx);

/**
 * @brief Stop DPS manager.
 *
 * Deregister handlers, clear context, stop DPS manager, close connection to DPS endpoint and remove identity
 * certificates retrieved from DPS endpoint.
 *
 * @param ctx dps context (cannot be NULL)
 */
void plgd_dps_manager_stop(plgd_dps_context_t *ctx);

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
int plgd_dps_manager_restart(plgd_dps_context_t *ctx);

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
int plgd_dps_on_factory_reset(plgd_dps_context_t *ctx);

/**
 * @brief Controls whether a dps client verifies the device provision service's certificate chain against trust anchor
 * in the device. To set skip verify, it must be called before plgd_dps_manager_start.
 *
 * @param ctx dps context (cannot be NULL)
 * @param skip_verify skip verification of the DPS service
 */
void plgd_dps_set_skip_verify(plgd_dps_context_t *ctx, bool skip_verify);

/**
 * @brief Get `skip verify` value from context.
 *
 * @param ctx dps context (cannot be NULL)
 * @return true `skip verify` is enabled
 * @return false `skip verify` is disabled
 *
 * @see plgd_dps_set_skip_verify
 */
bool plgd_dps_get_skip_verify(plgd_dps_context_t *ctx);

/**
 * @brief Set endpoint of DPS service.
 *
 * Expected format of the endpoint is "coaps+tcp://${HOST}:${PORT}". For example: coaps+tcp://localhost:40030
 *
 * @param ctx dps context (cannot be NULL)
 * @param endpoint endpoint of the provisioning server (cannot be NULL)
 */
void plgd_dps_set_endpoint(plgd_dps_context_t *ctx, const char *endpoint);

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
void plgd_dps_force_reprovision(plgd_dps_context_t *ctx);

/**
 * @brief Configuration resource
 *
 * Description:
 *  - Resource type: x.plgd.dps.conf
 *  - Resource structure in json format:
 *    {
 *      endpoint: string;
 *      lastErrorCode: int;
 *      selfOwned: bool;
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
void plgd_dps_set_configuration_resource(plgd_dps_context_t *ctx, bool create);

...
```

`dps_log.h`:

```C
...

/// @brief Set global logging function
void plgd_dps_set_log_fn(plgd_dps_print_log_fn_t fn);

/// @brief Get global logging function
plgd_dps_print_log_fn_t plgd_dps_get_log_fn(void);

...
```

### Initialize DPS

A DPS client device is an extension of an [Iotivity-lite](https://github.com/iotivity/iotivity-lite) device. Define your desired Iotivity-lite device and add DPS code to automatically provision the device.

Start up the DPS initialization by calling the `plgd_dps_init` function, which allocates and initializes required data structures.
Use setters `plgd_dps_set_endpoint`, `plgd_dps_set_manager_callbacks`, `plgd_dps_set_skip_verify` and `plgd_dps_set_configuration_resource` to configure the device.

<!--
### Set custom logging function

TODO
-->

### Set DPS Endpoint

To set the DPS endpoint call the `plgd_dps_set_endpoint(plgd_dps_context_t *ctx, const char *endpoint)` function.

### Set status callbacks

The DPS client device can optionally provide two custom callbacks to track the provisioning status and the cloud registration status by calling `plgd_dps_set_manager_callbacks` function. The status of cloud registration is part of the public API of [Iotivity-lite](https://github.com/iotivity/iotivity-lite/blob/master/include/oc_cloud.h) and can be examined there.

The DPS status callback is executed whenever a provisioning step succeeds or fails. The callback has the following signature:

```C
void (*)(struct plgd_dps_context_t *ctx, plgd_dps_status_t status, void *data);
```

The `ctx` argument is device context created by `plgd_dps_init`, the `data` argument is a value provided to the call of `plgd_dps_set_manager_callbacks`. The `status` argument is an integer value, that consists of the following flags defined in the `dps.h` header:

```C
typedef enum {
  PLGD_DPS_INITIALIZED = 1 << 0,
  PLGD_DPS_HAS_CREDENTIALS = 1 << 1,
  PLGD_DPS_HAS_ACLS = 1 << 2,
  PLGD_DPS_HAS_CLOUD = 1 << 3,
  PLGD_DPS_FAILURE = 1 << 6,
} plgd_dps_status_t;
```

#### Simple logging callback to tract provisioning state

```C
static void
dps_status_handler(plgd_dps_context_t *ctx, plgd_dps_status_t status, void *data)
{
  (void)data;
  (void)ctx;
  printf("\nDPS Manager Status:\n");
  if (status == 0) {
    printf("\t\t-Uninitialized\n");
  }
  if (status & PLGD_DPS_INITIALIZED) {
    printf("\t\t-Initialized\n");
  }
  if (status & PLGD_DPS_HAS_CREDENTIALS) {
    printf("\t\t-Has credentials\n");
  }
  if (status & PLGD_DPS_HAS_ACLS) {
    printf("\t\t-Has set acls\n");
  }
  if (status & PLGD_DPS_HAS_CLOUD) {
    printf("\t\t-Has configured cloud\n");
  }
  if (status & PLGD_DPS_FAILURE) {
    printf("\t\t-Failure\n");
  }
}
```

### DPS configuration resource

To expose the `/plgd/dps` resource call `plgd_dps_set_configuration_resource(plgd_dps_context_t *ctx, bool create)`, with the `create` parameter equal to `true` from your `register_resources` callback provided to `oc_main_init`.

The resource type of the DPS configuration resource is `x.plgd.dps.conf` and the resource has the following structure:

| Property Title | Property Name | Type | Access Mode | Mandatory | Description |
| -------------- | ------------- | -----| ----------- | --------- | ----------- |
| Endpoint | endpoint | string | RW | No | Device provisioning server endpoint in format `coaps+tcp://{domain}:{port}` |
| Last error code | lastErrorCode | string | R | No | Provides last error code when provision status is in `failed` state.<br/> `0` - OK<br/> `1` - error response<br/> `2` - cannot connect to dps<br/> `3` - cannot apply credentials configuration<br/> `4` - cannot apply acls configuration `5` - cannot apply cloud configuration |
| Self-owned | selfOwned | bool | R | No | True if the device is owned by itself (ie. by the device provisioning client) |
| Force reprovision | forceReprovision | bool | RW | No | Connect to dps service and reprovision credentials, acls and cloud configuration. |
| Provisioning status| provisionStatus | enum(string) | R | No | `uninitialized` - endpoint is not set or dps manger has not been started yet<br/> `initialized` - endpoint is set and manager is starting requests<br/> `provisioning credentials` - provisioning of credentials has been started<br/> `provisioning acls` - provisioning of acls has been started<br/> `provisioning cloud` - provisioning of cloud configuration has been started<br/> `provisioned` - device is fully provisioned and configured<br/> `failed` - provisioning fails, more information is stored in the last error code property |

### Load certificates and keys

To be secure the device communicates with the DPS endpoint using TLS. Use [PKI functions from Iotivity-lite](https://github.com/iotivity/iotivity-lite/blob/master/include/oc_pki.h) to add certificates to device.

Use `oc_pki_add_mfg_cert` function to add manufacturer's certificate and private key to device. This certificate will be used to sign communication with the DPS server, which then can authenticate and identify the device. On success, use the returned credential id to assign a security profile by calling `oc_pki_set_security_profile`.

Use `oc_pki_add_trust_anchor` function to add trusted root certificate of the DPS server, this will be used to verify the DPS server.

{{% note %}}
The verification of the DPS server can be disabled by calling `plgd_dps_set_skip_verify(false)`.
{{% /note %}}

### Provisioning

Provisioning algorithm is asynchronous and consists of several steps. Each step after it finishes schedules the next step. The whole process is fired up by calling `plgd_dps_manager_start`.

After the provisioning is successfully finished, the connection to DPS endpoint is closed and the registration to plgd cloud is started using the data that was set up during the provisioning.

#### Handling failure

If a step in the provisioning algorithm encounters an error, or if it does not trigger the next step (for example, the answer from DPS endpoint didn’t come, thus the handler didn’t trigger and schedule the next step), then the step is retried after a small interval.

The application uses the following retry intervals [10s, 20s, 40s, 80s, 120s] and forever loops through the array (i.e. when the last retry interval was 120s then the next will be 10s again) until the provisioning process succeeds.

## Example application - dps_cloud_server

Part of the package is an example application called `dps_cloud_server`. The following sections describe how DPS client library is integrated in the binary (with code examples). And the finally section shows how to run it.

### Bootstrapping device with mfg certificates

Currently, the handling of certificates is hard-coded into the binary. On start-up the process expects the certificates to be in a folder named `pki_certs` located next to the binary file. The folder must contain 3 files:

- dpsca.pem - certificate authority of the DPS
- mfgcrt.pem - manufacture certificate of the device
- mfgkey.pem - private key for the manufacture certificate

The `dps_cloud_server` supports `--no-verify-ca` option. If you run the binary with this option, then the DPS endpoint will be used without authentication and you don't need the `dpsca.pem` file.

In code, the application uses [Iotivity-lite's pki functions to load the certificates](#load-certificates-and-keys) in this helper function:

```C
/**
 * @brief Add dps ca trusted root certificate and device's manufacturer certificate.
 *
 * @param dps_ctx device context
 * @param cert_dir path to directory with certificates
 * @return int 0 on success
 * @return int -1 on failure
 */
static int
dps_add_certificates(plgd_dps_context_t *dps_ctx, const char *cert_dir)
{
  oc_assert(dps_ctx != NULL);
  oc_assert(cert_dir != NULL);
#define CERT_BUFFER_SIZE 4096

  char path[PATH_MAX];
  int dpsca_credit = -1;
  int mfg_credid = -1;
  if (plgd_dps_get_skip_verify(dps_ctx)) {
    DPS_DBG("adding of trusted root certificate skipped");
  } else {
    unsigned char dps_ca[CERT_BUFFER_SIZE];
    size_t dps_ca_len = sizeof(dps_ca);
    memset(path, 0, sizeof(path));
    strncpy(path, cert_dir, sizeof(path));
    strcat(path, "/dpsca.pem");
    if (dps_read_pem(path, (char *)dps_ca, &dps_ca_len) < 0) {
      DPS_ERR("ERROR: unable to read %s", path);
      goto error;
    }
    dpsca_credit = oc_pki_add_trust_anchor(plgd_dps_get_device(dps_ctx), dps_ca, dps_ca_len);
    if (dpsca_credit < 0) {
      DPS_ERR("ERROR: installing DPS trusted root ca");
      goto error;
    }
    DPS_DBG("DPS trusted root credid=%d", dpsca_credit);
  }

  unsigned char mfg_crt[CERT_BUFFER_SIZE];
  size_t mfg_crt_len = sizeof(mfg_crt);
  memset(path, 0, sizeof(path));
  strncpy(path, cert_dir, sizeof(path));
  strcat(path, "/mfgcrt.pem");
  if (dps_read_pem(path, (char *)mfg_crt, &mfg_crt_len) < 0) {
    DPS_ERR("ERROR: unable to read %s", path);
    goto error;
  }
  unsigned char mfg_key[CERT_BUFFER_SIZE];
  size_t mfg_key_len = sizeof(mfg_key);
  memset(path, 0, sizeof(path));
  strncpy(path, cert_dir, sizeof(path));
  strcat(path, "/mfgkey.pem");
  if (dps_read_pem(path, (char *)mfg_key, &mfg_key_len) < 0) {
    DPS_ERR("ERROR: unable to read %s", path);
    goto error;
  }
  mfg_credid = oc_pki_add_mfg_cert(plgd_dps_get_device(dps_ctx), mfg_crt, mfg_crt_len, mfg_key, mfg_key_len);
  if (mfg_credid < 0) {
    DPS_ERR("ERROR: installing manufacturer certificate");
    goto error;
  }
  DPS_DBG("manufacturer certificate credid=%d", mfg_credid);
  oc_pki_set_security_profile(plgd_dps_get_device(dps_ctx), OC_SP_BLACK, OC_SP_BLACK, mfg_credid);
  return 0;

error:
  if (dpsca_credit != -1) {
    if (oc_sec_remove_cred_by_credid(dpsca_credit, plgd_dps_get_device(dps_ctx))) {
      DPS_DBG("certificate(%d) removed", dpsca_credit);
    } else {
      DPS_WRN("failed to remove trusted root certificate(%d)", dpsca_credit);
    }
  }
  if (mfg_credid != -1) {
    if (oc_sec_remove_cred_by_credid(mfg_credid, plgd_dps_get_device(dps_ctx))) {
      DPS_DBG("certificate(%d) removed", mfg_credid);
    } else {
      DPS_WRN("failed to remove manufacturer certificate(%d)", mfg_credid);
    }
  }
  return -1;
}
```

### Configuring device

Use public API of Iotivity-lite and DPS to setup device to desired initial configuration:

```C
/**
 * @brief Setup the device to manufacturer's configuration.
 *
 * @param dps_ctx device context
 * @return int 0 on success
 * @return int -1 on failure
 */
static int
manufacturer_setup(plgd_dps_context_t *dps_ctx)
{
  // preserve name after factory reset
  oc_device_info_t *dev = oc_core_get_device_info(plgd_dps_get_device(dps_ctx));
  if (dev != NULL) {
    oc_free_string(&dev->name);
    oc_new_string(&dev->name, dps_device_name, strlen(dps_device_name));
  }
  plgd_dps_set_manager_callbacks(dps_ctx, dps_status_handler, /* on_change_data */ NULL, cloud_status_handler,
                                 /* on_cloud_change_data */ NULL);
  plgd_dps_set_skip_verify(dps_ctx, g_skip_ca_verification != 0);
  plgd_dps_set_endpoint(dps_ctx, dps_endpoint);
  if (dps_add_certificates(dps_ctx, dps_cert_dir) != 0) {
    DPS_ERR("failed to add initial certificates on factory reset");
    return -1;
  }
  plgd_dps_force_reprovision(dps_ctx);
  return 0;
}
```

### Factory reset

Use a factory reset handler function to react to a reset of the device. The function is also invoked on the first start of the device and thus can be used to call the configuration function.

{{% note %}}
If your device has run before then it should've created storage files in a folder next to the binary. During the initialization the data will be loaded from these storage files and the factory reset function might not be invoked. The configuration of the device will be based on the loaded data.
{{% /note %}}

To properly handle a device reset the handler must reload the manufacturer’s certificates, which have been removed by the reset and reconfigure the device. At the end of the handler, call the `plgd_dps_on_factory_reset` function, which does some additional clean-up and resets the provisioning state. Finally, restart the provisioning process by calling `plgd_dps_manager_start`.

```C
static void
factory_presets_cb(size_t device_id, void *data)
{
  (void)data;
  plgd_dps_context_t *dps_ctx = plgd_dps_get_context(device_id);
  if (dps_ctx == NULL) {
    DPS_DBG("skip factory reset handling: empty context");
    return;
  }

  if (plgd_dps_on_factory_reset(dps_ctx) != 0) {
    DPS_ERR("cannot handle factory reset");
    return;
  }
  if (manufacturer_setup(dps_ctx) != 0) {
    DPS_ERR("failed to configure device");
    return;
  }
  if (plgd_dps_manager_start(dps_ctx) != 0) {
    DPS_ERR("failed to start dps manager");
    return;
  }
}

```

### Device initialization

Now we have all the parts necessary to do the full initialization of the DPS client device:

```C
  ...
  oc_set_factory_presets_cb(factory_presets_cb, NULL);
  ...
  oc_main_init(...);
  ...

  plgd_dps_context_t *dps_ctx = plgd_dps_get_context(g_device_id);
  if (dps_ctx == NULL) {
    DPS_ERR("cannot start dps manager: empty context");
    ret = -1;
    goto finish;
  }
  if (!plgd_dps_manager_is_started(dps_ctx)) {
    plgd_dps_set_skip_verify(dps_ctx, g_skip_ca_verification != 0);
    plgd_dps_set_manager_callbacks(dps_ctx, dps_status_handler, /* on_change_data */ NULL, cloud_status_handler,
                                   /* on_cloud_change_data */ NULL);
    if (plgd_dps_manager_start(dps_ctx) != 0) {
      DPS_ERR("failed to start dps manager");
      ret = -1;
      goto finish;
    }
  }
  run(); // start run loop
finish:
  ...
```

### Running dps_cloud_server

Use `--help` switch to get information about how to correctly start up the process.

```bash
$ ./dps_cloud_server --help

./dps_cloud_server [device-name] [endpoint]

OPTIONS:
  -h | --help                print help
  --create-conf-resource     create DPS configuration resource
  --no-verify-ca             skip loading of the DPS certificate authority
  --wait-for-reset           don\'t start right away, but wait for SIGHUP signal


ARGUMENTS:
  device-name                name of the device (optional, default: dps)
  endpoint                   address of the endpoint (optional, default: coaps+tcp://127.0.0.1:20030)
```

The binary takes two optional positional arguments `device-name` and `endpoint`. If they are not specified, default values are used instead. The `device-name` argument will be used as the name of the device after it is registered to the plgd cloud. The `endpoint` argument is the endpoint of a running DPS service.

For example, if your DPS endpoint is running on the address `api.try.plgd.cloud:25684` then start your device named `my-device` by the following call:

```bash
./dps_cloud_server "my-device" "coaps+tcp://api.try.plgd.cloud:25684"
```

### Restarting dps_cloud_server

To force restarting of the provisioning send `SIGHUP` signal to the process. Upon receiving the signal, the application will execute a factory reset and call your handler setup by call to `oc_set_factory_presets_cb`.
