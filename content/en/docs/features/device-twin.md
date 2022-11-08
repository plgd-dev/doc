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

### How is the twin kept up to date?

#### Update resource from CoAP Gateway

The device resources can be updated by more services/users at the same time. These resource change requests are called [Pending Commands](/docs/features/pending-command/). As soon as the command is successfully processed on the device, the CoAP Gateway is informed about the change through our observation channel. More details in the following diagram.

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

#### Update resource from OCF Client

The device might be actuated also from the local network. The temperature in the room changed, or a user updated a resource over device-to-device connectivity, for example using the [plgd client](https://github.com/plgd-dev/client-application). This change also triggers the notify on the device, what delivers to the CoAP Gateway, subscribed device observer, information about the change what updates the twin in the plgd hub. More details in the following diagram.

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

### Device Metadata and Twin State

Device metadata contains additional information about the device lifecycle, including connection status and twin synchronization state. The twin synchronization state informs user about the reconciliation after the reconnect or twin creation after the first sign-in to the plgd hub. There are more states and various edge cases, so let's go through diagrams to understand all the state transitions during the device lifecycle.

#### Device successfully signs-in

{{< plantuml id="device-metadata-connect" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity Device as "Device"
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"
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
return Signed In
deactivate Device

@enduml
{{< /plantuml >}}

#### Twin synchronization starts

{{< note >}}
The synchronization as it is described in the following sequence diagram occurs only if the batch observe is supported by the device. By default, the plgd hub requires [support for the batch observe](https://github.com/plgd-dev/hub/blob/main/coap-gateway/config.yaml#L20). Devices which doesn't support it are disconnected. If you're interested how twin synchronization state transition works for devices without batch observe support, jump [here](#twin-synchronization---batch-not-supported).
{{< /note >}}

{{< plantuml id="device-metadata-twin-synchronization-batch" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity Device as "Device"
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"
participant EB as "Event Bus"

Device -> CGW ++: Publish resources
activate Device
CGW -> Device: Published resources
deactivate Device
CGW -> RA++: UpdateDeviceMetadataRequest with\nTwinSynchronization.State = SYNCING
RA -> EB: DeviceMetadataUpdated with\nTwinSynchronization.State = SYNCING
RA -> CGW
deactivate RA
CGW -> Device++: Batch observe
Device -> CGW: Current representation of resources
CGW -> RA++: BatchNotifyResourceChangedRequest
  RA --> EB: ResourceChanged event for all changed resources
return BatchNotifyResourceChangedResponse
CGW -> RA++: UpdateDeviceMetadataRequest with\nTwinSynchronization.State = IN_SYNC
RA -> EB: DeviceMetadataUpdated with\nTwinSynchronization.State = IN_SYNC
RA -> CGW: UpdateDeviceMetadataResponse
deactivate RA
note over Device, CGW
Device notifies CoAP Gateway in case of resource change.
For more info go to [[#how-is-the-twin-kept-up-to-date How is the twin kept up to date?]]
end note

@enduml
{{< /plantuml >}}

#### Device disconnects

{{< plantuml id="device-metadata-disconnect" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity Device as "Device"
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"
participant EB as "Event Bus"

Device ->x CGW++: Close connection
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

#### Twin Synchronization - batch not supported

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

#### Disable Twin

All changes that occur on the connected device are observed and stored in the EventStore. There are few use cases when the Twin - active observation of all changes is not desired. For example, if the device is in the maintenance state and produces test data that shall not be part of the audit log. To do so, a client needs to send the request `UpdateDeviceMetadataRequest` with `TwinEnabled` and `CorrelationId`.

The Twin is disabled only after the `DeviceMetadataUpdated` event is received and contains the same correlation id as was used in the update request. **This confirmation event is received only if the device is/comes online.**

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

#### Enable Twin

Same logic as for disable Twin applies. The Twin is enabled only after the `DeviceMetadataUpdated` event is received and contains the same correlation id as was used in the update request. **This confirmation event is received only if the device is/comes online.**

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
