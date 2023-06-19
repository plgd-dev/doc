---
title: 'HTTP Gateway'
description: 'What is plgd HTTP Gateway?'
date: '2023-06-16'
categories: [gateway, iot]
keywords: [http, iot]
weight: 1
isMainSectionPage: true
---

The HTTP Gateway is a crucial component that facilitates control over devices in the control plane by utilizing the HTTP and WebSocket protocols. It serves as an interface for managing various operations, all of which are seamlessly integrated with the gRPC Gateway API.

Here's an overview of the functionalities provided by the HTTP Gateway:

1. Device Management:
   - GetDevices: Retrieves a list of devices.
   - DeleteDevices: Deletes selected devices.
   - GetResourceLinks: Retrieves the resource links of devices.
   - GetResourceFromDevice: Retrieves a specific resource from a device.
   - GetResources: Retrieves resources from the device twin.
   - UpdateResource: Updates a resource on the device.
   - DeleteResource: Deletes a resource on the device.
   - CreateResource: Creates a resource on the device.

2. Event Handling and Subscription (via WebSocket):
   - SubscribeToEvents: Subscribes to events associated with devices.
   - GetEvents: Retrieves events for a given combination of device ID, resource ID, and timestamp.

3. Device Metadata and Configuration:
   - GetHubConfiguration: Retrieves the hub configuration.
   - UpdateDeviceMetadata: Enables or disables device twin.
   - GetDevicesMetadata: Retrieves metadata of devices, including online/offline status and device twin status.
   - CancelPendingMetadataUpdates: Cancels pending device metadata updates.

4. Command Handling:
   - GetPendingCommands: Retrieves pending commands for devices.
   - CancelPendingCommands: Cancels resource commands.

For more detailed information, refer to the [HTTP Gateway API documentation](/docs/services/http-gateway/http-api). Additionally, specific details about the integration with the gRPC Gateway API can be found [here](/docs/services/grpc-gateway/grpc-api).
