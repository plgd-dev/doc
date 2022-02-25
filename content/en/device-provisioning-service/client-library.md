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
 * @param skip_ca_verification if true then don't load and verify DPS CA certificate.
 * @return int  0   on success
 *              <0  on failure
 */
int plgd_dps_init(bool skip_ca_verification);

/**
 * @brief Stop all devices and deallocate data.
 */
void plgd_dps_shutdown(void);

/// Get context for given device
plgd_dps_context_t *plgd_dps_get_context(size_t device);

/// @brief Set DPS manager callbacks.
void plgd_dps_manager_init_callbacks(plgd_dps_context_t *ctx, plgd_dps_on_status_change_cb_t on_change_cb,
                                     void *on_change_data, oc_cloud_cb_t on_cloud_change_cb,
                                     void *on_cloud_change_data);

/**
 * @brief Start DPS manager to provision device by given server.
 *
 * Setup context, handlers, dps server and start dps manager.
 *
 * @param ctx device context (cannot be NULL)
 * @param endpoint endpoint of the provisioning server (cannot be NULL)
 * @return 0 on success
 * @return -1 on failure
 */
int plgd_dps_manager_start(plgd_dps_context_t *ctx, const char *endpoint);

/**
 * @brief Stop DPS manager.
 *
 * Deregister handlers, clear context, stop manager and close connection.
 *
 * @param ctx device context (cannot be NULL)
 */
void plgd_dps_manager_stop(plgd_dps_context_t *ctx);

/**
 * @brief Clean-up and restart DPS provisioning on factory reset.
 *
 * @param ctx device context (cannot be NULL)
 */
int plgd_dps_on_factory_reset(plgd_dps_context_t *ctx);

/**
 * @brief Set credid trusted root CA of the DPS service and/or disable its verification.
 *
 * If the credid has been set previously then certificate with the given credid is removed.
 *
 * @param device context (cannot be NULL)
 * @param ca_credid credid of the trusted root CA of the DPS service
 * @param ca_skip_verification skip verification of the DPS service
 */
void plgd_dps_set_service_ca(plgd_dps_context_t *ctx, int ca_credid, bool ca_skip_verification);
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

Start up the DPS initialization by calling the `plgd_dps_init(bool skip_ca_verification)` function, which allocates and initializes required data structures.
Set the `skip_ca_verification` parameter to true to [skip verification of the DPS endpoint](#load-certificates-and-keys), otherwise set it to false.

### Load certificates and keys

To be secure the device communicates with the DPS endpoint using TLS. Use [PKI functions from Iotivity-lite](https://github.com/iotivity/iotivity-lite/blob/master/include/oc_pki.h) to add certificates to device.

Use `oc_pki_add_mfg_cert` function to add manufacturer's certificate and private key to device. This certificate will be used to sign communication with the DPS server, which then can authenticate and identify the device. On success, use the returned credential id to assign a security profile by calling `oc_pki_set_security_profile`. Additionally, assign the credential id value to `dps_mfg_credid` member variable on device context struct.

Use `oc_pki_add_trust_anchor` function to add trusted root certificate of the DPS server, this will be used to verify the DPS server. On success, assign the returned credential id value to `dps_ca_credid` member variable on device context struct.

{{% note %}}
The verification of the DPS server can be disabled by setting the `skip_ca_verification` argument to false when calling `plgd_dps_init`.
{{% /note %}}

<!--
### Set custom logging function

TODO
-->

### Set status callbacks

The DPS client device can optionally provide two custom callbacks to track the provisioning status and the cloud registration status by calling `plgd_dps_manager_init_callbacks` function. The status of cloud registration is part of the public API of [Iotivity-lite](https://github.com/iotivity/iotivity-lite/blob/master/include/oc_cloud.h) and can be examined there.

The DPS status callback is executed whenever a provisioning step succeeds or fails. The callback has the following signature:

```C
void (*)(struct plgd_dps_context_t *ctx, plgd_dps_status_t status, void *data);
```

The `ctx` argument is device context created by `plgd_dps_init`, the `data` argument is a value provided to the call of `plgd_dps_manager_init_callbacks`. The `status` argument is an integer value, that consists of the following flags defined in the `dps.h` header:

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
  int mfg_credid = -1;
  if (dps_ctx->dps_ca_skip_verification) {
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
    int dpsca_credit = oc_pki_add_trust_anchor(dps_ctx->device, dps_ca, dps_ca_len);
    if (dpsca_credit < 0) {
      DPS_ERR("ERROR: installing DPS trusted root ca");
      goto error;
    }
    DPS_DBG("DPS trusted root credid=%d", dpsca_credit);
    plgd_dps_set_service_ca(dps_ctx, dpsca_credit, /*ca_skip_verification*/ false);
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
  mfg_credid = oc_pki_add_mfg_cert(dps_ctx->device, mfg_crt, mfg_crt_len, mfg_key, mfg_key_len);
  if (mfg_credid < 0) {
    DPS_ERR("ERROR: installing manufacturer certificate");
    goto error;
  }
  DPS_DBG("manufacturer certificate credid=%d", mfg_credid);
  oc_pki_set_security_profile(dps_ctx->device, OC_SP_BLACK, OC_SP_BLACK, mfg_credid);
  return 0;

error:
  if (dps_ctx->dps_ca_credid != -1) {
    if (dps_remove_certificate(dps_ctx->dps_ca_credid, dps_ctx->device) != 0) {
      DPS_WRN("failed to remove trusted root certificate(%d)", dps_ctx->dps_ca_credid);
    }
    dps_ctx->dps_ca_credid = -1;
  }
  if (mfg_credid != -1) {
    if (dps_remove_certificate(mfg_credid, dps_ctx->device) != 0) {
      DPS_WRN("failed to remove manufacturer certificate(%d)", mfg_credid);
    }
  }
  return -1;
}
```

### Device initialization

Now we have all the parts necessary to do the full initialization of the DPS client device:

```C
  ...
  oc_set_factory_presets_cb(dps_factory_presets_cb, NULL);
  ...
  oc_main_init(...);
  ...

  if (plgd_dps_init(skip_ca_verification != 0) != 0) {
    DPS_ERR("failed to initialize dps");
    goto finish;
  }
  plgd_dps_context_t *dps_ctx = plgd_dps_get_context(g_device_id);
  if (dps_ctx == NULL) {
    DPS_ERR("device(%zu) context not found", g_device_id);
    goto finish;
  }
  if (dps_add_certificates(dps_ctx, dps_cert_dir) != 0) {
    DPS_ERR("failed to add initial certificates");
    goto finish;
  }
  plgd_dps_manager_init_callbacks(dps_ctx, dps_status_handler, NULL, cloud_status_handler, NULL);
  if (plgd_dps_manager_start(dps_ctx, dps_endpoint) != 0) {
    DPS_ERR("failed to start dps manager");
    goto finish;
  }

  ...
  run(); // start run loop
  ...
```

### Running dps_cloud_server

Use `--help` switch to get information about how to correctly start up the process.

```bash
$ ./dps_cloud_server --help

./dps_cloud_server [device-name] [endpoint]

OPTIONS:
  -h | --help                print help
  --no-verify-ca             skip loading of the DPS certificate authority

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

To force restarting of the provisioning send `SIGHUP` signal to the process. Upon receiving the signal, the application will execute a factory reset and call your handler setup by call to `oc_set_factory_presets_cb`. In the handler you must reload the manufacturer’s certificates, which have been removed by the reset. At the end of the handler, call `plgd_dps_on_factory_reset`, which does some additional clean-up and then restarts the provisioning process.

The factory reset handler might look like this:

```C
static void
dps_factory_presets_cb(size_t device_id, void *data)
{
  (void)data;
  // preserve name after factory reset
  oc_device_info_t *dev = oc_core_get_device_info(device_id);
  oc_free_string(&dev->name);
  oc_new_string(&dev->name, dps_device_name, strlen(dps_device_name));

  plgd_dps_context_t *dps_ctx = plgd_dps_get_context(device_id);
  if (dps_ctx == NULL) {
    DPS_DBG("skip factory reset handling: empty context");
    return;
  }

  if (dps_add_certificates(dps_ctx, dps_cert_dir) != 0) {
    DPS_ERR("failed to add initial certificates on factory reset");
    return;
  }

  if (plgd_dps_on_factory_reset(dps_ctx) != 0) {
    DPS_ERR("cannot handle factory reset");
    return;
  }
}
```
