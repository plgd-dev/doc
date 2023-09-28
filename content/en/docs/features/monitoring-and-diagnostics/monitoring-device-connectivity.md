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

In the context of managing CoAP gateways and their associated devices, the process of setting devices to an offline state involves a series of structured steps:

1. **Generation of Unique IDs**: Each CoAP gateway instance is initialized with a distinct, automatically generated identification (ID). These IDs serve a dual purpose – they are stored in a database for reference and are also linked to device metadata. This linkage facilitates streamlined updates regarding the online status of connected devices.

2. **Updating Record in the Database (DB)**: Periodically, each CoAP gateway dispatches a `UpdateServicesMetadata` request to update the Database via the resource-aggregate. This request includes the ID, time to live(TTL), and a timestamp indicating when the request was created. The request is processed into an event and stored in the database. If a CoAP gateway's time in the resource aggregate expires, the update operation fails, and the respective coap-gateway is terminated by sending a self-destruct signal (SIGTERM). During event processing, the resource aggregate calculates the `onlineValidUntil` based on the time to live and the duration the request has been in the system. While processing the request, other online services are also validated, and if they are no longer valid, they are transitioned to the offline state. The `onlineValidUntil` parameter is crucial for determining whether a service is online or offline. If `onlineValidUntil` lies in the future, the service is considered online; otherwise, it is marked as offline.

   {{< warning >}}

   The `UpdateServicesMetadata` function is exclusively designed for internal service usage. Consequently, this command does not rely on JWT for authorization. It is essential to note that this function should not be accessible from public endpoints. Instead, the sole method of authorization utilized is mutual TLS authentication between the CoAP gateway and the Resource Aggregate.

   {{< /warning >}}

3. **Processing ServicesMetadataUpdated**: Initially, the response containing the new validity information is forwarded to the CoAP gateway, which is then utilized to calculate the timing of the next update. In the event that any offline services are encompassed within the `ServicesMetadataUpdated` event, the resource-aggregate proceeds to iterate through all devices that are linked to these offline services. It triggers the `UpdateDeviceMetadata` operation for each device, commanding a transition to an OFFLINE state, effectively disassociating them from the respective service. Once all devices have undergone this update, and the `DeviceMetadataUpdated` event has been disseminated, the resource aggregate initiates the `ConfirmOfflineServices` procedure, which serves the purpose of purging offline services from the database.

4. **Iterate Until onlineValidUntil Expires**: The CoAP gateway continuously iterates through the update process until the `onlineValidUntil` parameter reaches its expiration. These updates are scheduled at intervals of one-third until the service's expiration deadline, denoted by `onlineValidUntil`. With each successful update, the `onlineValidUntil` parameter is refreshed with a new value. However, if the time surpasses the `onlineValidUntil` parameter, the service is deemed offline, so the CoAP gateway initiate a self-termination process (SIGTERM).

{{< plantuml id="update-services-metadata" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

participant CoAPGateway as "CoAP Gateway"
participant ResourceAggregate as "Resource Aggregate"
participant EventBus as "Event Bus"

CoAPGateway -> CoAPGateway++: Generate Unique ID
return Unique ID
CoAPGateway -> ResourceAggregate++: Send UpdateServicesMetadataRequest with\nTTL, ID, Timestamp
return UpdateServicesMetadataResponse with\nonlineValidUntil
activate CoAPGateway
loop every 1/3 until onlineValidUntil
   CoAPGateway -> ResourceAggregate++: Send UpdateServicesMetadataRequest with\nTTL, ID, Timestamp
   ResourceAggregate -> ResourceAggregate: Process ServicesMetadataUpdated with\nOnline and Offline services
   ResourceAggregate -> CoAPGateway: Send UpdateServicesMetadataResponse with\nonlineValidUntil
   deactivate CoAPGateway
   loop for each offline service
      loop for each device associated with the offline service
         ResourceAggregate -> ResourceAggregate: Send UpdateDeviceMetadataRequest with\nDevice ID, Connection.Status = OFFLINE
         ResourceAggregate -> EventBus: Publish DeviceMetadataUpdated with\nConnection.Status = OFFLINE, Service.Id = ""
      end
      ResourceAggregate -> ResourceAggregate: Confirm the offline service
   end
   deactivate ResourceAggregate
end

== Device Online ==

CoAPGateway -> ResourceAggregate++ : Send UpdateDeviceMetadataRequest with\nDevice ID, Connection.Status = ONLINE, Service.Id = "ID"
ResourceAggregate -> EventBus: Publish DeviceMetadataUpdated with\nConnection.Status = ONLINE, Service.Id = "ID"
return UpdateDeviceMetadataResponse

@enduml
{{< /plantuml >}}

### Example

This series of steps describes the process of expanding the setup, encountering an Out of Memory event, recording and analyzing gateway states, and identifying gateway termination within a Kubernetes environment using CoAP gateways.

1. **Initial Setup Expansion:**
   - Added two CoAP gateways to the Kubernetes (k8s) environment.
   - Configured a 1-minute time to live, so each CoAP gateway will update the database every minute.
   - Current time is `Wed Sep 27 2023 13:47:20 GMT+0000`.
   - In the database, there are two CoAP gateways in the online state:

     ```jsonc
     {
        "online": [
           {
              "id": "ID-0",
              "onlineValidUntil": "1695822490000000000" // Wed Sep 27 2023 13:48:10 GMT+0000
           },
           {
              "id": "ID-1",
              "onlineValidUntil": "1695822500000000000" // Wed Sep 27 2023 13:48:20 GMT+0000
           }
        ],
        "offline": []
     }
     ```

2. **OOM Event and Restart:**
   - One gateway instance (ID-1) experiences an Out of Memory (OOM) event and restarts.
   - Two CoAP gateway instances exist concurrently (ID-0 and ID-2), while ID-1 is dead but still in the online state.
   - Current time is `Wed Sep 27 2023 13:47:30 GMT+0000`.

     ```jsonc
     {
        "online": [
           {
              "id": "ID-0",
              "onlineValidUntil": "1695822490000000000" // Wed Sep 27 2023 13:48:10 GMT+0000
           },
           {
              "id": "ID-1", // Dead instance
              "onlineValidUntil": "1695822500000000000" // Wed Sep 27 2023 13:48:20 GMT+0000
           },
           {
              "id": "ID-2", // New instance
              "onlineValidUntil": "1695822510000000000" // Wed Sep 27 2023 13:48:30 GMT+0000
           }
        ],
        "offline": []
     }
     ```

3. **Update record by CoAP gateway (ID-0):**
   - After 20 seconds, the CoAP gateway (ID-0) updates its record.
   - Current time is `Wed Sep 27 2023 13:47:50 GMT+0000`.
   - CoAP gateway hasn't detected the termination of one gateway.
   - Update the record for ID-0.

    ```jsonc
     {
        "online": [
           {
              "id": "ID-0",
              "onlineValidUntil": "1695815330000000000" // Wed Sep 27 2023 11:48:50 GMT+0000
           },
           {
              "id": "ID-1", // Dead instance
              "onlineValidUntil": "1695822500000000000" // Wed Sep 27 2023 13:48:20 GMT+0000
           },
           {
              "id": "ID-2",
              "onlineValidUntil": "1695822510000000000" // Wed Sep 27 2023 13:48:30 GMT+0000
           }
        ],
        "offline": []
     }
     ```

4. **Update record by CoAP gateway (ID-2):**
   - After 21 seconds, the CoAP gateway (ID-2) updates its record.
   - Current time is `Wed Sep 27 2023 13:48:11 GMT+0000`.
   - CoAP gateway detects the termination of one gateway (ID-1).
   - The database is updated for ID-2, and ID-1 is moved to the offline state.

    ```jsonc
     {
        "online": [
           {
              "id": "ID-0",
              "onlineValidUntil": "1695815330000000000" // Wed Sep 27 2023 11:48:50 GMT+0000
           },
           {
              "id": "ID-2",
              "onlineValidUntil": "1695815351000000000" // Wed Sep 27 2023 13:49:11 GMT+0000
           }
        ],
        "offline": [
           {
              "id": "ID-1", // Dead instance
              "onlineValidUntil": "1695822500000000000" // Wed Sep 27 2023 13:48:20 GMT+0000
           }
        ]
     }
     ```

5. **Update all devices associated with ID-1:**
   - Update all devices associated with ID-1 to the offline state and send the event `DeviceMetadataUpdated` to the event bus.

6. **Confirm offline services:**
   - Confirm offline services and remove them from the database.

   ```jsonc
     {
         "online": [
           {
              "id": "ID-0",
              "onlineValidUntil": "1695815330000000000" // Wed Sep 27 2023 11:48:50 GMT+0000
           },
           {
              "id": "ID-2",
              "onlineValidUntil": "1695815351000000000" // Wed Sep 27 2023 13:49:11 GMT+0000
           }
        ],
        "offline": [],
     }
   ```
