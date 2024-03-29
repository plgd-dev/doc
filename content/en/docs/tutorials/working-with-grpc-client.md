---
title: 'Working with gRPC Client'
description: 'Understading API of gRPC Gateway and how to use it in your application'
date: '2021-05-13'
categories: [tutorials, grpc gateway]
keywords: [grpc, gateway, client]
weight: 5
---

For creating grpc-client you need to generate a code for your language from proto files, which are stored at [plgd hub](https://github.com/plgd-dev/hub/tree/main/grpc-gateway/pb). The plgd gRPC Gateway uses TLS. The client needs to have therefore properly configured TLS. Here is a simple example how to create a secured gRPC client communicating with the plgd gRPC Gateway.

```go
import (
    "google.golang.org/grpc"
    "google.golang.org/grpc/credentials"
    "github.com/plgd-dev/hub/v2/grpc-gateway/pb"
    "github.com/plgd-dev/hub/v2/grpc-gateway/client"
)

    ...
    // Create TLS connection to the grpc-gateway.
    gwConn, err := grpc.Dial(
        address,
        grpc.WithTransportCredentials(credentials.NewTLS(tlsConfig)),
    )
    if err != nil {
        panic("cannot connect to grpc-gateway: " + err.Error())
    }
    // Create basic client which was generated from proto files.
    basicClient := pb.NewGrpcGatewayClient(gwConn)

    // Create Extended client which provide us more friendly functions.
    extendedClient := client.NewClient(basicClient)
    ...
```

## Using extended gRPC Client

More info in [doc](https://pkg.go.dev/github.com/plgd-dev/hub/grpc-gateway/client).

## API

All requests to the service must contain a valid access token in the [grpc metadata](https://github.com/grpc/grpc-go/blob/master/Documentation/grpc-auth-support.md#oauth2).

{{< warning >}}
Each request to the gRPC Gateway shall contain a valid access token as a part of the [grpc metadata](https://github.com/grpc/grpc-go/blob/master/Documentation/grpc-auth-support.md#oauth2).
{{< /warning >}}

### Get Devices

The `GetDevices` command supports various filter options. If all of them are **unset**, all devices of a user identified by the access token are returned.

**Example usages of filter options:**

- to retrieve certain devices use `GetDevicesRequest.device_id_filter` where ids of these devices need to be set
- to retrieve all offline devices set `GetDevicesRequest.status_filter` to `OFFLINE`
- to retrieve all devices of certain types use `GetDevicesRequest.type_filter` (e.g. `x.com.plgd.light`)

To return only `ONLINE` devices with ids `deviceID1` and `deviceID2`, the following options shall be set: `GetDevicesRequest.device_id_filter("[deviceID1, deviceID2]") && GetDevicesRequest.status_filter([ONLINE])`.

### Get Resource Links

The `GetResourceLinks` command supports various filter options. If all of them are **unset**, all links of all devices user is authorized to use are returned.

**Example usages of filter options:**

- to retrieve links of certain devices use `GetResourceLinksRequest.device_id_filter` where ids of these devices needs to be set
- to retrieve links of certain types use `GetResourceLinksRequest.type_filter` (e.g. `oic.r.switch.binary`)

To return only binary switches resources hosted by devices with ids `deviceID1` and `deviceID2`, the following options shall be set: `GetResourceLinksRequest.device_id_filter("[deviceID1, deviceID2]") && GetResourceLinksRequest.type_filter([oic.r.switch.binary])`.

### Get Resource Content

The `GetResources` command supports various filter options. If all of them are **unset**, all resource contents (device twin) of all devices user is authorized to use are returned.

**Example usages of filter options:**

- to retrieve of resources identified by their hrefs use `GetResourcesRequest.resource_id_filter` where combinations `deviceID` and `href` is required  in format `deviceID/href`
- to retrieve resource values of certain devices use `GetResourcesRequest.device_id_filter` where ids of these devices need to be set
- to retrieve values from resources of a specific type use `GetResourcesRequest.type_filter`

To return values of binary switch resources hosted by devices with ids `deviceID1` and `deviceID2`, following options shall be set: `GetResources.device_id_filter("[deviceID1, deviceID2]") && GetResources.type_filter([oic.r.switch.binary])`.

### Subscribe to Events

The `SubscribeToEvents` command opens the stream, which content is driven by the control message.

**To control the content of the stream, send a `SubscribeToEvents` message with the following options for example:**

- Set `action.create_subscription.event_filter` to `DEVICE_METADATA_UPDATED` to receive events for devices that have changed their status (e.g., ONLINE/OFFLINE) among the selected devices in `device_id_filter`.
- Set `action.create_subscription.{device_id_filter, event_filter}` and set `event_filter` to `RESOURCE_PUBLISHED, RESOURCE_UNPUBLISHED` to receive device events for the selected devices in `device_id_filter`.
- Use `action.create_subscription.{href_filter}` to receive resource events from all devices for the selected resources specified in `href_filter`.
- Set `action.create_subscription.{device_id_filter, href_filter, event_filter}` and set `event_filter` to `CONTENT_CHANGED` to receive resource events from filtered resources specified in `href_filter` among the selected devices in `device_id_filter`.
- Use `action.create_subscription.{resource_id_filter}` to receive resource events from the selected device resources specified in `resource_id_filter`.
- If no filters are set, using `action.create_subscription` alone will receive all device events from the hub.

The first event returned after the successful subscription is of type `OperationProcessed`. Property `OperationProcessed.error_status.code` contains information if the subscription was successful. If it was successful, property `subscriptionId` is set. All events belonging to a single `SubscribeToEvents` request are then identified by this `subscriptionId`.

If the user loses a device _(unregistered / no more shared with the user)_, the client receives an event `SubscriptionCanceled` with corresponding `subscription_id`.

### Get Resource from Device

The `GetResourceFromDevice` retrieves resource content directly from the device - device twin resource value is not returned. To define the expiration of command, set `GetResourceFromDeviceRequest.time_to_live` in nanoseconds(minimal 100ms). If the pending event is expired `GetResourceFromDevice.valid_until` (Unix timestamp in nanoseconds, 0 means forever), the hub skips it and will be removed by creating a new snapshot event.
> This command execution is "expensive" as it has to reach the real device while your client synchronously waits for a response.

### Update Resource Content

The `UpdateResource` command requests resource updates on the device. To define the expiration of command, set `UpdateResourceRequest.time_to_live` in nanoseconds(minimal 100ms). If the pending event is expired `ResourceUpdatePending.valid_until` (Unix timestamp in nanoseconds, 0 means forever), the hub skips it and will be removed by creating a new snapshot event.

### Create Resource

The `Create Resource` command requests the creation of a new resource on a specific collection on the device. To define the expiration of command, set `CreateResourceRequest.time_to_live` in nanoseconds(minimal 100ms). If the pending event is expired `ResourceCreatePending.valid_until` (Unix timestamp in nanoseconds, 0 means forever), the hub skips it and will be removed by creating a new snapshot event.

### Delete Resource

The `DeleteResource` command requests the device to delete a specific resource. A confirmation message doesn't mean that the resource was deleted. After successful deletion, the device unpublishes its resource. This information is propagated to the client in the form of a `RESOURCE_UNPUBLISHED` event. To define the expiration of command, set `DeleteResourceRequest.time_to_live` in nanoseconds(minimal 100ms). If the pending event is expired `ResourceDeletePending.valid_until` (Unix timestamp in nanoseconds, 0 means forever), the hub skips it and will be removed by creating a new snapshot event.

### Get pending commands

The `GetPendingCommands` command supports various filter options. If all of them are **unset**, all pending commands of all devices the user is authorized to use are returned.

**Example usages of filter options:**

- to retrieve pending commands of resources identified by their hrefs use `GetPendingCommandsRequest.resource_id_filter` where combinations `deviceID` and `href` is required in format `deviceID/href`
- to retrieve pending commands of certain devices use `GetPendingCommandsRequest.device_id_filter` where ids of these devices need to be set
- to retrieve pending commands of a specific type of resources use `GetPendingCommandsRequest.type_filter`
- to retrieve pending commands of certain commands use `GetPendingCommandsRequest.command_filter`

To return certain pending commands of binary switch resources hosted by devices with ids `deviceID1` and `deviceID2`, following options shall be set: `GetPendingCommandsRequest.device_id_filter("[deviceID1, deviceID2]") && RetrievePendingCommandsRequest.type_filter([oic.r.switch.binary])`.

### Get devices metadata

The `GetDevicesMetadata` command supports various filter options. If all of them are **unset**, all metadata of all devices the user is authorized to use are returned. Metadata contains information about [connection status](https://github.com/plgd-dev/hub/blob/main/resource-aggregate/pb/commands.proto#L433) and [twin synchronization state](https://github.com/plgd-dev/hub/blob/main/resource-aggregate/pb/commands.proto#L477).

**Example usages of filter options:**

- to retrieve metadata of certain devices use `GetDevicesMetadataRequest.device_id_filter` where ids of these devices need to be set
- to retrieve metadata a specific type of devices use `GetDevicesMetadataRequest.type_filter`

### Update Device Metadata

The `UpdateDeviceMetadata` command requests enable/disable the device twin. To define the expiration of command, set `UpdateDeviceMetadata.time_to_live` in nanoseconds(minimal 100ms). If the pending event is expired `DeviceMetadataUpdatePending.valid_until` (Unix timestamp in nanoseconds, 0 means forever), the hub skips it and will be removed by creating a new snapshot event.

### Get events

The `GetEvents` command returns events that occurred on a resource. The command supports several filtering options. If all of them are **unset** all events from resources belonging to given user devices are returned.

**Example usages of filter options:**

- to retrieve events for all user resources that occurred after given time use `GetEventsRequest.timestamp_filter` and set it to Unix timestamp in nanoseconds that represents given time.
- to retrieve events for resources from selected user device(s) after given time combine `GetEventsRequest.device_id_filter` and `GetEventsRequest.timestamp_filter`. To return events from devices with ids `deviceID1` and `deviceID2` that occurred after given time do the following steps. Set the device filter to `GetEventsRequest.device_id_filter("[deviceID1, deviceID2]")`, then transform the desired time into Unix `timestamp` in nanoseconds and set `GetEventsRequest.timestamp_filter(timestamp)`.
- to retrieve events for selected user resources after given time combine `GetEventsRequest.resource_id_filter` and `GetEventsRequest.timestamp_filter`. The required format for `resource_id_filter` of a given resource is `deviceID/href`, where `deviceID` is the id of the device by which the resource is hosted and `href` is the location of the resource on the host device. To return events from resources `deviceID1/href1` and `deviceID2/href2` set resource filter to `GetEventsRequest.resource_id_filter("[deviceID1/href1, deviceID2/href2]")`.

### Delete devices

The `DeleteDevices` command requests to delete entries of selected devices in the identity and events databases. The command supports filtering by setting `deviceIdFilter` value. A list of deleted device ids is returned in the response.

**Example usages of filter options:**

- to delete entries for devices owned by the user with ids `deviceID1` and `deviceID2` use `DeleteDevicesRequest.device_id_filter("[deviceID1, deviceID2]")`.
- to delete entries for all devices owned by the user, use `DeleteDevicesRequest` with empty `device_id_filter`.

### Cancel pending commands

Pending command of resource is identified by the `correlationId`. If a `correlationIdFilter` is not specified, all pending commands(Create, Get, Update, Delete) will be canceled.

### Cancel pending metadata updates

Pending metadata update is identified by `correlationId`. If a `correlationIdFilter` is not specified, all pending metadata updates(Set device twin enabled) will be canceled.

## Contracts

- [service](https://github.com/plgd-dev/hub/blob/main/grpc-gateway/pb/service.proto)
- [requests/responses](https://github.com/plgd-dev/hub/blob/main/grpc-gateway/pb/devices.proto)
- [get events request/response](https://github.com/plgd-dev/hub/blob/main/grpc-gateway/pb/events.proto)
- [client configuration](https://github.com/plgd-dev/hub/blob/main/grpc-gateway/pb/clientConfiguration.proto)
