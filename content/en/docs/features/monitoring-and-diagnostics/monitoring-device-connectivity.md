---
title: 'Monitoring Device Connectivity'
description: 'Learn how to determine the online or offline status of connected devices'
date: '2023-08-08'
categories: [features,iot-connectivity]
keywords: [service, coap-gateway, network-monitoring]
weight: 2
---

The CoAP gateway is responsible for the connection management including the up to date representation of the device status. This information is driven through the Resource Aggregate command - `UpdateDeviceMetadata`, that gets initiated when device connection status changes - device connects or disconnects from the CoAP Gateway. However, there are various scenarios to consider, such as unexpected termination of the CoAP Gateway, due to factors like node failure, power loss or others.

## Managing Service Status Updates

Every process instance of the CoAP gateway generates a unique ID during initialization. These IDs are not only stored in a database but are also linked to device metadata to facilitate online state updates.

Periodically, each CoAP gateway process instance updates a shared record in database by appending its unique ID. The specific record being updated changes over time, following a cyclic pattern (e.g., within a 1-minute period, each update corresponds to a different record). In the database, both the current shared record and the preceding one are retained.

```golang
    type SharedRecord struct {
        RecordID          int64 `bson:"_id"`      // ID of the record moving forward in time
        CoAPGatewayIDs    []string `bson:"coapgatewayids"` // IDs of all CoAP gateways that were operational during the period
    }
```

To calculate the `recordID` for updating the record within a 1-minute period, the following logic in Golang is employed:

```golang\(recordID-1\)
    now := time.Now().UnixNano()
    period := 1 * time.Minute
    recordID = now - (now % int64(period))
```

Upon the generation of a new record, all records with a `recordID` corresponding to the current time minus twice the period are utilized to construct a merged list of all CoAP gateways that were operational before. Subsequently, CoAP gateways running within the time range of the current time and the current time minus twice the period are excluded from this list. Any remaining CoAP gateways in the list are deemed terminated, triggering the use of the Resource Aggregate command `TerminateConnectionsByService` to set associated devices with those CoAP gateways to an offline state. Subsequently, all the previous records that pertained to past operational periods are purged.

{{< warning >}}

The `TerminateConnectionsByService` function is designed exclusively for service usage. As a result, this command does not rely on JWT for authorization. It's important to note that this function should not be callable from public endpoints. Instead, the sole method of authorization employed is mutual TLS authentication.

{{< /warning >}}

### Example

We expanded our setup to include two CoAP gateways within a k8s environment, operating on a 1-minute check interval. In the database, both the previous(`recordID-0`) and current(`recordID-1`) records held the IDs UUID-0 and UUID-1 for the two CoAP gateways. However, one instance (UUID-1) experienced an OOM (Out of Memory) event, resulting in a restart. This led to two CoAP gateway instances existing concurrently, and the current record(`recordID-1`) now encompassed UUID-0, UUID-1, and the new UUID-2 due to the restarted gateway receiving a new UUID.

As time advanced by 1 minute, a new record(`recordID-2`) was created, prompting an examination of the preceding records. The earliest previous record(`recordID-0`) included UUID-0 and UUID-1, while the most recent previous record(`recordID-1`) held UUID-0, UUID-1, and UUID-2. During this scenario, the coap-gateway hadn't yet detected the termination of one of the gateways, and consequently, all records(`recordID-0`) preceding the last one were deleted.

After another minute had passed, a fresh record was established, leading to an assessment of the past two records in chronological order. The first of these previous records(`recordID-1`) encompassed UUID-0, UUID-1, and UUID-2, whereas the most recent record contained only UUID-0 and UUID-2(`recordID-2`). At this juncture, the termination of UUID-1 was successfully identified, allowing the coap-gateway responsible for record creation to transition all devices connected to the terminated gateway into an offline state.
