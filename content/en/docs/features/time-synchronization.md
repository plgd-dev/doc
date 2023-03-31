---
title: 'Time Synchronization'
description: 'How to synchronize time on the device?'
date: '2023-03-20'
categories: [features]
keywords: [time-synchronization, iotivity-lite, provisioning, zero-touch, TLS, NTP, GPS, time server]
weight: 10
---

When a device experiences a disruption in power supply, such as during a power outage or when the device's motherboard clock battery fails, the time on the device can become unsynchronized. Similarly, if the battery of the device runs out of power, the time can also become desynchronized.

This seemingly small issue can cause significant problems, especially for secure devices that rely on TLS certificates verification. TLS certificates are a crucial element of secure communication and are used to establish trust between two parties. During the verification process, the validity of the certificate is checked, including the time at which it was issued and the time at which it expires. If the device's time is incorrect, the verification process will fail, so it won't trust the service certificate, which might be too far in the future or the opposite. Thus, ensuring that the time on a device is accurate is of utmost importance in maintaining the security and integrity of secure communication channels.

To prevent these potential problems, it is essential that the device is capable of synchronizing its time with an external source. By utilizing an external time source, the device can accurately set its time and perform verification of TLS certificates. There are multiple methods for synchronizing the time on a device, including NTP, GPS, or a dedicated time server. Nevertheless, the most prevalent approach for synchronizing time on a device involves using the time server provided by the cloud platform. In the event that the device is unable to establish connectivity to any external time source, it can use the last synchronized time as a rough approximation of the current time.

## Synchronization time with device provisioning service

If this feature is enabled and the device's time is unsynchronized, the device will synchronize its time with the hub as the first step of the initialization process. The time synchronization is performed by sending a CoAP request to the `/x.plgd.dev/time` resource of the service, which is sent over a secure connection and undergoes TLS certificate verification. During the time synchronization process, the device will accept the hub certificate, even if it is either valid in the future or has already expired. If this occurs, the device will terminate the connection after completing the time synchronization and resume its operations as normal.

{{< note >}}
It is worth noting that enabling the time synchronization feature on IoTivity-lite requires an additional step during configuration through CMake. Specifically, this can be done by adding the `cmake -DPLGD_DEV_TIME_ENABLED=ON -DOC_CLOUD_ENABLED=ON ...` command to enable the feature.
{{< /note >}}

## IoTivity-Lite Time

IoTivity Lite provides a resource that can be used to synchronize time on a device. To allow this feature you need to compile IoTivity-lite with `cmake -DPLGD_DEV_TIME_ENABLED=ON ...`. When you set the time, device will store the time to the persistent storage and it will be restored after the device restart. So if the device time is in the past during initialization the time from persistent storage will be used as current time. If you have multiple devices in iotivity-lite, the time resource will be shared among them.

The definition time resource is available in [swagger](https://petstore.swagger.io/?url=https://raw.githubusercontent.com/iotivity/iotivity-lite/master/api/plgd/x.plgd.dev.time.yaml)

{{< note >}}

If you are interested in using this feature in a Docker container, note that it is already enabled in the [cloud_server](https://github.com/iotivity/iotivity-lite/pkgs/container/iotivity-lite%2Fcloud-server-discovery-resource-observable) Docker image.

To run the image with a specific time, for example January 1st, 2000 at 11:12:13, use the following command:

`docker run --rm -it --pull always -e FAKETIME="@2000-01-01 11:12:13" ghcr.io/iotivity/iotivity-lite/cloud-server-discovery-resource-observable:master`

{{< /note >}}

### Calculating current time

In order to calculate the current time, we utilize the monotonic time of the system in conjunction with the last synchronized time. Monotonic time represents time that is continuously increasing from a starting point, typically the system boot time, and is not affected by changes to the system clock that would otherwise affect the system time. The current time is then computed by adding the elapsed time since synchronization to the `lastSyncTime`. Upon setting lastSyncTime, the system's monotonic time is also recorded. When determining the elapsed time, we simply calculate the difference between the monotonic time at synchronization and the current monotonic time at the time of the query for the current time.

```pseudo-code
elapsed time = monotonic time now - monotonic time at synchronization

current time = lastSyncTime + elapsed time
```

### C-API

When initializing the time feature via the `plgd_time_init` function in the C-API, there are several parameters that can be set. These include specifying whether to use the time in MbedTLS time callback for TLS certificate verification and specifying a callback function to set the device clock. For more information about these parameters and other functions in the C-API such as set/get time, please refer to the [doxygen documentation](http://iotivity.org/iotivity-lite-doxygen/group__time__synchronization.html).

{{< note >}}
For an example of how to implement this feature, please refer to the [cloud_server.c](https://github.com/iotivity/iotivity-lite/blob/cf33fe739a640c301735ca60d5ac390194a430fb/apps/cloud_server.c#L915) file in the iotivity-lite repository.
{{< /note >}}
