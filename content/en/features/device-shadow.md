---
title: 'Device Shadow'
description: 'What is device shadow?'
date: '2021-05-13'
lastmod: '2021-07-01'
categories: [features]
keywords: [twin, shadow, cache, history]
menu:
  docs:
    parent: features
    weight: 50
toc: true
---

The device shadow represents represents the current state of each device's resource. Each connected device notifies the plgd Hub about every change using the CoAP Gateway observations, which are started right after the device successfully connects and authenticates. All changes are persisted in form of an audit log in the EventStore, from which is the latest version returned to clients through the Resource Directory.

## Operation overview

### Update a resource from CoAP Gateway

{{< plantuml id="update-device-shadow-from-hub" >}}
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

== Resource content changed - Update device shadow ==

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

{{< plantuml id="update-device-shadow" >}}
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

== Resource content changed - Update device shadow ==

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

## Disable Device Shadow feature

All changes that occur on the connected device are observed and stored in the EventStore. There are few use cases where Device Shadow - active observation of all changes is not desired. For example, if the device is in the maintenance state and produces test data that shall not be part of the audit log, Device Shadow should be for this device disabled. To do so, a client needs to send the request `UpdateDeviceMetadataRequest` with `ShadowSynchronization` and `CorrelationId`.

The Device Shadow feature is disabled only after the successful recipient of the `DeviceMetadataUpdated` event containing the same correlation id used in the Update request. This confirmation event is received only if the device is/comes online.

{{< plantuml id="update-shadow-synchronization" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

entity "OCF Server" as Server
participant "Gateway" as Gateway
participant "Resource Aggregate" as ResourceAggregate
control "Event Bus" as Bus
entity "Client" as Client

Client -> ResourceAggregate : UpdateDeviceMetadataRequest
activate Client
activate ResourceAggregate
activate Server
ResourceAggregate -> Client : UpdateDeviceMetadataResponse
ResourceAggregate --> Bus : DeviceMetadataUpdatePending
deactivate ResourceAggregate
Bus --> Gateway : DeviceMetadataUpdatePending
activate Gateway

Gateway -> ResourceAggregate : ConfirmDeviceMetadataUpdateRequest
activate ResourceAggregate
ResourceAggregate -> Gateway : ConfirmDeviceMetadataUpdateResponse
deactivate Gateway
ResourceAggregate --> Bus : DeviceMetadataUpdated
deactivate ResourceAggregate
Bus --> Client : DeviceMetadataUpdated
deactivate Client
deactivate Server
@enduml
{{< /plantuml >}}
