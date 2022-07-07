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

{{% note %}}
Are you interested in testing out the zero-touch provisioning? [Reach out](https://plgd.dev/contact/) to us!
{{% /note %}}

The Device Provisioning Service is distributed as a `tar.gz` package, which contains the dps shared library, public C headers and an example application.

{{% warning %}}
Please examine the contents of the provided pkg-config (`.pc`) file and install required dependencies.
{{% /warning %}}

## DPS Client API

The API is defined in the public header files provided in the distributed package.

- [dps.h](../../dps/dps.h)
- [dps_export.h](../../dps/dps_export.h)
- [dps_log.h](../../dps/dps_log.h)

### Initialize DPS

A DPS client device is an extension of an [IoTivity](https://github.com/iotivity/iotivity-lite) device. Define your desired device and add DPS code to automatically provision the device.

Start up the DPS initialization by calling the `plgd_dps_init` function, which allocates and initializes required data structures.
Use setters `plgd_dps_set_endpoint`, `plgd_dps_set_manager_callbacks`, `plgd_dps_set_skip_verify` and `plgd_dps_set_configuration_resource` to configure the device.

### Set DPS Endpoint

To set the DPS endpoint call the `plgd_dps_set_endpoint(plgd_dps_context_t *ctx, const char *endpoint)` function.

### Set status callbacks

The DPS client device can optionally provide two custom callbacks to track the provisioning status and the cloud registration status by calling `plgd_dps_set_manager_callbacks` function. The status of cloud registration is part of the public API of [IoTivity](https://github.com/iotivity/iotivity-lite/blob/master/include/oc_cloud.h) and can be examined there.

The DPS status callback is executed whenever a provisioning step succeeds or fails. The callback has the following signature:

```C
void (*)(struct plgd_dps_context_t *ctx, plgd_dps_status_t status, void *data);
```

The `ctx` argument is device context created by `plgd_dps_init`, the `data` argument is a value provided to the call of `plgd_dps_set_manager_callbacks`. The `status` argument is an integer value, that consists of the following flags defined in the `dps.h` header:

```C
/**
 * @brief DPS provisioning status flags.
 */
typedef enum {
  PLGD_DPS_INITIALIZED = 1 << 0,
  PLGD_DPS_GET_CREDENTIALS = 1 << 1,
  PLGD_DPS_HAS_CREDENTIALS = 1 << 2,
  PLGD_DPS_GET_ACLS = 1 << 3,
  PLGD_DPS_HAS_ACLS = 1 << 4,
  PLGD_DPS_GET_CLOUD = 1 << 6,
  PLGD_DPS_HAS_CLOUD = 1 << 7,
  PLGD_DPS_CLOUD_STARTED = 1 << 8,
  PLGD_DPS_TRANSIENT_FAILURE = 1 << 14,
  PLGD_DPS_FAILURE = 1 << 15,
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
  if ((status & PLGD_DPS_INITIALIZED) != 0) {
    printf("\t\t-Initialized\n");
  }
  if ((status & PLGD_DPS_GET_CREDENTIALS) != 0) {
    printf("\t\t-Get credentials\n");
  }
  if ((status & PLGD_DPS_HAS_CREDENTIALS) != 0) {
    printf("\t\t-Has credentials\n");
  }
  if ((status & PLGD_DPS_GET_ACLS) != 0) {
    printf("\t\t-Get acls\n");
  }
  if ((status & PLGD_DPS_HAS_ACLS) != 0) {
    printf("\t\t-Has set acls\n");
  }
  if ((status & PLGD_DPS_GET_CLOUD) != 0) {
    printf("\t\t-Get cloud configuration\n");
  }
  if ((status & PLGD_DPS_HAS_CLOUD) != 0) {
    printf("\t\t-Has cloud configuration\n");
  }
  if ((status & PLGD_DPS_CLOUD_STARTED) != 0) {
    printf("\t\t-Started cloud\n");
  }
  if ((status & PLGD_DPS_TRANSIENT_FAILURE) != 0) {
    printf("\t\t-Transient failure\n");
  }
  if ((status & PLGD_DPS_FAILURE) != 0) {
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
| Last error code | lastErrorCode | string | R | No | Provides last error code when provision status is in `failed` state (see list below for possible values). |
| Self-owned | selfOwned | bool | R | No | True if the device is owned by itself (ie. by the device provisioning client) |
| Force reprovision | forceReprovision | bool | RW | No | Connect to dps service and reprovision credentials, acls and cloud configuration. |
| Provisioning status | provisionStatus | enum(string) | R | No | String representation of the provisioning status (see list below for possible values). |

Last error code values:

- `0`: OK
- `1`: error response
- `2`: cannot connect to dps
- `3`: cannot apply credentials configuration
- `4`: cannot apply acls configuration
- `5`: cannot apply cloud configuration
- `6`: cannot start cloud

Provisioning status values:

- `uninitialized`: endpoint is not set or dps manger has not been started yet
- `initialized`: endpoint is set and manager is starting requests
- `provisioning credentials`: provisioning of credentials has been started
- `provisioned credentials`: credentials are provisioned
- `provisioning acls`: provisioning of acls has been started
- `provisioned acls`: acls are provisioned
- `provisioning cloud`: provisioning of cloud configuration has been started
- `provisioned cloud`: cloud configuration is provisioned
- `provisioned`: device is fully provisioned and configured
- `transient failure`: provisioning failed with a transient error and the failed step is being retried
- `failure`: provisioning failed, more information is stored in the last error code property

### Load certificates and keys

To make sure the device communicates with the DPS endpoint using TLS, add the certificate using the [IoTivity PKI API](https://github.com/iotivity/iotivity-lite/blob/master/include/oc_pki.h) to the device.

The `oc_pki_add_mfg_cert` function adds manufacturer's certificate and private key to device. This certificate is used to sign the communication with the DPS server, authenticatating and identifying the device. On success, use the returned credential id to assign a security profile by calling `oc_pki_set_security_profile`.

The `oc_pki_add_trust_anchor` function adds trusted root certificate of the DPS server, used to verify the DPS.

{{% note %}}
The verification of the DPS can be disabled by calling `plgd_dps_set_skip_verify(false)`.
{{% /note %}}

### Provisioning

Provisioning algorithm is asynchronous and consists of several steps. Each step after it finishes schedules the next step. The whole process is fired up by calling `plgd_dps_manager_start`.

After the provisioning is successfully finished, the connection to the DPS endpoint is closed and the registration to the plgd hub is started using the data, that was set up during the provisioning.

#### Handling failure

If a step in the provisioning algorithm encounters an error, or if it does not trigger the next step (for example, the answer from DPS endpoint didn’t come, thus the handler didn’t trigger and schedule the next step), then the step is retried after a small interval.

The application uses the following retry intervals [10s, 20s, 40s, 80s, 120s] and forever loops through the array (i.e. when the last retry interval was 120s then the next will be 10s again) until the provisioning process succeeds. To read more about the retry mechanism of the DPS Client Library, read [here](../retry-mechanism).

## Example application - dps_cloud_server

Part of the package is an example application called `dps_cloud_server`. The following sections describe how DPS client library is integrated in the binary (with code examples). And the finally section shows how to run it.

### Bootstrapping device with manufacturer certificates

Currently, the handling of certificates is hard-coded into the binary. On start-up, the process expects the certificates to be in a folder named `pki_certs` located next to the binary file. The folder must contain 3 files:

- `dpsca.pem` - certificate authority of the DPS
- `mfgcrt.pem` - manufacture certificate of the device
- `mfgkey.pem` - manufacturer certificate private key

The `dps_cloud_server` supports `--no-verify-ca` option. If you run the binary with this option, then the DPS endpoint will be used without authentication and you don't need the `dpsca.pem` file.

In code, the application uses [IoTivity's PKI functions to load the certificates](#load-certificates-and-keys) in this helper function:

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
  -h | --help                   print help
  -c | --create-conf-resource   create DPS configuration resource
  -h | --no-verify-ca           skip loading of the DPS certificate authority
  -r | --retry-configuration    retry timeout configuration (array of non-zero values delimited by ',', maximum of 8 values is accepted; example: 1,2,4,8,16)
  -w | --wait-for-reset         don\'t start right away, but wait for SIGHUP signal


ARGUMENTS:
  device-name                   name of the device (optional, default: dps)
  endpoint                      address of the endpoint (optional, default: coaps+tcp://127.0.0.1:20030)
```

The binary takes two optional positional arguments `device-name` and `endpoint`. If they are not specified, default values are used instead. The `device-name` argument will be used as the name of the device after it is registered to the plgd cloud. The `endpoint` argument is the endpoint of a running DPS service.

For example, if your DPS endpoint is running on the address `api.try.plgd.cloud:25684` then start your device named `my-device` by the following call:

```bash
./dps_cloud_server "my-device" "coaps+tcp://api.try.plgd.cloud:25684"
```

### Restarting dps_cloud_server

To force provisioning restart, send the `SIGHUP` signal to the process. Upon receiving the signal, the application will execute a factory reset and call your handler setup by a call to `oc_set_factory_presets_cb`.
