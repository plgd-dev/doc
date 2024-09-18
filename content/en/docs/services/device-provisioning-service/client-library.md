---
title: 'Client Library'
description: 'How to create a client of plgd Device Provisioning Service?'
date: '2023-04-19'
categories: [zero-touch, provisioning]
keywords: [provisioning]
weight: 2
---

The Device Provisioning Service is part of the [IoTivity-lite client-server library](https://github.com/iotivity/iotivity-lite/wiki/Device-Provisioning-Service).

## DPS Client API

The API is defined in the public header file.

- [plgd_dps.h](https://github.com/iotivity/iotivity-lite/blob/master/include/plgd/plgd_dps.h)

### Initialize DPS

A DPS client device is an extension of an [IoTivity](https://github.com/iotivity/iotivity-lite) device. Define your desired device and add DPS code to automatically provision the device.

Start up the DPS initialization by calling the `plgd_dps_init` function, which allocates and initializes required data structures.
Use setters `plgd_dps_set_manager_callbacks`, `plgd_dps_set_skip_verify`, `plgd_dps_set_configuration_resource`, `plgd_dps_set_retry_configuration`, `plgd_dps_set_cloud_observer_configuration` and `plgd_dps_pki_set_expiring_limit` to configure the device.

### Set DPS Endpoint

DPS endpoints are stored in a list ordered by priority. (ie. the first endpoint has the highest priority and should be tried first.)

- To add an endpoint to the list of DPS server endpoints use `plgd_dps_add_endpoint_address(plgd_dps_context_t *ctx, const char *uri, size_t uri_len, const char *name, size_t name_len)` function.
- To remove an endpoint use `plgd_dps_remove_endpoint_address(plgd_dps_context_t *ctx, const oc_endpoint_address_t *address)`
- To iterate over the list use `plgd_dps_iterate_server_addresses(const plgd_dps_context_t *ctx, oc_endpoint_addresses_iterate_fn_t iterate_fn, void *iterate_fn_data)`
- To select an endpoint from the list to be used during DPS provisioning use `plgd_dps_select_endpoint_address(plgd_dps_context_t *ctx, const oc_endpoint_address_t *address)` (by default the first endpoint added is selected)
- To get the currently selected endpoint use `plgd_dps_selected_endpoint_address(const plgd_dps_context_t *ctx)`

Function `plgd_dps_set_endpoint` used to previously configure the DPS server endpoint when it was a single element has been deprecated. Use the functions above to work with the list of endpoints.

{{< note >}}
On repeated DPS provisioning failure with the currently selected endpoint, the default retry mechanism changes the selected endpoint to the next one in the list of endpoints. For details, go [here](/docs/services/device-provisioning-service/retry-mechanism)
{{< /note >}}

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
  PLGD_DPS_RENEW_CREDENTIALS = 1 << 9,
  PLGD_DPS_GET_OWNER = 1 << 10,
  PLGD_DPS_HAS_OWNER = 1 << 11,
  PLGD_DPS_GET_TIME = 1 << 12,
  PLGD_DPS_HAS_TIME = 1 << 13,
  PLGD_DPS_TRANSIENT_FAILURE = 1 << 29,
  PLGD_DPS_FAILURE = 1 << 30,
} plgd_dps_status_t;
```

#### Simple logging callback to tract provisioning state

```C
static void
dps_status_handler(plgd_dps_context_t *ctx, plgd_dps_status_t status, void *data)
{
  (void)data;
  (void)ctx;
  PRINT("\nDPS Manager Status:\n");
  if (status == 0) {
    PRINT("\t\t-Uninitialized\n");
  }
  if ((status & PLGD_DPS_INITIALIZED) != 0) {
    PRINT("\t\t-Initialized\n");
  }
  if ((status & PLGD_DPS_GET_TIME) != 0) {
    PRINT("\t\t-Get time\n");
  }
  if ((status & PLGD_DPS_HAS_TIME) != 0) {
    PRINT("\t\t-Has time\n");
  }
  if ((status & PLGD_DPS_GET_OWNER) != 0) {
    PRINT("\t\t-Get owner\n");
  }
  if ((status & PLGD_DPS_HAS_OWNER) != 0) {
    PRINT("\t\t-Has owner\n");
  }
  if ((status & PLGD_DPS_GET_CLOUD) != 0) {
    PRINT("\t\t-Get cloud configuration\n");
  }
  if ((status & PLGD_DPS_HAS_CLOUD) != 0) {
    PRINT("\t\t-Has cloud configuration\n");
  }
  if ((status & PLGD_DPS_GET_CREDENTIALS) != 0) {
    PRINT("\t\t-Get credentials\n");
  }
  if ((status & PLGD_DPS_HAS_CREDENTIALS) != 0) {
    PRINT("\t\t-Has credentials\n");
  }
  if ((status & PLGD_DPS_GET_ACLS) != 0) {
    PRINT("\t\t-Get acls\n");
  }
  if ((status & PLGD_DPS_HAS_ACLS) != 0) {
    PRINT("\t\t-Has set acls\n");
  }
  if ((status & PLGD_DPS_CLOUD_STARTED) != 0) {
    PRINT("\t\t-Started cloud\n");
  }
  if ((status & PLGD_DPS_RENEW_CREDENTIALS) != 0) {
    PRINT("\t\t-Renew credentials\n");
  }
  if ((status & PLGD_DPS_TRANSIENT_FAILURE) != 0) {
    PRINT("\t\t-Transient failure\n");
  }
  if ((status & PLGD_DPS_FAILURE) != 0) {
    PRINT("\t\t-Failure\n");
  }
}
```

### DPS configuration resource

To expose the `/plgd/dps` resource call `plgd_dps_set_configuration_resource(plgd_dps_context_t *ctx, bool create)`, with the `create` parameter equal to `true` from your `register_resources` callback provided to `oc_main_init`.

The resource type of the DPS configuration resource is `x.plgd.dps.conf` and the resource has the following structure:

| Property Title | Property Name | Type | Access Mode | Mandatory | Description |
| -------------- | ------------- | -----| ----------- | --------- | ----------- |
| Endpoint | endpoint | string | RW | No | Selected device provisioning server endpoint in format `coaps+tcp://{domain}:{port}` |
| Endpoint name | endpointName | string | RW | No | Name associated with the selected device provisioning server endpoint (currently unused by DPS). |
| Endpoints | endpoints | array of objects | RW | No | Array of device provisioning server endpoints. Each item is a pair of (`uri`, `name`) values, where `uri` is the endpoint address in the format `coaps+tcp://{domain}:{port}` and `name` is a string name associated with the endpoint. (Note: the property is generated only if there are at least 2 endpoints set) |
| Last error code | lastErrorCode | string | R | No | Provides last error code when provision status is in `failed` state (see list below for possible values). |
| Force reprovision | forceReprovision | bool | RW | No | Connect to dps service and reprovision time, owner, cloud configuration, credentials and acls. |
| Provisioning status | provisionStatus | enum(string) | R | No | String representation of the provisioning status (see list below for possible values). |

Last error code values:

- `0` (`PLGD_DPS_OK`): OK
- `1` (`PLGD_DPS_ERROR_RESPONSE`): error response
- `2` (`PLGD_DPS_ERROR_CONNECT`): cannot connect to dps
- `3` (`PLGD_DPS_ERROR_GET_CREDENTIALS`): cannot apply credentials configuration
- `4` (`PLGD_DPS_ERROR_GET_ACLS`): cannot apply acls
- `5` (`PLGD_DPS_ERROR_SET_CLOUD`): cannot apply cloud configuration
- `6` (`PLGD_DPS_ERROR_START_CLOUD`): cannot start cloud
- `7` (`PLGD_DPS_ERROR_GET_OWNER`): cannot retrieve device owner
- `8` (`PLGD_DPS_ERROR_GET_TIME`): cannot retrieve current time

Provisioning status values:

- `uninitialized`: endpoint is not set or dps manger has not been started yet
- `initialized`: endpoint is set and manager is starting requests
- `provisioning credentials`: provisioning of credentials has been started
- `provisioned credentials`: credentials are provisioned
- `provisioning owner`: requesting of device owner has been started
- `provisioned owner`: device is owned by the received owner
- `provisioning acls`: provisioning of acls has been started
- `provisioned acls`: acls are provisioned
- `provisioning cloud`: provisioning of cloud configuration has been started
- `provisioned cloud`: cloud configuration is provisioned
- `provisioning time`: time synchronization has been started
- `provisioned time`: time is synchronized
- `provisioned`: device is fully provisioned and configured
- `renew credentials`: renewing expired or expiring certificates
- `transient failure`: provisioning failed with a transient error and the failed step is being retried
- `failure`: provisioning failed, more information is stored in the last error code property

### Load certificates and keys

To make sure the device communicates with the DPS endpoint using TLS, add the certificate using the [IoTivity PKI API](https://github.com/iotivity/iotivity-lite/blob/master/include/oc_pki.h) to the device.

The `oc_pki_add_mfg_cert` function adds manufacturer's certificate and private key to device. This certificate is used to sign the communication with the DPS server, authenticatating and identifying the device. On success, use the returned credential id to assign a security profile by calling `oc_pki_set_security_profile`.

The `oc_pki_add_mfg_trust_anchor` function adds manufacturer's trusted root certificate of the DPS server, which is used to verify the DPS.

{{< note >}}
The verification of the DPS can be disabled by calling `plgd_dps_set_skip_verify(false)`.
{{< /note >}}

### Provisioning

Provisioning algorithm is asynchronous and consists of several steps. Each step after it finishes schedules the next step. The whole process is fired up by calling `plgd_dps_manager_start`.

After the provisioning is successfully finished, the connection to the DPS endpoint is closed and the registration to the plgd hub is started using the data, that was set up during the provisioning.

#### Handling failure

If a step in the provisioning algorithm encounters an error, or if it does not trigger the next step (for example, the answer from DPS endpoint didn’t come, thus the handler didn’t trigger and schedule the next step), then the step is retried after a small interval.

The application uses the following retry intervals [10s, 20s, 40s, 80s, 120s] and forever loops through the array (i.e. when the last retry interval was 120s then the next will be 10s again) until the provisioning process succeeds. To read more about the retry mechanism of the DPS Client Library, read [here](/docs/services/device-provisioning-service/retry-mechanism).

### Time synchronization

In order for the TLS handshake to verify the server certificate and for certificate rotation to occur, the device must have the correct time. Time synchronization is achieved by utilizing DPS clients to synchronize with the DPS server over a connection that does not verify the validity interval of the certificates.
It's important to note that IoTivity-Lite needs to be compiled with the cmake option `-DPLGD_DEV_TIME_ENABLED=ON` in order to enable the [time synchronization feature](/docs/services/device-provisioning-service/time-synchronization) in the IoTivity-Lite library, and initialized with `plgd_time_configure`.

{{< note >}}
To compensate for the disabled time verification during the TLS handshake, time synchronization is established through a separate connection. Once synchronization is completed, the connection is terminated and the device's time is adjusted to bring it back into alignment. While this approach may not provide pinpoint accuracy, it is adequate for verifying the server certificate during the TLS handshake and for certificate rotation. For more precise time synchronization, utilizing NTP or another time synchronization protocol would be recommended.
{{< /note >}}

### Certificate renewal

{{< note >}}
Proper device functionality is contingent upon time synchronization.
{{< /note >}}

A certificate retrieved from the DPS service can be in one of the following states:

- not yet valid - the valid-from time hasn't yet passed
- expired - the valid-to time has passed already
- expiring - the valid-to time will pass soon (the expiration interval is determined by the `expiringLimit` value, by default this is 1 hour)
- valid - otherwise

In the provisioning of credentials step, the retrieved certificates are checked for validity. If at least one of the retrieved certificates is not valid, they are all discarded and the whole step is repeated.

A valid certificate fulfills all of the following conditions:

- certificate was correctly parsed and stored by the IoTivity-lite library
- its valid-from and valid-to timestamps are currently valid
- the certificate is not expiring (ie. the interval until the certificate expires is longer than the `expiringLimit` value)

Once the certificates are stored, the certificate renewal operation is scheduled based on the earliest valid-to timestamp of a retrieved certificate.

#### Setting expiration interval

To set expiration interval use `plgd_dps_pki_set_expiring_limit`. To get the current expiration interval call `plgd_dps_pki_get_expiring_limit`. The interval is in seconds and the default value is 1 hour (3600 seconds).

#### Certificate renewal interval

To get the interval after which the certificate renewal operation is scheduled, all retrieved certificates are examined and the minimal valid-to timestamp (`min_valid_to`) is found. The internal is then calculated by the following simple algorithm:

```pseudo
if (certificate is expiring) or (min_valid_to is within 1 minute from now) {
  return 0 // force renewal to trigger right away
}

if min_valid_to is within 3 minutes from now {
  return 1 minute
}

if min_valid_to is within 6 minutes from now {
  return 2 minutes
}

return 2/3 of the remaining time until min_valid_to has passed
```

#### Replacing certificates

Once the renewal interval is reached, then the certificate renewal operation is executed. A certificate signing request (CSR) is sent to the DPS service. When the response with new certificates is received, the new certificates are validated. If they are valid, then they replace the previously stored certificates. If they are not valid, they are discarded and the operation is repeated.
A new certificate renewal interval is calculated based on the new certificates, and renewal is scheduled again using this new interval.

That the algorithm is active can be detected by checking for `PLGD_DPS_RENEW_CREDENTIALS` provisioning status flag. The flag is added when the renewal starts and removed when it ends.

The algorithm can fail, and the possible failures are handled in the following way:

- sending of the CSR fails - scheduling of the renewal is retried (the same renewal interval calculation is used, however to avoid 100% CPU usage in case of a repeated send failure the calculated interval cannot be lower than the interval calculated by the retry mechanism used during provisioning).
- the response is not received - after successful sending of the CSR then operation is rescheduled using the same retry mechanism that's used during provisioning
- in the response handler if either an invalid response was received or a valid response was received, but the replacement of the current fails, then a full reprovisioning is triggered
- if any other failures occur during renewal, then a full reprovisioning is triggered

## Example application - dps_cloud_server

An example application called `dps_cloud_server` is available among [Iotivity-lite's examples](https://github.com/iotivity/iotivity-lite/tree/master/apps). The following sections describe how DPS client library is integrated in the binary (with code examples). And the final section shows how to run it.

### Bootstrapping device with manufacturer certificates

Currently, the handling of certificates is hard-coded into the binary. On start-up, the process expects the certificates to be in a folder named `pki_certs` located next to the binary file. The folder must contain 3 files:

- `dpsca.pem` - manufacturer certificate authority of the DPS
- `mfgcrt.pem` - manufacturer certificate of the device
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
  assert(dps_ctx != NULL);
  assert(cert_dir != NULL);
#define CERT_BUFFER_SIZE 4096

  char path[PATH_MAX];
  int dpsca_credid = -1;
  int mfg_credid = -1;
  if (plgd_dps_get_skip_verify(dps_ctx) || g_dhcp_enabled) {
    DPSCS_DBG("adding of manufacturer trusted root ca skipped\n");
  } else {
    unsigned char dps_ca[CERT_BUFFER_SIZE];
    size_t dps_ca_size = sizeof(dps_ca) / sizeof(unsigned char);
    dps_concat_paths(path, sizeof(path), cert_dir, "/dpsca.pem");
    if (dps_read_pem(path, (char *)dps_ca, &dps_ca_size) < 0) {
      printf("ERROR: unable to read %s\n", path);
      goto error;
    }
    dpsca_credid = oc_pki_add_mfg_trust_anchor(plgd_dps_get_device(dps_ctx),
                                               dps_ca, dps_ca_size);
    if (dpsca_credid < 0) {
      printf("ERROR: installing manufacturer trusted root ca\n");
      goto error;
    }
    DPSCS_DBG("manufacturer trusted root ca credid=%d\n", dpsca_credid);
  }

  unsigned char mfg_crt[CERT_BUFFER_SIZE];
  size_t mfg_crt_size = sizeof(mfg_crt) / sizeof(unsigned char);
  dps_concat_paths(path, sizeof(path), cert_dir, "/mfgcrt.pem");
  if (dps_read_pem(path, (char *)mfg_crt, &mfg_crt_size) < 0) {
    printf("ERROR: unable to read %s\n", path);
    goto error;
  }
  unsigned char mfg_key[CERT_BUFFER_SIZE];
  size_t mfg_key_size = sizeof(mfg_key) / sizeof(unsigned char);
  dps_concat_paths(path, sizeof(path), cert_dir, "/mfgkey.pem");
  if (dps_read_pem(path, (char *)mfg_key, &mfg_key_size) < 0) {
    printf("ERROR: unable to read %s\n", path);
    goto error;
  }
  mfg_credid = oc_pki_add_mfg_cert(plgd_dps_get_device(dps_ctx), mfg_crt,
                                   mfg_crt_size, mfg_key, mfg_key_size);
  if (mfg_credid < 0) {
    printf("ERROR: installing manufacturer certificate\n");
    goto error;
  }
  DPSCS_DBG("manufacturer certificate credid=%d\n", mfg_credid);
  oc_pki_set_security_profile(plgd_dps_get_device(dps_ctx), OC_SP_BLACK,
                              OC_SP_BLACK, mfg_credid);
  return 0;

error:
  if (dpsca_credid != -1) {
    if (oc_sec_remove_cred_by_credid(dpsca_credid,
                                     plgd_dps_get_device(dps_ctx))) {
      DPSCS_DBG("certificate(%d) removed\n", dpsca_credid);
    } else {
      printf("WARNING: failed to remove manufacturer trusted root ca(%d)\n",
             dpsca_credid);
    }
  }
  if (mfg_credid != -1) {
    if (oc_sec_remove_cred_by_credid(mfg_credid,
                                     plgd_dps_get_device(dps_ctx))) {
      DPSCS_DBG("certificate(%d) removed\n", mfg_credid);
    } else {
      printf("WARNING: failed to remove manufacturer certificate(%d)\n",
             mfg_credid);
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
    oc_new_string(&dev->name, g_dps_device_name, strlen(g_dps_device_name));
  }
  plgd_dps_manager_callbacks_t callbacks = {
    .on_status_change = dps_status_handler,
    .on_status_change_data = NULL,
    .on_cloud_status_change = cloud_status_handler,
    .on_cloud_status_change_data = NULL,
  };
  plgd_dps_set_manager_callbacks(dps_ctx, callbacks);
  if (g_expiration_limit != -1) {
    plgd_dps_pki_set_expiring_limit(dps_ctx, (uint16_t)g_expiration_limit);
  }
  if (g_observer_max_retry != -1) {
    plgd_dps_set_cloud_observer_configuration(dps_ctx,
                                              (uint8_t)g_observer_max_retry, 1);
  }
  plgd_dps_set_skip_verify(dps_ctx, g_skip_ca_verification != 0);
  for (int i = 0; i < g_dps_endpoint_count; i++) {
    size_t dps_endpoint_len = strlen(g_dps_endpoint[i]);
    if (dps_endpoint_len > 0 &&
        !plgd_dps_add_endpoint_address(dps_ctx, g_dps_endpoint[i],
                                       dps_endpoint_len, NULL, 0)) {
      printf("ERROR: failed to add endpoint address\n");
      return -1;
    }
  }
  if (dps_add_certificates(dps_ctx, g_dps_cert_dir) != 0) {
    printf("ERROR: failed to add initial certificates on factory reset\n");
    return -1;
  }
  plgd_dps_force_reprovision(dps_ctx);
  return 0;
}
```

### Factory reset

Use a factory reset handler function to react to a reset of the device. The function is also invoked on the first start of the device and thus can be used to call the configuration function.

{{< note >}}
If your device has run before then it should've created storage files in a folder next to the binary. During the initialization the data will be loaded from these storage files and the factory reset function might not be invoked. The configuration of the device will be based on the loaded data.
{{< /note >}}

To properly handle a device reset the handler must reload the manufacturer’s certificates, which have been removed by the reset and reconfigure the device. At the end of the handler, call the `plgd_dps_on_factory_reset` function, which does some additional clean-up and resets the provisioning state. Finally, restart the provisioning process by calling `plgd_dps_manager_start`.

```C
static void
factory_presets_cb(size_t device_id, void *data)
{
  (void)data;
  plgd_dps_context_t *dps_ctx = plgd_dps_get_context(device_id);
  if (dps_ctx == NULL) {
    DPSCS_DBG("skip factory reset handling: empty context\n");
    return;
  }

  if (plgd_dps_on_factory_reset(dps_ctx) != 0) {
    printf("ERROR: cannot handle factory reset\n");
    return;
  }
  if (manufacturer_setup(dps_ctx) != 0) {
    printf("ERROR: failed to configure device\n");
    return;
  }
  if (plgd_dps_manager_start(dps_ctx) != 0) {
    printf("ERROR: failed to start dps manager\n");
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
    printf("ERROR: cannot start dps manager: empty context\n");
    ... // cleanup
    return -1;
  }
  if (parsed_options.retry_configuration_size > 0 &&
      !plgd_dps_set_retry_configuration(
        dps_ctx, parsed_options.retry_configuration,
        parsed_options.retry_configuration_size)) {
    printf("ERROR: cannot start dps manager: invalid retry configuration\n");
    ... // cleanup
    return -1;
  }

  ...

  if (!plgd_dps_manager_is_started(dps_ctx)) {
    if (g_expiration_limit != -1) {
      plgd_dps_pki_set_expiring_limit(dps_ctx, (uint16_t)g_expiration_limit);
    }
    if (g_observer_max_retry != -1) {
      plgd_dps_set_cloud_observer_configuration(dps_ctx, (uint8_t)g_observer_max_retry, 1);
    }
    plgd_dps_set_skip_verify(dps_ctx, g_skip_ca_verification != 0);
    plgd_dps_manager_callbacks_t callbacks = {
      .on_status_change = dps_status_handler,
      .on_status_change_data = NULL,
      .on_cloud_status_change = cloud_status_handler,
      .on_cloud_status_change_data = NULL,
    };
    plgd_dps_set_manager_callbacks(dps_ctx, callbacks);
    if (plgd_dps_manager_start(dps_ctx) != 0) {
      printf("ERROR: failed to start dps manager\n");
      ... // cleanup
      return -1;
    }
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
  -h | --help                       print help
  -c | --create-conf-resource       create DPS configuration resource
  -e | --expiration-limit           set certificate expiration limit (in seconds)
  -l | --log-level                  set runtime log-level of the DPS library (supported values: disabled, trace, debug, info, notice, warning, error)
     | --oc-log-level               set runtime log-level of the IoTivity library (supported values: disabled, trace, debug, info, notice, warning, error)
  -n | --no-verify-ca               skip loading of the DPS certificate authority
  -f | --dhcp-leases-file           path to the dhcp leases file (default: /var/lib/dhcp/dhclient.leases)
  -x | --dhcp-enabled               pull dhcp leases file every 5sec
  -o | --cloud-observer-max-retry   maximal number of retries by cloud observer before forcing reprovisioning
  -r | --retry-configuration        retry timeout configuration (array of non-zero values delimited by ',', maximum of 8 values is accepted; example: 1,2,4,8,16)
  -s | --set-system-time            use plgd time to set system time (root required)
  -w | --wait-for-reset             don\'t start right away, but wait for SIGHUP signal


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
