---
title: 'gRPC Gateway'
description: 'What is plgd gRPC Gateway?'
date: '2023-06-16'
categories: [gateway, iot]
keywords: [grpc, iot]
weight: 1
isMainSectionPage: true
---

The gRPC Gateway is an integral component that empowers you to control devices in the control plane using the gRPC (Google Remote Procedure Call) protocol.

Here's an overview of the functionalities provided by the gRPC Gateway:

1. Device Management:
   - GetDevices: Retrieves a list of devices.
   - DeleteDevices: Deletes selected devices.
   - GetResourceLinks: Retrieves the resource links of devices.
   - GetResourceFromDevice: Retrieves a specific resource from a device.
   - GetResources: Retrieves resources from the device twin.
   - UpdateResource: Updates a resource on the device.
   - DeleteResource: Deletes a resource on the device.
   - CreateResource: Creates a resource on the device.

2. Event Handling and Subscription:
   - SubscribeToEvents: Subscribes to events related to devices.
   - GetEvents: Retrieves events for a given combination of device ID, resource ID, and timestamp.

3. Device Metadata and Configuration:
   - GetHubConfiguration: Retrieves the hub configuration.
   - UpdateDeviceMetadata: Enables or disables device twin.
   - GetDevicesMetadata: Retrieves metadata of devices, including online/offline status or device twin status.
   - CancelPendingMetadataUpdates: Cancels pending device metadata updates.

4. Command Handling:
   - GetPendingCommands: Retrieves pending commands for devices.
   - CancelPendingCommands: Cancels resource commands.

For more detailed information, please refer to the [gRPC Gateway API documentation](/docs/services/grpc-gateway/grpc-api).