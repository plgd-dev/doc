---
title: 'Time Synchronization'
description: 'How to synchronize time on the device?'
date: '2023-03-20'
categories: [features]
keywords: [time-synchronization, iotivity, TLS]
weight: 10
---

The time on a device can become desynchronized (for example during a power outage on a device without a motherboard clock battery). This can cause several issues, one of them being TLS certificates verification for secure devices. The verification checks the time validity of the certificate

## Synchronizing time with Iotivity-Lite device

To solve the issue of desynchronized time, the resource `/x.plgd.dev/time` can be used.
It is a plafform wide resource, shared by all devices of a process (similar to the `/oic/p` resource.)

The resource exposes 3 properties: `time`, `lastSyncedTime`, `status`.

* `time` is a readonly property and it is the current time calculated from the runtime data of the resource
* `lastSyncedTime` is writable property and represents the UTC time at the moment of synchronization
* `status` is a readonly property that can have two values `in-sync` or `syncing`, the values describe the current time synchronization status

On secure devices, properties `lastSyncedTime` and `status` are not retrieved by GET requests sent over an insecure connection.

### Calculating current time

Current time is calcuted as `lastSyncTime` + time elapsed since synchronization. When the value of `lastSyncTime` is set, the runtime also stores the monotonic time of the system at the time of synchronization. Elapsed time is then simply calculated as the difference between the monotonic time at synchronization and the monotonic time at the moment of querying for current time.

```pseudo-code
elapsed time = monotonic time now - monotonic time at synchronization

current time = lastSyncTime + elapsed time
```

TODO: device restart

### C-API

TODO: synchronization time

TODO: integrate with mbedTLS

TODO: set system time

### Coap request

TODO: link to swagger
