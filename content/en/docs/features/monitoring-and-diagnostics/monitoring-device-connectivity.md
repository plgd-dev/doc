---
title: 'Monitoring Device Connectivity'
description: 'Learn how to determine the online or offline status of connected devices'
date: '2023-08-08'
categories: [features,iot-connectivity]
keywords: [service, coap-gateway, network-monitoring]
weight: 2
---

In the context of device connectivity, the CoAP gateway plays a crucial role in managing connections, authentication, and authorization. One of the key processes is the `UpdateDeviceMetadata` process within the ResourceAggregate that gets initiated when devices are successfully connected and verified. This process updates the connection state of the device to 'Online'. The validity of the online status is determined based on the expiration of the JWT (JSON Web Token).

Conversely, when a device is disconnected or when the CoAP gateway is gracefully shut down (e.g., using SIGTERM), an event is triggered to indicate that the device has gone offline. However, there are various scenarios to consider, such as unexpected termination of the CoAP gateway due to factors like exceeding memory limits in a Kubernetes (k8s) environment or power loss.

## Managing Service Status Updates

Every instance of the CoAP gateway is assigned a unique ID during initialization. These IDs are not only stored in a database but are also linked to device metadata to facilitate online state updates.

At regular intervals, each CoAP gateway instance collaboratively updates a shared record containing their respective information, primarily their unique IDs. This update takes place periodically, typically once every minute. The specific record being updated changes over time, following a cyclic pattern (e.g., within a 1-minute period, each update corresponds to a different record). In the database, both the current shared record and the preceding one are retained.

To calculate the appropriate `recordID` for updating the record within a 1-minute period, the following logic in Golang is employed:

```golang
    now := time.Now().UnixNano()
    period := 1 * time.Minute
    recordID = now - (now % int64(period))
```

Upon the generation of a new record, all records with a `recordID` corresponding to the current time minus twice the period are utilized to construct a merged list of all CoAP gateways that were operational before. Subsequently, CoAP gateways running within the time range of the current time and the current time minus twice the period are excluded from this list. Any remaining CoAP gateways in the list are deemed terminated, triggering the use of the Resource Aggregate command `TerminateConnectionsByService` to set associated devices with those CoAP gateways to an offline state. Subsequently, all the previous records that pertained to past operational periods are purged.

{{< warning >}}

The `TerminateConnectionsByService` function is designed exclusively for service usage. As a result, this command does not rely on JWT for authorization. It's important to note that this function should not be callable from public endpoints. Instead, the sole method of authorization employed is mutual TLS authentication.

{{< /warning >}}
