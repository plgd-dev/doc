---
title: 'Monitoring Device Connectivity'
description: 'Learn how to determine the online or offline status of connected devices'
date: '2023-08-08'
categories: [features,iot-connectivity]
keywords: [service, coap-gateway, network-monitoring]
weight: 2
---

The CoAP gateway is responsible for the connection management including the up to date representation of the device status. This information is driven through the Resource Aggregate command - `UpdateDeviceMetadata`, that gets initiated when device connection status changes - device connects or disconnects from the CoAP Gateway. However, there are various scenarios to consider, such as unexpected termination of the CoAP Gateway, due to factors like node failure, power loss or others.

## Managing Service Heartbeat Updates

In the context of managing CoAP gateways and their associated devices, the process of setting devices to an offline state involves a series of structured steps:

1. **Generating Unique Service ID**: Each instance of a CoAP gateway initiates with a unique ID, generated during startup. These IDs are stored in a database for reference and are also associated with the metadata of each device.

2. **Sending Heartbeat**: Periodically, each CoAP gateway sends a heartbeat using the [UpdateServiceMetadata command](https://github.com/plgd-dev/hub/blob/main/resource-aggregate/pb/commands.proto#L668) to update the heartbeat entries managed by the Resource Aggregate. This request includes the service ID, [time to live (TTL)](https://github.com/plgd-dev/hub/blob/main/charts/plgd-hub/values.yaml#L887), and a timestamp indicating when the request was created. The request is transformed into an event after it's successfully stored in the event store. During command-to-event transformation, the Resource Aggregate calculates the `heartbeatValidUntil` based on the time to live and the duration of the request. If the latest heartbeats are no longer valid, they are set in the new event as expired.

   In case the heartbeat update came too late - the previous one already expired and it's present in the expired list, the update operation fails and the requesting CoAP Gateway is terminated by sending a self-destruct signal - SIGTERM.

   {{< warning >}}

   The `UpdateServiceMetadata` function is exclusively designed for internal service usage. Consequently, this command does not rely on JWT for authorization. It is essential to note that this function should not be accessible from public endpoints. Authentication using mutual TLS authentication is in place.

   {{< /warning >}}

3. **Updating Device Status**: As soon as the Resource Aggregate identifies expired CoAP Gateways and marks them as expired, the procedure to set devices previously connected to this Gateway offline starts. Devices whose connection was maintained by matching service ID are identified and their status is set to offline using the standard `UpdateDeviceMetadata` command. The audit context of this command contains a **nil-uuid** user ID. Once all devices have undergone this update, the Resource Aggregate initiates the `ConfirmOfflineServices` procedure, which purges offline gateways from the event store.

4. **Handling Response**: The response [ServicesMetadataUpdated](https://github.com/plgd-dev/hub/blob/main/resource-aggregate/pb/events.proto#L219) contains the validity of the heartbeat, which helps the CoAP Gateway to decide on when to send the next heartbeat. The next heartbeat is scheduled at intervals of one-third until the service's expiration deadline, denoted by returned `heartbeatValidUntil`.

{{< plantuml id="update-services-metadata" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

participant CoAPGateway as "CoAP Gateway"
participant ResourceAggregate as "Resource Aggregate"
participant EventBus as "Event Bus"

CoAPGateway -> CoAPGateway++: Generate Unique ID
return Unique ID
CoAPGateway -> ResourceAggregate++: Send UpdateServiceMetadataRequest with\nTTL, ID, Timestamp
return UpdateServiceMetadataResponse with\nheartbeatValidUntil
activate CoAPGateway
loop every 1/3 until heartbeatValidUntil
   CoAPGateway -> ResourceAggregate++: Send UpdateServiceMetadataRequest with\nTTL, Service ID, Timestamp
   ResourceAggregate -> ResourceAggregate: Process ServicesMetadataUpdated with\nValid and Expired service's heartbeats
   ResourceAggregate -> CoAPGateway: Send UpdateServiceMetadataResponse with\nheartbeatValidUntil
   deactivate CoAPGateway
   loop for each expired service's heartbeat
      loop for each device associated with the offline service
         ResourceAggregate -> ResourceAggregate: Send UpdateDeviceMetadataRequest with\nDevice ID, Connection.Status = OFFLINE
         ResourceAggregate -> EventBus: Publish DeviceMetadataUpdated with\nConnection.Status = OFFLINE, Connection.ServiceId = "", AuditContext.UserId = "00000000-0000-0000-0000-000000000000"
      end
      ResourceAggregate -> ResourceAggregate: Confirm the offline service
   end
   deactivate ResourceAggregate
end

== Device Online ==

CoAPGateway -> ResourceAggregate++ : Send UpdateDeviceMetadataRequest with\nDevice ID, Connection.Status = ONLINE, ServiceId = "Service ID"
ResourceAggregate -> EventBus: Publish DeviceMetadataUpdated with\nConnection.Status = ONLINE, Connection.ServiceId = "Service ID"
return UpdateDeviceMetadataResponse

@enduml
{{< /plantuml >}}

### Example

This series of steps describes the process of expanding the setup, encountering an Out of Memory event, recording and analyzing gateway states, and identifying gateway termination within a Kubernetes environment using CoAP gateways and the Resource Aggregate.

1. **Initial Sequence:**
   - Added two CoAP Gateways to the Kubernetes (k8s) environment.
   - Configured a 1-minute time to live, so each CoAP Gateway will update its metadata every minute.
   - The current time is `Wed Sep 27 2023 13:47:20 GMT+0000`.
   - In the Event Store, there are two CoAP Gateways with valid heartbeats:

     ```jsonc
     {
        "valid": [
           {
              "id": "ID-0",
              "heartbeatValidUntil": "1695822490000000000" // Wed Sep 27 2023 13:48:10 GMT+0000
           },
           {
              "id": "ID-1",
              "heartbeatValidUntil": "1695822500000000000" // Wed Sep 27 2023 13:48:20 GMT+0000
           }
        ],
        "expired": []
     }
     ```

2. **OOM Event and Restart:**
   - One gateway instance (ID-1) experiences an Out of Memory (OOM) event and restarts.
   - Two CoAP Gateway instances exist concurrently (ID-0 and ID-2), while ID-1 is dead but the heartbeat is still valid.
   - The current time is `Wed Sep 27 2023 13:47:30 GMT+0000`.

     ```jsonc
     {
        "valid": [
           {
              "id": "ID-0",
              "heartbeatValidUntil": "1695822490000000000" // Wed Sep 27 2023 13:48:10 GMT+0000
           },
           {
              "id": "ID-1", // Dead instance
              "heartbeatValidUntil": "1695822500000000000" // Wed Sep 27 2023 13:48:20 GMT+0000
           },
           {
              "id": "ID-2", // New instance
              "heartbeatValidUntil": "1695822510000000000" // Wed Sep 27 2023 13:48:30 GMT+0000
           }
        ],
        "expired": []
     }
     ```

3. **CoAP Gateway (ID-0) sends heartbeat:**
   - After 40 seconds, the CoAP Gateway (ID-0) sends heartbeat 2 times.
   - The current time during the second heartbeat is `Wed Sep 27 2023 13:47:50 GMT+0000`.
   - Resource Aggregate hasn't detected the termination of one gateway.
   - Update the record for ID-0.

    ```jsonc
     {
        "valid": [
           {
              "id": "ID-0",
              "heartbeatValidUntil": "1695815330000000000" // Wed Sep 27 2023 11:48:50 GMT+0000
           },
           {
              "id": "ID-1", // Dead instance
              "heartbeatValidUntil": "1695822500000000000" // Wed Sep 27 2023 13:48:20 GMT+0000
           },
           {
              "id": "ID-2",
              "heartbeatValidUntil": "1695822510000000000" // Wed Sep 27 2023 13:48:30 GMT+0000
           }
        ],
        "expired": []
     }
     ```

4. **CoAP Gateway (ID-2) sends heartbeat:**
   - After 61 seconds, the CoAP Gateway (ID-2) sends heartbeat 2 times.
   - The current time during the second heartbeat is `Wed Sep 27 2023 13:48:11 GMT+0000`.
   - Resource Aggregate detects the termination of one gateway (ID-1).
   - The Event Store is updated for ID-2, and ID-1 is marked as expired.

    ```jsonc
     {
        "valid": [
           {
              "id": "ID-0",
              "heartbeatValidUntil": "1695815330000000000" // Wed Sep 27 2023 11:48:50 GMT+0000
           },
           {
              "id": "ID-2",
              "heartbeatValidUntil": "1695815351000000000" // Wed Sep 27 2023 13:49:11 GMT+0000
           }
        ],
        "expired": [
           {
              "id": "ID-1", // Dead instance
              "heartbeatValidUntil": "1695822500000000000" // Wed Sep 27 2023 13:48:20 GMT+0000
           }
        ]
     }
     ```

5. **Update status of all devices associated with ID-1:**
   - The Resource Aggregate updates all devices associated with ID-1 to the offline state and sends the `DeviceMetadataUpdated` event to the Event Bus.

6. **Confirm offline service:**
   - The Resource Aggregate confirms the offline services and removes them from the Event Store.

   ```jsonc
     {
         "valid": [
           {
              "id": "ID-0",
              "heartbeatValidUntil": "1695815330000000000" // Wed Sep 27 2023 11:48:50 GMT+0000
           },
           {
              "id": "ID-2",
              "heartbeatValidUntil": "1695815351000000000" // Wed Sep 27 2023 13:49:11 GMT+0000
           }
        ],
        "expired": [],
     }
   ```
