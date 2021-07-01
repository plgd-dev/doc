---
title: 'Device shadow'
description: 'What is device shadow?'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [features]
keywords: [features, device-shadow, resource-shadow]
menu:
  docs:
    parent: features
    weight: 10
toc: true
---


# Device shadow

The device shadow mirrors contents of published resources by the device to the cloud through CoAP Gateway. CoAP Gateway starts observations of all published resources so the device is responsible to send notifications about change with the full content of the resource. The notification is stored in the Event Store by resource aggregate.

## Projection

Contents of resources are projected by resource-directory.

## Update a resource from CoAP Gateway

{{< plantuml id="update-device-shadow-from-cloud" >}}
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

## Update a resource from OCF Client

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

## Disable/Enable device shadow

When the client wants to disable/enable the device shadow, for eg maintenance reasons, it needs to send the request `UpdateDeviceMetadataRequest` with `ShadowSynchronization` and `CorrelationId`. The disable/enable device shadows occur only when the device is connected. For confirmation, the client needs to wait for the event `DeviceMetadataUpdated`, where `CorrelationId` will be the same as was sent in `UpdateDeviceMetadataRequest`.


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
