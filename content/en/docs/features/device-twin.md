---
title: 'Device Twin'
description: 'What is device twin?'
date: '2021-05-13'
lastmod: '2022-10-28'
categories: [features]
keywords: [twin, twin, cache, history]
menu:
  docs:
    parent: features
    weight: 50
toc: true
---

The device twin represents the current state of each device's resource. Each connected device notifies the plgd hub about every change using the CoAP Gateway observations, which are started right after the device successfully connects and authenticates. All changes are persisted in form of an audit log in the EventStore, from which is the latest version returned to clients through the Resource Directory.

## Operation overview

### Update a resource from CoAP Gateway

{{< plantuml id="update-device-twin-from-hub" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity "OCF Server" as Server
participant "Gateway" as Gateway
participant "Resource Aggregate" as ResourceAggregate
control "Event Bus" as Bus
entity "Client" as Client

Client -> ResourceAggregate : UpdateResourceRequest
activate Client
activate ResourceAggregate
ResourceAggregate -> Client : UpdateResourceResponse
ResourceAggregate --> Bus : ResourceUpdatePending
deactivate ResourceAggregate
Bus --> Gateway : ResourceUpdatePending
activate Gateway
Gateway -> Server: [UPDATE] 'oic.r.temperature'
activate Server
Server -> Gateway : OK
deactivate Server
Gateway -> ResourceAggregate : ConfirmResourceUpdateRequest
activate ResourceAggregate
ResourceAggregate -> Gateway : ConfirmResourceUpdateResponse
deactivate Gateway
ResourceAggregate --> Bus : ResourceUpdated
deactivate ResourceAggregate
Bus --> Client : ResourceUpdated
deactivate Client

== Resource content changed - Update device twin ==

Server -> Gateway : [NOTIFY] 'oic.r.temperature' changed
activate Gateway
Gateway -> ResourceAggregate : NotifyResourceChangedRequest
activate ResourceAggregate
ResourceAggregate -> Gateway : NotifyResourceChangedResponse
deactivate Gateway
ResourceAggregate --> Bus : ResourceChanged
deactivate ResourceAggregate

Server -> Gateway : [NOTIFY] 'oic.r.temperature' changed
@enduml
{{< /plantuml >}}

### Update a resource from OCF Client

{{< plantuml id="update-device-twin" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity "OCF Client" as Client
entity "OCF Server" as Server
participant "Gateway" as Gateway
participant "Resource Aggregate" as ResourceAggregate
control "Event Bus" as Bus

Client -> Server : [UPDATE] 'oic.r.temperature'
Server -> Client : OK

== Resource content changed - Update device twin ==

Server -> Gateway : [NOTIFY] 'oic.r.temperature' changed
activate Gateway
Gateway -> ResourceAggregate : NotifyResourceChangedRequest
activate ResourceAggregate
ResourceAggregate -> Gateway : NotifyResourceChangedResponse
deactivate Gateway
ResourceAggregate --> Bus : ResourceChanged
deactivate ResourceAggregate

Server -> Gateway : [NOTIFY] 'oic.r.temperature' changed
@enduml
{{< /plantuml >}}

### Update Device metadata

Device metadata contains status about the connection and twin synchronization. It is updated to online with expiration when the device comes online, and twin synchronization is set to out of sync state. Device is responsible for signing in again before expiration, otherwise the connection is closed, and the device metadata is updated to offline. Whenever the device closes the connection, the device metadata is updated to offline.

#### Device Twin Synchronization

Describes how is synchronization of device twin handled. There are two options whether device supports batch observations or not.

##### Batch observation is supported

{{< plantuml id="device-metadata-twin-synchronization-batch" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity Device as "Device"
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"
participant RD as "Resource Directory"
participant EB as "Event Bus"

alt TwinEnabled == true
  CGW -> RA++: UpdateDeviceMetadataRequest with\nTwinSynchronization.State = SYNCING
  RA -> EB: DeviceMetadataUpdated with\nTwinSynchronization.State = SYNCING
  RA -> CGW
  deactivate RA
  CGW -> Device++: Batch observe of all discoverable resources
  Device -> CGW: first notification from observe with current representation of all device resources
  CGW -> RA++: BatchNotifyResourceChangedRequest
  return BatchNotifyResourceChangedResponse
  CGW -> RA++: UpdateDeviceMetadataRequest with\nTwinSynchronization.State = IN_SYNC
  RA -> EB: DeviceMetadataUpdated with\nTwinSynchronization.State = IN_SYNC
  RA -> CGW: UpdateDeviceMetadataResponse
  deactivate RA
  deactivate Device
end
deactivate CGW

@enduml
{{< /plantuml >}}

##### Batch observation is not supported

{{< plantuml id="device-metadata-twin-synchronization" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity Device as "Device"
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"
participant RD as "Resource Directory"
participant EB as "Event Bus"

CGW -> Device++: Get device resources links to check if batch observation is supported
activate CGW
return resources links
alt TwinEnabled == true
  CGW -> RD++: Get all device published resources from the HUB
  return Published resources
  CGW -> RA++: UpdateDeviceMetadataRequest with\nTwinSynchronization.State = SYNCING
  RA -> EB: DeviceMetadataUpdated with\nTwinSynchronization.State = SYNCING
  RA->CGW:
  deactivate RA
  loop for each new published resource
      CGW -> Device++: Observe resource
      Device -> CGW: first notification from observe with current representation of the resource
      CGW -> RA++: NotifyResourceChangedRequest
      return NotifyResourceChangedResponse
  end
  CGW -> RA++: UpdateDeviceMetadataRequest with\nTwinSynchronization.State = IN_SYNC
  RA -> EB: DeviceMetadataUpdated with\nTwinSynchronization.State = IN_SYNC
  RA->CGW: UpdateDeviceMetadataResponse
  deactivate RA
  deactivate Device
end
deactivate CGW

@enduml
{{< /plantuml >}}

#### Connect Device

When device is connected and successfully signed in, the hub starts synchronize device twin if it is enabled.

{{< plantuml id="device-metadata-connect" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity Device as "Device"
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"
participant RD as "Resource Directory"
participant EB as "Event Bus"

Device -> CGW ++: Sign In
activate Device
CGW -> RA++: UpdateDeviceMetadataRequest with\nConnected.Status == ONLINE
alt DeviceMetadataUpdated.TwinEnabled == true
  RA -> EB: DeviceMetadataUpdated with\nConnected.Status == ONLINE and TwinSynchronization.State = OUT_OF_SYNC
else
  RA -> EB: DeviceMetadataUpdated with\nConnected.Status == ONLINE and TwinSynchronization.State = DISABLED
end
RA->CGW:
deactivate RA
CGW -> Device: Signed In
deactivate Device
CGW -> Device++: Get device resources links to check if batch observation is supported
activate CGW
return Device resources links
ref over CGW, RA, EB, Device :  Device Twin Synchronization
deactivate CGW

@enduml
{{< /plantuml >}}

#### Disconnect Device

{{< plantuml id="device-metadata-disconnect" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity Device as "Device"
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"
participant EB as "Event Bus"

Device -> CGW++: Connection has been closed
CGW -> RA++: UpdateDeviceMetadataRequest with\nConnection.Status == OFFLINE
alt DeviceMetadataUpdated.TwinEnabled == true
  RA -> EB: DeviceMetadataUpdated with\nConnected.Status == OFFLINE and TwinSynchronization.State = OUT_OF_SYNC
else
  RA -> EB: DeviceMetadataUpdated with\nConnected.Status == OFFLINE and TwinSynchronization.State = DISABLED
end
RA->CGW: UpdateDeviceMetadataResponse
deactivate RA
deactivate CGW

@enduml
{{< /plantuml >}}

#### Publish resources

When device publish resources and batch observation is not supported, the hub starts synchronize device twin if it is enabled.

{{< plantuml id="device-metadata-publish-resources" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity Device as "Device"
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"
participant RD as "Resource Directory"
participant EB as "Event Bus"

Device -> CGW ++: Publish resources
activate Device
CGW -> Device: Published resources
deactivate Device
ref over CGW, RA, EB, Device : Device Twin Synchronization - Batch observation is not supported

@enduml
{{< /plantuml >}}

#### Disable Device Twin

All changes that occur on the connected device are observed and stored in the EventStore. There are few use cases where Device Twin - active observation of all changes is not desired. For example, if the device is in the maintenance state and produces test data that shall not be part of the audit log, Device Twin should be for this device disabled. To do so, a client needs to send the request `UpdateDeviceMetadataRequest` with `TwinEnabled` and `CorrelationId`.

The Device Twin feature is disabled only after the successful recipient of the `DeviceMetadataUpdated` event containing the same correlation id used in the Update request. This confirmation event is received only if the device is/comes online.

{{< plantuml id="device-metadata-disable-twin" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity Device as Device
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"
control EB as "Event Bus"
entity Client as Client

Client -> RA : UpdateDeviceMetadataRequest \nwith\nTwinEnabled == false, correlationId == "abc"
activate Client
activate RA
activate Device
RA -> Client: UpdateDeviceMetadataResponse
RA --> EB: DeviceMetadataUpdatePending with\nTwinEnabled == false, correlationId == "abc"
deactivate RA
EB --> CGW : DeviceMetadataUpdatePending with\nTwinEnabled == false, correlationId == "abc"
activate CGW
CGW -> Device: Close all observations
Device -> CGW: Observe closed
CGW -> RA: ConfirmDeviceMetadataUpdateRequest with\nTwinEnabled == false, correlationId == "abc"
activate RA
RA --> EB: DeviceMetadataUpdated with\nTwinEnabled = false, TwinSynchronization.State = DISABLED, correlationId == "abc"
deactivate RA
RA -> CGW: ConfirmDeviceMetadataUpdateResponse
deactivate CGW
EB --> Client: DeviceMetadataUpdated with\nTwinEnabled = false, TwinSynchronization.State = DISABLED, correlationId == "abc"
deactivate Client
deactivate Device
@enduml
{{< /plantuml >}}

#### Enable Device Twin

All changes that occur on the connected device are observed and stored in the EventStore. To enable device twin a client needs to send the request `UpdateDeviceMetadataRequest` with `TwinEnabled` and `CorrelationId`.

The Device Twin feature is enabled only after the successful recipient of the `DeviceMetadataUpdated` event containing the same correlation id used in the Update request. This confirmation event is received only if the device is/comes online.

{{< plantuml id="device-metadata-enabled-twin" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity Device as Device
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"
control EB as "Event Bus"
entity "Client" as Client

Client -> RA : UpdateDeviceMetadataRequest with\nTwinEnabled = true, correlationId = "abc"
activate Client
activate RA
activate Device
RA -> Client : UpdateDeviceMetadataResponse
RA --> EB : DeviceMetadataUpdatePending with\nTwinEnabled = true, correlationId = "abc"
deactivate RA
EB --> CGW : DeviceMetadataUpdatePending with\nTwinEnabled = true, correlationId = "abc"
activate CGW
CGW -> RA : ConfirmDeviceMetadataUpdateRequest with\nTwinEnabled = true, correlationId = "abc"
activate RA
RA --> EB : DeviceMetadataUpdated with\nTwinEnabled = true, TwinSynchronization.State = OUT_OF_SYNC, correlationId = "abc"
deactivate RA
EB --> Client : DeviceMetadataUpdated with\nTwinEnabled = true, TwinSynchronization.State = OUT_OF_SYNC, correlationId == "abc"
deactivate Client
RA -> CGW : ConfirmDeviceMetadataUpdateResponse
CGW -> Device: Get device resources links\nto check if batch observation is supported
Device->CGW: Device resources links
ref over CGW, RA, EB, Device: Device Twin Synchronization
deactivate CGW
deactivate Device
@enduml
{{< /plantuml >}}
