---
title: 'Component Overview'
description: 'L3 and L4 plgd overview'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [architecture, C4 model, components]
keywords: [c4 model, diagrams]
weight: 31
---

## CoAP Gateway

The CoAP gateway acts as a CoAP Client, communicating with IoT devices, which serve as CoAP Servers following the [OCF specification](https://openconnectivity.org/developer/specifications/). As the component diagram describes, the responsibilities of the gateway are:

- Handle and maintain TCP connections coming from devices
- [Authenticate and authorize requests (see 5.5.5)](https://openconnectivity.org/specs/OCF_Device_To_Cloud_Services_Specification_v2.2.1.pdf#page=15) from the device in conjunction with an OAuth2.0 Server
- Process device CRUDN operations which are by their nature forwarded to the [Resource Aggregate](#resource-aggregate) or [Resource Directory](#resource-directory)

![L3](../static/component-coapgateway.svg "medium-zoom-image")

### Operational flow

Before a device becomes operational and is able to interact with other devices, it needs to be appropriately onboarded. The first step in onboarding the device is to [configure ownership (see 5.3.3)](https://openconnectivity.org/specs/OCF_Security_Specification_v2.2.1.pdf#page=38) where the legitimate user who owns/purchases the device uses one of the Owner Transfer Methods (OTMs) within the Onboarding tool to establish ownership. Once ownership is established, the OBT [provisions the device (see 5.3.4)](https://openconnectivity.org/specs/OCF_Security_Specification_v2.2.1.pdf#page=39), after which the device can be [provisioned for the plgd hub (see 8.1.2.3)](https://openconnectivity.org/specs/OCF_Device_To_Cloud_Services_Specification_v2.2.1.pdf#page=32). After successful provisioning, the device should [establish a TLS connection (see 7.2)](https://openconnectivity.org/specs/OCF_Cloud_Security_Specification_v2.2.1.pdf#page=14) using certificate based credentials.

{{< note >}}
Use [plgd OCF Client](https://github.com/plgd-dev/device) or [Apple](https://apps.apple.com/us/app/plgd/id1536315811) / [Android](https://play.google.com/store/apps/details?id=dev.plgd.client) mobile app for **easy** device discovery, ownership configuration and provisioning for the plgd hub!
{{< /note >}}

#### Device Onboarding

{{< plantuml id="device-onboarding" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

participant D as "Device"
participant CGW as "CoAP Gateway"
participant IS as "Identity Store"
participant OBT as "Onboarding Tool"

OBT -->[: Discover devices
|||
D --> OBT: Here I am
group OCF Onboarding
    OBT -> D ++: Establish Device Owner
    return Ownership established
    OBT -> D ++: Provisioning security configuration
    return Provisioning successful
end
group OCF Cloud Provisioning
    OBT -> D ++: Provisioning cloud configuration resource
    return Provisioning successful
end
D -> CGW ++: Establish TCP connection
@enduml
{{< /plantuml >}}

The TCP connection which the device established to the CoAP Gateway is now authenticated, but not authorized. In order for the device to be reachable, the TCP connection must be authorized. This flow describes the operation of a new device; within the first connection the device must Sign Up - [register with the plgd hub (see 8.1.4)](https://openconnectivity.org/specs/OCF_Device_To_Cloud_Services_Specification_v2.2.1.pdf#page=33). The [authorization code](https://tools.ietf.org/html/rfc6749#section-4.1) received during the OCF Cloud Provisioning process described in the diagram above is exchanged by the CoAP Gateway for an access and refresh token and returned to the device. This process is described in more detail in the [OCF Cloud Security Specification (see 6.2)](https://openconnectivity.org/specs/OCF_Cloud_Security_Specification_v2.2.1.pdf#page=12).

#### Hub Registration

{{< plantuml id="hub-registration" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

participant D as "Device"
participant CGW as "CoAP Gateway"
participant O as "OAuth 2.0 Server"
participant IS as "Identity Store"

D -> CGW ++: Sign Up
group OAuth2.0 Authorization Code Grant Flow
    CGW -> O ++: Verify and exchange authorization code for JWT access token
    return Ok\n(JWT Access Token, Refresh Token, ...)
end
CGW -> IS ++: Register and assign device to user
return Registered
return Signed up\n(JWT Access Token, Refresh Token, ...)
@enduml
{{< /plantuml >}}

Successful registration to plgd.dev is followed by an authorization request called Sign In. Sign In is required right after a successfully established TCP connection to the CoAP Gateway, otherwise the device won’t be reachable – marked as online. Other device requests are blocked as well unless the device successfully Signs In. Successful authorization precedes validation of the [JWT Access Token](https://tools.ietf.org/html/rfc6749#section-1.4).

{{< warning >}}
Only JWT access tokens are supported on the device.
{{< /warning >}}

#### Device Authorization

{{< plantuml id="device-authorization" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

participant D as "Device"
participant CGW as "CoAP Gateway"
participant IS as "Identity Store"
participant EB as "Event Bus"
participant RA as "Resource Aggregate"

D -> CGW ++: Sign In
CGW -> CGW: Validate JWT Access Token
CGW -> IS ++: Is device registered?
return Ok
CGW -> EB: Subscribe to device & owner events
CGW -> RA ++: Declare device as online
return
return Signed In

@enduml
{{< /plantuml >}}

Device capabilities are represented in the form of resources. Configuration in terms of whether the resource is published (remotely accessible over plgd hub) or not, is handled by the [IoTivity-Lite API](https://github.com/iotivity/iotivity-lite/blob/master/include/oc_cloud.h#L128). Whether the resource is published is up to the device vendor, who might want to have some resources accessible only on the proximity network. Resource information which is published to the plgd hub provides insights into device capabilities. Clients are interested not only in the resource href - location on which they can request resource representation – but mainly in the resource type, as this allows them to filter only capabilities they are able to control.
As an example, if you have a client application which controls lighting, it will search the Resource Directory for all lights the user has at home – filtering resources by resource type `oic.r.switch.binary`. Other resources, with data like temperature, moisture, etc. are not of any interest, as the application doesn't understand their representation.
Information which is published doesn't contain resource representation, only resource information as described [here (see 6.1.3.2.2)](https://openconnectivity.org/specs/OCF_Device_To_Cloud_Services_Specification_v2.2.0.pdf#page=21).

```json
{
   "di":"e61c3e6b-9c54-4b81-8ce5-f9039c1d04d9",
   "links":[
      {
         "anchor":"ocf://e61c3e6b-9c54-4b81-8ce5-f9039c1d04d9",
         "href":"/myLightSwitch",
         "rt":[
            "oic.r.switch.binary"
         ],
         "if":[
            "oic.if.a",
            "oic.if.baseline"
         ],
         "p":{
            "bm":3
         },
         "eps":[
            {
               "ep":"coaps://[fe80::b1d6]:1111",
               "pri":2
            },
            {
               "ep":"coaps://[fe80::b1d6]:1122"
            }
         ]
      }
   ],
   "ttl":600476
}
```

A resource publish request is forwarded to the [Resource Aggregate](#resource-aggregate), which registers a new resource. This process makes the resource discoverable.
The plgd hub starts observation of **every successfully published resource** by sending the [OBSERVE request](https://tools.ietf.org/html/rfc7641#section-1.2). Each of the received notifications from the device is sent to the [Resource Aggregate](#resource-aggregate) to record the change.

As the response to the resource observation request contains actual [representation](https://tools.ietf.org/html/rfc7641#section-1.1), the CoAP Gateway doesn't have to pull the data at all. Additional responses called [notifications](https://tools.ietf.org/html/rfc7641#section-3.2) are sent by the device whenever the representation of the device changes.

#### Resource Publish & Subscription

{{< plantuml id="resource-publish" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

participant D as "Device"
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"

D -> CGW ++: Publish Resources
CGW -> RA ++: Publish Resources
return
CGW -> D: Observe published resource
activate D
D --> CGW: Resource representation
CGW -> RA ++: Update resource representation
return
|||
[-> D: Actuate
activate D
D --> CGW: Resource representation
deactivate D
CGW -> RA ++: Update resource representation
return
@enduml
{{< /plantuml >}}

From this moment on, the device is reachable to all authorized clients and devices. Resource update requests received by a particular Gateway to which the client is connected are forwarded to the [Resource Aggregate](#resource-aggregate). Successful command validation precedes storing and publishing of this event to the [Event Bus](#event-bus) to which the CoAP Gateway is subscribed. If the update request event targets the device hosted by this instance of the CoAP Gateway, an [UPDATE](https://tools.ietf.org/html/rfc7252#section-5.8.2) is forwarded over the authorized TCP channel to the device. The device response is forwarded to the [Resource Aggregate](#resource-aggregate), which issues a resource updated event, updating the resource projection and informing the client that the update was successful.

#### Resource Update

{{< plantuml id="resource-update" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

participant D as "Device"
participant CGW as "CoAP Gateway"
participant GGW as "gRPC Gateway"
participant RA as "Resource Aggregate"
participant EB as "Event Bus"
participant C as "Mobile App"

C -> GGW ++: Update device/light resource
activate C
GGW -> RA ++: Update device/light resource
RA --> EB: Publish ResourceUpdatePending event
return
EB --> CGW: ResourceUpdatePending event
activate CGW
CGW -> D ++: Update /light resource
return Update successful
CGW -> RA ++: Update device/light successful
RA --> EB: Publish ResourceUpdated event
return
deactivate CGW
EB --> GGW: ResourceUpdated event
return Updated

@enduml
{{< /plantuml >}}

{{< note >}}
A client requesting resource observation will immediately start to receive notifications without additional requests to the device over the CoAP Gateway. As the plgd hub is by default observing the resources of all connected devices, the Gateway responsible will just subscribe to the [Event Bus](#event-bus) and forward requested notifications. **Handling of CRUDN operations is the same for every Gateway.**
{{< /note >}}

#### Delete a device

At some point, a user might want to delete a device from the plgd hub. There are two ways to achieve this.

##### Disconnect the device from the plgd hub using Onboarding Tool

Through Onboarding Tool, a device is requested to disconnect from the plgd hub and from the local network. There are three steps:

1. Send the SignOff message, which deregisters the device
2. Close the TCP connection
3. Cleanup of the cloud configuration resource

##### Force disconnect by the plgd hub

A user might decide to delete the device directly using the plgd API. This approach is handy for stale devices, which cannot any longer be requested to disconnect through the Onboarding Tool; thus enabling a clean up of the list of devices in the plgd hub.

{{< plantuml id="device-delete" >}}
@startuml
skinparam backgroundColor transparent
hide footbox

participant C as "Client"
participant GGW as "gRPC Gateway"
participant RA as "Resource Aggregate"
participant IS as "Identity Store"
participant EB as "Event Bus"
participant CGW as "CoAP Gateway"
participant D as "Device"

C -> GGW ++: DeleteDevicesRequest
GGW -> RA ++: DeleteDevicesRequest
return DeleteDevicesResponse
GGW -> IS ++: DeleteDevicesRequest
return DeleteDevicesResponse
IS --> EB: DevicesDeleted
return DeleteDevicesResponse
EB --> CGW: DevicesDeleted
CGW -> D: Disconnect
destroy D

D -> CGW ++: Sign In
CGW -> CGW: Validate JWT Access Token
CGW -> IS ++: Is device registered?
return Not registered
note right
  Revoke Refresh Token
end note
return Unauthorized
CGW -> D: Disconnect
destroy D

D -> D: Cleanup cloud configuration
@enduml
{{< /plantuml >}}

## gRPC Gateway

![L3](../static/component-grpcgateway.svg "medium-zoom-image")

## HTTP Gateway

![L3](../static/component-httpgateway.svg "medium-zoom-image")

## Resource Aggregate

Every transaction on the device's resource is scoped to the single [aggregate](https://martinfowler.com/bliki/DDD_Aggregate.html) – the Resource Aggregate. The RA builds its internal state, which is a projection of a single fine-grained event stream. When the aggregate receives a new command from any of the plgd gateways, the command is validated and after successful validation an event describing the action is created and persisted in the [EventStore](#event-store). After successful write to the [EventStore](#event-store), the event is published to the [EventBus](#event-bus).

>To prevent conflicts during the write to EventStore, the [Optimistic concurrency control](https://en.wikipedia.org/wiki/Optimistic_concurrency_control) method is used.

### Commands and Events Overview

{{< plantuml id="commands-events-overview" >}}
@startuml
skinparam backgroundColor transparent

left to right direction
skinparam usecase {
    BackgroundColor<< Command >> LightBlue
    BorderColor<< Command >> LightBlue
    BackgroundColor<< Event >> LightRed
    BorderColor<< Event >> DarkRed
}

usecase (Resource\nAggregate) as Aggregate

(PublishResourceLinks) << Command >>
(UnpublishResourceLinks) << Command >>
(RetrieveResource) << Command >>
(ConfirmResourceRetrieve) << Command >>
(CreateResource) << Command >>
(ConfirmResourceCreate) << Command >>
(UpdateResource) << Command >>
(ConfirmResourceUpdate) << Command >>
(DeleteResource) << Command >>
(ConfirmResourceDelete) << Command >>
(NotifyResourceChanged) << Command >>
(CancelPendingCommands) << Command >>
(CancelPendingMetadataUpdates) << Command >>

(ResourceLinksPublished) << Event >>
(ResourceLinksUnpublished) << Event >>
(ResourceLinksSnapshotTaken) << Event >>
(ResourceRetrievePending) << Event >>
(ResourceRetrieved) << Event >>
(ResourceCreatePending) << Event >>
(ResourceCreated) << Event >>
(ResourceUpdatePending) << Event >>
(ResourceUpdated) << Event >>
(ResourceDeletePending) << Event >>
(ResourceDeleted) << Event >>
(ResourceChanged) << Event >>
(ResourceStateSnapshotTaken) << Event >>
(DeviceMetadataUpdatePending) << Event >>
(DeviceMetadataUpdated) << Event >>
(DeviceMetadataSnapshotTaken) << Event >>

(PublishResourceLinks) -down-> Aggregate
(UnpublishResourceLinks) -down-> Aggregate
(RetrieveResource) -down-> Aggregate
(ConfirmResourceRetrieve) -down-> Aggregate
(CreateResource) -down-> Aggregate
(ConfirmResourceCreate) -down-> Aggregate
(UpdateResource) -down-> Aggregate
(ConfirmResourceUpdate) -down-> Aggregate
(DeleteResource) -down-> Aggregate
(ConfirmResourceDelete) -down-> Aggregate
(NotifyResourceChanged) -down-> Aggregate
(CancelPendingCommands) -down-> Aggregate
(CancelPendingMetadataUpdates) -down-> Aggregate

Aggregate -down-> (ResourceLinksPublished)
Aggregate -down-> (ResourceLinksUnpublished)
Aggregate -down-> (ResourceLinksSnapshotTaken)
Aggregate -down-> (ResourceRetrievePending)
Aggregate -down-> (ResourceRetrieved)
Aggregate -down-> (ResourceCreatePending)
Aggregate -down-> (ResourceCreated)
Aggregate -down-> (ResourceUpdatePending)
Aggregate -down-> (ResourceUpdated)
Aggregate -down-> (ResourceDeletePending)
Aggregate -down-> (ResourceDeleted)
Aggregate -down-> (ResourceChanged)
Aggregate -down-> (ResourceStateSnapshotTaken)
Aggregate -down-> (DeviceMetadataUpdatePending)
Aggregate -down-> (DeviceMetadataUpdated)
Aggregate -down-> (DeviceMetadataSnapshotTaken)

@enduml
{{< /plantuml >}}

{{< note >}}
More detailed flows on which commands trigger which events can be found in the [commands proto file](https://github.com/plgd-dev/hub/blob/main/resource-aggregate/pb/commands.proto).
{{< /note >}}

![L3](../static/component-resourceaggregate.svg "medium-zoom-image")

## Resource Directory

![L3](../static/component-resourcedirectory.svg "medium-zoom-image")

## Device

![L3](../static/component-device.svg "medium-zoom-image")

## Client

WIP
[8.1.2.2 OCF Cloud User Authorization of Mediator](https://openconnectivity.org/specs/OCF_Device_To_Cloud_Services_Specification_v2.2.1.pdf#page=31)

![L3](../static/component-clients.svg "medium-zoom-image")

## Event Bus

After every successful write to the [EventStore](#event-store), an event is published on the event bus. Other plgd services are subscribed to the event bus to be notified when a change in the system / on the devices occurs or is requested. One such party is the resource directory, which aggregates resource models in memory for fast retrieve on request by the plgd gateways. Gateways are subscribed also, to enable synchronous processing of devices’ resource updates.

The plgd hub uses the [NATS](https://nats.io) messaging system as its event bus.

> We use pure NATS, not NATS Streaming nor Jetstream.

## Event Store

The plgd hub persists each resource state mutation as a separate record called an event. This enables us to reconstruct past states, use events as a foundation to understand user behaviour, or even explore a tamper-proof audit log. The plgd hub adopts [Event Sourcing](https://docs.microsoft.com/en-us/azure/architecture/patterns/event-sourcing) and [CQRS](https://docs.microsoft.com/en-us/azure/architecture/patterns/cqrs) patterns to support flexibility, scalability and trackability of the system.

Accepted commands are represented as PendingChange events; resource state changes received from connected devices are represented as ResourceChanged events. All these events are persisted in the EventStore, with the possibility to define your own cleanup policies to remove old events in case they are no longer needed.
