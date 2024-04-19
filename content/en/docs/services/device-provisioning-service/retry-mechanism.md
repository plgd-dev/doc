---
title: 'Retry mechanism'
description: 'How is recoverable failure handled?'
date: '2022-06-28'
categories: [zero-touch, provisioning]
keywords: [retry, recovery, failure]
weight: 5
---

Various failures can occur as a DPS device is running. Some failures are recoverable and are handled by retrying the failed step or triggering a full reprovisioning.

The mechanism is illustrated by the following diagram:

![Retry mechanism](/docs/services/device-provisioning-service/static/retry-mechanism.drawio.svg)

The used retry interval is determined by the retry counter and the retry configuration. Internally, DPS client library uses a counter to keep track of the number of retries and timeouts.

## Configuration

The retry configuration consists of an array of non-zero values, which are interpreted as retry intervals in seconds. The configuration array must contain at least one value and the maximal size of is 8. By default, the retry configuration is initialized to the following array of values:

```C
[10, 20, 40, 80, 120]
```

The configuration can be changed by the `plgd_dps_set_retry_configuration` function. To retrieve the current configuration use the `plgs_dps_get_retry_configuration` function.

## Failures during provisioning

Provisioning consists of 3 main steps:

* synchronization of time
* requesting and set of device owner
* sending of signing certificate request
* requesting and applying of ACLs
* requesting and applying of plgd hub connection configuration

Each step sends a request to the DPS service and waits for response. After a request is sent, then the retry interval is used as a deadline. If the response is not received before this deadline, the operation timeouts and the request is resend. If the response is received in time, then its status code is checked. We distinguish between transient and non-transient errors.

Non-transient errors force a full provisioning on retry. Transient errors first try to repeat the failed step in case the problem clears up. However, if a transient failure occurs consecutively 3 times, then a full reprovisioning is forced.

The retry counter starts with zero. Each retry, after either a transient or a non-transient error or a timeout, increments the retry counter. The counter is reset to zero after a provisioning step successfully finishes. If the value of the retry counter is greater than the maximal index of the configuration array, then the counter is also reset back to zero.

### Retry action

Each failure or timeout triggers a retry action that calculates the retry delay (how long after a failure is the step or full reprovisioning retried) and timeout based on the configuration. By default, it is calculated like this:

```pseudocode

  timeout = take the timeout value from configuration array indexed by the retry counter

  delay = timeout / 2;
  // Include a random delay to prevent multiple devices from attempting to
  // connect or make requests simultaneously.
  delay += random value % delay;

```

Moreover, once the retry counter reaches higher value than the maximal index of the configuration array, not only is the counter reset back to zero, but the library attempts to change the selected DPS endpoint. If the are more than one DPS endpoint servers configured, then the selected endpoint is changed to the next in the list (the list is considered circular, so the next endpoint after the last endpoint is the first endpoint).

To set up a custom retry action use `plgd_dps_set_schedule_action`.

## Failures of cloud manager connection and authentication

After successfully provisioning, the device disconnects from the DPS service and initiates the cloud manager within `IoTivity-lite`. If the cloud manager fails to start, it triggers a full reprovisioning process. Conversely, if the cloud manager starts successfully, a cloud status observer also activates.

The cloud status observer operates as a simple polling mechanism, checking the cloud status value 30 times at 1-second intervals. It waits for the status to have both `OC_CLOUD_REGISTERED` and `OC_CLOUD_LOGGED_IN` flags set. Once these flags are set, the polling stops. The observer restarts the polling mechanism if the connection to the PLGD hub is lost.

You can configure the limit of polling checks (default: 30) and the interval (default: 1 second) using the `plgd_dps_set_cloud_observer_configuration` function.

{{< note >}}
Successful authentication of the cloud manager relies on a valid access token. If the access token retrieved during provisioning is not permanent, it will eventually expire. To prevent the PLGD hub from closing the connection to the device, the access token must be refreshed. This refresh operation is handled internally by the `IoTivity-lite` library, which schedules a token refresh operation before the access token expires.
{{< /note >}}

### Changing cloud servers on repeated failures

If the limit of polling checks is reached and the required flags are still not set, the cloud manager attempts to connect using other cloud server addresses available in the configuration. When attempting different cloud server addresses, some DPS steps may need to be redone.

If the IDs of the cloud servers in the configuration differ, it implies that certificates and ACLs might also be different. Therefore, reprovisioning of credentials and ACLs is triggered. If the IDs are the same, DPS provisioning is not triggered, and the cloud manager is simply restarted with the new address.

If the observer goes through all the addresses without establishing a successful connection, the cloud manager is stopped, and a full DPS reprovisioning is forced.
