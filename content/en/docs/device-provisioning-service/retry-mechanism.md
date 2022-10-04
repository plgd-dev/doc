---
title: 'Retry mechanism'
description: 'How is recoverable failure handled?'
date: '2022-06-28'
lastmod: '2022-06-28'
categories: [zero-touch, provisioning]
keywords: [retry, recovery, failure]
menu:
  docs:
    parent: device-provisioning-service
    weight: 50
toc: true
---

Various failures can occur as a DPS device is running. Some failures are recoverable and are handled by retrying the failed step or triggering a full reprovisioning.

The mechanism is illustrated by the following diagram:

![Retry mechanism](../static/retry-mechanism.drawio.svg)

The used retry interval is determined by the retry counter and the retry configuration. Internally, DPS client library uses a counter to keep track of the number of retries and timeouts.

## Configuration

The retry configuration consists of an array of non-zero values, which are interpreted as retry intervals in seconds. The maximal size of the configuration array is 8. By default, the retry configuration is initialized to the following array of values:

```C
[10, 20, 40, 80, 120]
```

The configuration can be changed by the `plgd_dps_set_retry_configuration` function. To retrieve the current configuration use the `plgs_dps_get_retry_configuration` function.

## Failures during provisioning

Provisioning consists of 3 main steps:

* sending of signing certificate request
* requesting and applying of ACLs
* requesting and applying of plgd hub connection configuration

Each step sends a request to the DPS service and waits for response. After a request is sent, then the retry interval is used as a deadline. If the response is not received before this deadline, the operation timeouts and the request is resend. If the response is received in time, then its status code is checked. We distinguish between transient and non-transient errors.

Non-transient errors force a full provisioning on retry. Transient errors first try to repeat the failed step in case the problem clears up. However, if a transient failure occurs consecutively 3 times, then a full reprovisioning is forced.

The retry counter starts with zero. Each retry, after either a transient or a non-transient error or a timeout, increments the retry counter. The counter is reset to zero after a provisioning step successfully finishes. If the value of the retry counter is greater than the maximal index of the configuration array, then the counter is also reset back to zero.

## Failures of cloud manager connection and authentication

After a successful provisioning, the device disconnects from the DPS service and starts up the cloud manager in `IoTivity-lite`. If the cloud manager fails to start, then a full reprovisioning is triggered. If the cloud manager starts successfully, then a cloud status observer also starts to operate.

Cloud status observer is a simple polling mechanism which examines the cloud status value 30 times in 1 second intervals. The observer checks the cloud status and waits for the status to have both `OC_CLOUD_REGISTERED` and `OC_CLOUD_LOGGED_IN` flags sets. If the flags are set, then the polling stops. If the limit of polling checks is reached and the flags are still not set, then the cloud manager is stopped and a full reprovisioning is forced.

The limit of polling checks (default: 30) and the interval (default: 1 second) can be configured by the `plgd_dps_set_cloud_observer_configuration` function.

{{< note >}}
Valid authentication of cloud manager depends on a valid access token. If the access token retrieved during provisioning is not permanent, it will eventually expire. It must be refreshed, because otherwise #plgd hub will close the connection to the device. This is handled internally by IoTivity-lite library, which schedules a refresh token operation before the access token expires.
{{< /note >}}
