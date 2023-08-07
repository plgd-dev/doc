---
title: 'Monitoring Device Connectivity'
description: 'Understanding how to determine if a device is online or offline'
date: '2023-08-07'
categories: [features]
keywords: ['service']
weight: 2
---

When devices are successfully connected, authenticated, and authorized, the CoAP gateway initiates an `UpdateDeviceMetadata` process within the ResourceAggregate. This process sets the device's connection state to 'Online' and the online validity is determined by JWT expiration. Conversely, when a device becomes disconnected or the CoAP gateway is gracefully shut down (e.g., using SIGTERM), an event is sent indicating that the device has gone offline. However, there are scenarios to consider, such as when the CoAP gateway terminates unexpectedly due to reasons like being killed for exceeding memory limits in a Kubernetes (k8s) environment or due to power loss.

## Updating Service Status

Each instance of the CoAP gateway generates a unique ID during initialization. These IDs are stored in a database and are also associated with device metadata updates to indicate the online state.

At regular intervals, all instances of the CoAP gateway update a shared record named `ServiceMetadataUpdated` with their own information, primarily their unique IDs. This update occurs periodically, such as once every minute. The specific record being updated changes over time (e.g., for a 1-minute period, each update corresponds to a different record). In the database, both the current shared record and the previous one are retained. Once a new record is generated, the previous shared record becomes irrelevant. The new record is streamed as an event named 'ServiceMetadataUpdated', and the old record is subsequently deleted.

Calculating the recordID for updating the record (e.g., for a 1-minute period):

```golang
    now := time.Now().UnixNano()
    period := 1 * time.Minute
    recordID = now - (now % int64(period))
```

## Determining Device Online Status

To assess whether a device is currently online, we rely on the most recent `DeviceMetadataUpdated` event. If the device is marked as online (`DeviceMetadataUpdated.connection.state`) and validity is not expired(`DeviceMetadataUpdated.connection.online_valid_until) in this event, we proceed to examine when this online status was set. If the time difference between the current time and the device's last update time is less than a specified interval (e.g., 1 minute), we consider the device to be online.

In cases where the time difference exceeds the interval, we compare the `ServiceMetadataUpdated` record's IDs with the CoAP gateway ID provided in the `DeviceMetadataUpdated` event. If a match is found between these IDs, the device is considered as online.
