---
title: 'Component Overview'
description: 'L3 and L4 plgd overview'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [architecture, C4 model, components]
keywords: [c4 model, diagrams]
menu:
  docs:
    parent: architecture
    weight: 20
toc: true
---

## CoAP Gateway

The CoAP gateway acts act as a CoAP Client, communicating with IoT devices - CoAP Servers following the [OCF specification](https://openconnectivity.org/developer/specifications/). As the component diagram describes, responsibilities of the gateway are:

- handle and maintain TCP connections coming from devices
- forward [authentication and authorization requests (see 5.5.5)](https://openconnectivity.org/specs/OCF_Device_To_Cloud_Services_Specification_v2.2.1.pdf#page=15)  to the Authorization Service
- process device CRUDN operations which are by its nature forwarded to the [Resource Aggregate](#resource-aggregate) or [Resource Directory](#resource-directory)

![L3](/images/diagrams/component-coapgateway.svg "medium-zoom-image")

### Operational flow

Before a device becomes operational and is able to interact with other devices, it needs to be appropriately onboarded. The first step in onboarding the device is to [configure the ownership (see 5.3.3)](https://openconnectivity.org/specs/OCF_Security_Specification_v2.2.1.pdf#page=38) where the legitimate user that owns/purchases the device uses an Onboarding tool (OBT) and using the OBT uses one of the Owner Transfer Methods (OTMs) to establish ownership. Once ownership is established, the OBT [provisions the device (see 5.3.4)](https://openconnectivity.org/specs/OCF_Security_Specification_v2.2.1.pdf#page=39), at the end of which the device can be [provisioned for the plgd.cloud (see 8.1.2.3)](https://openconnectivity.org/specs/OCF_Device_To_Cloud_Services_Specification_v2.2.1.pdf#page=32). After successful provisioning, the device should [establish the TLS connection (see 7.2)](https://openconnectivity.org/specs/OCF_Cloud_Security_Specification_v2.2.1.pdf#page=14) using the certificate based credentials.

{{% note %}}
Use [plgd.ocfclient](https://github.com/plgd-dev/sdk) or sample [Apple](https://apps.apple.com/us/app/plgd/id1536315811) / [Android](https://play.google.com/store/apps/details?id=dev.plgd.client) mobile app for **easy** device discovery, ownership configuration and provisioning for the plgd.cloud!
{{% /note %}}

#### Device Onboarding

{{< plantuml id="device-onboarding" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

participant D as "Device"
participant CGW as "CoAP Gateway"
participant AS as "Authorization Server"
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

TCP connection which device established to the CoAP Gateway is now authenticated but not authorized. Before the devices becomes reachable, TCP connection needs to be authorized. As this flow describes operation of a new device, device needs within the first connection Sign Up - [register with the plgd.cloud (see 8.1.4)](https://openconnectivity.org/specs/OCF_Device_To_Cloud_Services_Specification_v2.2.1.pdf#page=33). The [authorization code](https://tools.ietf.org/html/rfc6749#section-4.1) received during the OCF Cloud Provisioning process described in the diagram above is be by the CoAP Gateway exchanged for an access and refresh token and returned back to the device. This process is in more detail described in the [OCF Cloud Security Specification (see 6.2)](https://openconnectivity.org/specs/OCF_Cloud_Security_Specification_v2.2.1.pdf#page=12).

#### Cloud Registration

{{< plantuml id="cloud-registration" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

participant D as "Device"
participant CGW as "CoAP Gateway"
participant O as "OAuth 2.0 Server"
participant AS as "Authorization Server"

D -> CGW ++: Sign Up
group OAuth2.0 Authorization Code Grant Flow
    CGW -> O ++: Verify and exchange authorization code for JWT access token
    return Ok\n(JWT Access Token, Refresh Token, ...)
end
CGW -> AS ++: Register and assign device to user
return Registered
return Signed up\n(JWT Access Token, Refresh Token, ...)
@enduml
{{< /plantuml >}}

Successful registration to the plgd.dev is followed by authorization request called Sign In. Sign In is required right after successfully established TCP connection to the CoAP Gateway, otherwise the device won't be reachable - marked as online. Other device requests are blocked as well unless the device successfully Signs In. Successful autorization precedes validation of the [JWT Access Token](https://tools.ietf.org/html/rfc6749#section-1.4).

{{% warning %}}
Only JWT access tokens are supported on the device.
{{% /warning %}}

#### Device Authorization

{{< plantuml id="device-authorization" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

participant D as "Device"
participant CGW as "CoAP Gateway"
participant RA as "Resource Aggregate"

D -> CGW ++: Sign In
CGW -> CGW: Validate JWT Access Token
CGW -> RA ++: Declare device as online
return
return Signed In

@enduml
{{< /plantuml >}}

Device capabilities are represented in form of resources. Configuration if the resource is published (remotely accessible over plgd.cloud) or not is part of the [IoTivity-Lite API](https://github.com/iotivity/iotivity-lite/blob/master/include/oc_cloud.h#L128). If the resource is publised or not is up to the device vendor which might want to have some resources accessible only on the proximity network. Resources information which are published to the plgd.cloud provides insights into device capabilities. Clients are interested not only in the resource href - location on which they can request resource representation, but mainly in the resource type as this allows them to filter only capabilities they are able to control.
As an example, if you have an client application which controls the light, it will search the Resource Directory for all lights user have at home - filter resources by resource type `oic.r.switch.binary`. Other resources like temperature, moisture, etc. are not of any interest, as application doesn't understand their representation.
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

Resource publish request is forwarded to the [Resource Aggregate](#resource-aggregate) which registers a new resource. This process makes the resource discoverable.
The plgd.cloud starts observation of **every successfully published resource** by sending the [OBSERVE request](https://tools.ietf.org/html/rfc7641#section-1.2). Each of the received notification from the device is send to the [Resource Aggregate](#resource-aggregate) to record the change.

As the response to the resource observation request contains actual [representation](https://tools.ietf.org/html/rfc7641#section-1.1), CoAP Gateway doesn't have to pull the data at all. Additional responses called [notifications](https://tools.ietf.org/html/rfc7641#section-3.2) are by the device send whenever the representation of the device changes.

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
MD013
From this moment on, device is reachable to all authorized clients and devices. Resource update requests received by particular Gateway where the client is connected are forwarded to the [Resource Aggregate](#resource-aggregate). Successful command validation precede storing and publishing of this event to the [Event Bus](#event-bus), to which is the CoAP Gateway subscribed. If the update request event targets the device hosted by this instance of the CoAP Gateway, [UPDATE](https://tools.ietf.org/html/rfc7252#section-5.8.2) is forwarded over the authorized TCP channel to the device. Device response is forwarded to the [Resource Aggregate](#resource-aggregate) which issues resource updated event updating the resource projection and informing client that the update was successful.

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

{{% note %}}
Client requesting resource observation will immediately start to receive notifications without additional request to the device over CoAP Gateway. As the plgd.cloud is by default observing resources of all connected devices, responsible Gateway will just subscribe to the [Event Bus](#event-bus) and forward requested notifications. **Handling of CRUDN operations is same for every Gateway.**
{{% /note %}}

## gRPC Gateway

![L3](/images/diagrams/component-grpcgateway.svg "medium-zoom-image")

## HTTP Gateway

![L3](/images/diagrams/component-httpgateway.svg "medium-zoom-image")

## Resource Aggregate

Every transaction on the device's resource is scoped to the single [aggregate](https://martinfowler.com/bliki/DDD_Aggregate.html) - Resource Aggregate. The RA builds it's internal state, which is a projection of a single fine-grained event stream. When the aggregate receives a new command from any of the plgd gateway, the command is validated and after successful validation event describing the action is created and persisted in the [EventStore](#event-store). After successful write to the [EventStore](#event-store), event is published to the [EventBus](#event-bus).

> To prevent the conflicts during the write to EventStore, [Optimistic concurrency control](https://en.wikipedia.org/wiki/Optimistic_concurrency_control) method is used

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

{{% note %}}
More detailed flows which command triggers which event can be find directly in the [commands proto file](https://github.com/plgd-dev/cloud/blob/v2/resource-aggregate/pb/commands.proto).
{{% /note %}}

![L3](/images/diagrams/component-resourceaggregate.svg "medium-zoom-image")

## Resource Directory

![L3](/images/diagrams/component-resourcedirectory.svg "medium-zoom-image")

## Device

![L3](/images/diagrams/component-device.svg "medium-zoom-image")

## Client

WIP
[8.1.2.2 OCF Cloud User Authorization of Mediator](https://openconnectivity.org/specs/OCF_Device_To_Cloud_Services_Specification_v2.2.1.pdf#page=31)

![L3](/images/diagrams/component-clients.svg "medium-zoom-image")

## Event Bus

After every successful write to the [EventStore](#event-store), an event is published on the event bus. Other plgd services are subscribed to the event bus to be notified when the change in the system / on the devices occurs or is requested. One such party is a resource directory, which aggregates resource models in memory for fast retrieve requested by the plgd gateways. Gateways are subscribed as well to be able to synchronously process device's resource updates.

plgd cloud uses [NATS](https://nats.io) messaging system as it's event bus.

> We are using pure NATS, not NATS Streaming nor Jetstream.

## Event Store

plgd cloud persist events in an event store, which is a database of events. The store has an API for adding and retrieving device's events. Events needs to be versioned and written in a correct order. To achieve the consistency, optimistic concurrency control method is applied during each write.
After the event is successfully written into the event store, it's distributed to the event bus which notifies all subscribers about the change asynchronously.

The plgd cloud defines an [EventStore](https://github.com/plgd-dev/cloud/blob/v2/resource-aggregate/cqrs/eventstore/eventstore.go#L23) interface what allows integration of different technologies to store the events. During the last 2 years the project evaluated multiple technologies for both EventStore and EventBus:

- CockroachDB
- Apache Kafka
- MongoDB
- NATS Jetstream
- Google Firestore
- Amazon Kinesis

{{% note %}}
Currently supported and preffered solution is MongoDB. Read further to know more about the database scheme and how we are Event Sourcing!
{{% /note %}}

### MongoDB

Device's data are in the MongoDB organized per devices. For each connected device a new collection is created. Each event is modeled as a new document.
{{% warning %}}
This model is now being evaluated by the MondoDB team and is likely to be improved in Q1-2021.
{{% /warning %}}

#### Schema Overview

![L3](/images/diagrams/mongodb-schema.png "medium-zoom-image")

#### Event Organization

![L3](/images/diagrams/mongodb-eventsourcing.png "medium-zoom-image")

#### Queries

- Details Query resources B of device d9dd7...

1. Get latest snapshot
    a. Find document `where _id == B.s`
    b. Get `version` of latest snapshot event
2. Find documents `where aggregateID == B && version >= latestSnapshot.version`

- Details Query all resources of device d9dd7...

1. Get latest snapshots of all resources
    a. Find documents `where aggregateID == snapshot && version == -1`
    b. Get `versions` of latest snapshot events per each resource
2. Find all documents **per each resource** `where aggregateID == snapshot.aggregateID && version >= latestSnapshot.version`
