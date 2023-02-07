---
title: 'JetStream'
description: 'How to persist plgd events in the JetStream'
date: '2021-07-01'
lastmod: '2021-07-01'
categories: [features]
keywords: ['persistence', 'message log', 'nats']
menu:
  docs:
    parent: features
    weight: 60
toc: true
---

By default, plgd hub services use NATS as an EventBus and MongoDB as an EventStore. Some use-cases require subscription directly to the internal messaging system instead of communicating with the plgd using its gateways. To simplify the data reconciliation and scale consumers easier, plgd supports [JetStream](https://github.com/nats-io/jetstream) technology as an alternative EventBus. JetStream is built on top of NATS, persisting all published events. Using JetStream as an EventBus allows you to access older, not yet processed messages without accessing the EventStore.

{{< note >}}
There are still some edge-cases when the plgd event couldn't be published to the JetStream but it was stored to the EventStore. In such a case you need to identify that one event was lost and if needed, retrieve it using plgd gRPC Gateway.
{{< /note >}}

{{< note >}}
More information about the JetStream can be found [here](https://docs.nats.io/jetstream).
{{< /note >}}

## NATS subjects overview

### Definitions

- `ownerID` is the owner of the device. It is calculated as `uuid.NewV5(uuid.NamespaceURL, value of JWT ownerClaim)`.
- `deviceID` is the UUID of the device.
- `resourceID` is the unique identifier of resource over the whole hub. It is calculated as `uuid.NewV5(uuid.NamespaceURL, deviceID+href)`, where the `href` is a resource path. (eg "/oic/d").
- `resourceHrefID` is the identifier of resource href. It is calculated as `uuid.NewV5(uuid.NamespaceURL, resourceHref)`. For multiple hrefs of the same resource, the same `resourceHrefID` is used.
- `resourceTypeID` is the identifier of the resource type. It is calculated as `uuid.NewV5(uuid.NamespaceURL, resourceType)`. For multiple resource types, the same `resourceTypeID` is used.

### Device events

Each event is compressed by [snappy](https://github.com/google/snappy) and encoded in protobuf [devices event envelope](https://github.com/plgd-dev/hub/blob/main/resource-aggregate/cqrs/eventbus/pb/eventbus.proto). The event envelope consist of `Event.data` containing the event and `Event.event_type` describing the type of the event.

#### Resource links

- `plgd.owners.{ownerID}.devices.{deviceID}.resource-links.{eventType}` publishes resource-links events of types `resourcelinkspublished`, `resourcelinksunpublished`, `resourcelinkssnapshottaken` for device `deviceID` and `ownerID`

#### Metadata events

- `plgd.owners.{ownerID}.devices.{deviceID}.metadata.{eventType}` publishes metadata events of types `devicemetadataupdatepending`,`devicemetadataupdated`, `devicemetadatasnapshottaken` for device `deviceID` and `ownerID`

#### Resources events

The resources events are published for each resource of the device. The event is published multiple times to different subjects:

- once for resource ID
- once for resource href
- once for each resource type of the resource

##### Resources events by resource ID

- `plgd.owners.{ownerID}.devices.{deviceID}.resources.{resourceID}.{eventType}` publishes resources events of types `resourcechanged`, `resourcecreated`, `resourcecreatepending`, `resourcedeleted` `resourcedeletepending`, `resourceretrieved`, `resourceretrievepending`, `resourcestatesnapshottaken`, `resourceupdated`, `resourceupdatepending` for resource `resourceID`, `deviceID` and `ownerID`.

##### Resources events by href

- `plgd.owners.{ownerID}.devices.{deviceID}.resourceHrefs.{resourceHrefID}.{eventType}` publishes resources events of types `resourcechanged`, `resourcecreated`, `resourcecreatepending`, `resourcedeleted` `resourcedeletepending`, `resourceretrieved`, `resourceretrievepending`, `resourcestatesnapshottaken`, `resourceupdated`, `resourceupdatepending` for resource `resourceHrefID`, `deviceID` and `ownerID`.

##### Resources events by resource type

- `plgd.owners.{ownerID}.devices.{deviceID}.resourceTypes.{resourceTypeID}.{eventType}` publishes resources events of types `resourcechanged`, `resourcecreated`, `resourcecreatepending`, `resourcedeleted` `resourcedeletepending`, `resourceretrieved`, `resourceretrievepending`, `resourcestatesnapshottaken`, `resourceupdated`, `resourceupdatepending` for resource `resourceTypeID`, `deviceID` and `ownerID`.

### Owner events

Each event is encoded in protobuf [event envelope](https://github.com/plgd-dev/hub/blob/main/authorization/pb/events.proto) and then compressed by [snappy](https://github.com/google/snappy).

#### Registration events

- `plgd.owners.{ownerID}.registrations.{eventType}` publishes owner events of types `devicesregistered`,`devicesunregistered` for `ownerID`.

### Consumer subscriptions options

For the consumers of events you can subscribe to:

- `plgd.owners.>` gets all events of hub
- `plgd.owners.{ownerId}.>` gets all events of owner `ownerId`
- `plgd.owners.*.devices.{deviceID}.>` gets all events of device `deviceID`
- `plgd.owners.*.devices.{deviceID}.resource-links.>` go get all resource links events of device `deviceID`
- `plgd.owners.*.devices.{deviceID}.resource-links.resourcelinkspublished` gets `resourcelinkspublished` event of device `deviceID`
- `plgd.owners.*.devices.*.resource-links.>` gets all resource links events of all devices
- `plgd.owners.{ownerId}.devices.*.resource-links.>`  gets all resource links events of all devices of owner `ownerId`
- `plgd.owners.*.devices.{deviceID}.metadata.>` gets all metadata events of device `deviceID`
- `plgd.owners.*.devices.{deviceID}.metadata.devicemetadataupdated` gets `devicemetadataupdated` event of device `deviceID`
- `plgd.owners.*.devices.*.metadata.>` to gets all metadata events of all devices
- `plgd.owners.{ownerId}.devices.*.metadata.>` to gets all metadata events of all devices of owner `ownerId`
- `plgd.owners.*.devices.{deviceID}.resources.>` gets all resources events of device `deviceID`
- `plgd.owners.*.devices.{deviceID}.resources.{resourceID}.>` gets all events of resource `resourceID` for device `deviceID`
- `plgd.owners.*.devices.{deviceID}.resources.{resourceID}.resourcechanged` gets `resourcechanged` events of resource `resourceID` for device `deviceID`
- `plgd.owners.*.devices.{deviceID}.resources.*.resourcechanged` gets `resourcechanged` events of all resources for device `deviceID`
- `plgd.owners.*.devices.*.resources.*.resourcechanged` gets `resourcechanged` events of all resources for all devices
- `plgd.owners.{ownerId}.devices.*.resources.*.resourcechanged` gets `resourcechanged` events of all resources for all devices of owner `ownerId`

#### Subscribe by resource href

- `plgd.owners.*.devices.{deviceID}.resourceHrefs.>` gets all resources events of device `deviceID`
- `plgd.owners.*.devices.{deviceID}.resourceHrefs.{resourceHrefID}.>` gets all events of resource `resourceID` for device `deviceID`
- `plgd.owners.*.devices.{deviceID}.resourceHrefs.{resourceHrefID}.resourcechanged` gets `resourcechanged` events of resource `resourceHrefID` for device `deviceID`
- `plgd.owners.*.devices.{deviceID}.resourceHrefs.*.resourcechanged` gets `resourcechanged` events of all resources for device `deviceID`
- `plgd.owners.*.devices.*.resourceHrefs.{resourceHrefID}.>` gets all events of resources with `resourceHrefID` for all devices
- `plgd.owners.*.devices.*.resourceHrefs.*.resourcechanged` gets `resourcechanged` events of all resources for all devices
- `plgd.owners.{ownerId}.devices.*.resourceHrefs.*.resourcechanged` gets `resourcechanged` events of all resources for all devices of owner `ownerId`

#### Subscribe by resource type

- `plgd.owners.*.devices.{deviceID}.resourceTypes.{resourceTypeID}.>` gets all events of resource with resource type `resourceTypeID` for device `deviceID`
- `plgd.owners.*.devices.{deviceID}.resourceTypes.{resourceTypeID}.resourcechanged` gets `resourcechanged` events of all resources with resource type `resourceTypeID`  for device `deviceID`
- `plgd.owners.*.devices.*.resourceTypes.{resourceTypeID}.>` gets all events of resource with resource type `resourceTypeID` for all devices
- `plgd.owners.{ownerId}.devices.*.resourceTypes.{resourceTypeID}.>` gets all events of resource with resource type `resourceTypeID` for all devices of owner `ownerId`

{{< warning >}}
This subjects can producer duplications events, because resource can contain multiple resource types. It's up to you to filter duplications.

- `plgd.owners.{ownerID or *}.devices.{deviceID or *}.resourceTypes.>`
- `plgd.owners.{ownerID or *}.devices.{deviceID or *}.resourceTypes.*.{eventType}>`

## Enable JetStream

{{< note >}}
Deployment of the JetStream as an EventBus will be controlled by a single configuration option available in the plgd HELM chart. This is currently WIP.

{{< warning >}}
It's required from you to create event streams before the JetStream can be used as the plgd EventBus. If streams are not created, plgd services won't work.

### Enable jetstream at plgd #bundle

Set env variable `JETSTREAM=true` of bundle

```bash
docker run -it --rm -e JETSTREAM=true --network=host -v `pwd`/.tmp/data:/data plgd/bundle:vnext)
```

### Enable jetstream manually

{{< warning >}}
Required [nats-server 2.3+](https://github.com/nats-io/nats-server/releases/latest)
Required [nats client](https://github.com/nats-io/natscli/releases/latest)
{{< /warning >}}

#### Enable jetstream at nats-server

Append jetstream configuration to `nats.config` of nats-server:

```jsonc
...
jetstream: {
  store_dir: "$JETSTREAM_PATH"
  // 1GB of memory
  max_memory_store: 1073741824

  // 10GB of memory
  max_file_store: 10737418240
}
```

{{< note >}}
More [information](https://docs.nats.io/nats-server/configuration) about nats-server configuration.
{{< /note >}}

and start it:

```bash
nats-server -c nats.config
```

#### Setup and create stream

Setup events stream `stream.json` where all events of hub will be stored:

```jsonc
{
  "name": "EVENTS", // A name for the Stream that may not have spaces, tabs, period (.), greater than (>), or asterisk (*).
  "subjects": [ // A list of subjects to consume, supports wildcards.
    "plgd.>"
  ],
  "retention": "limits",  // How message retention is considered, limits (default), interest or workQueue.
  "max_consumers": -1, // How many Consumers can be defined for a given Stream, -1 for unlimited.
  "max_msgs_per_subject": 0, // / How many messages may be in a subject of Stream, -1 for unlimited.
  "max_msgs": -1,  // How many messages may be in a Stream. Adheres to Discard Policy, removing oldest or refusing new messages if the Stream exceeds this size, -1 for unlimited.
  "max_bytes": -1, // How many bytes the Stream may contain. Adheres to Discard Policy, removing oldest or refusing new messages if the Stream exceeds this number of messages, -1 for unlimited.
  "max_age": 0,  // Maximum age of any message in the Stream, expressed in nanoseconds, -1 for unlimited.
  "max_msg_size": -1, // The largest message that will be accepted by the Stream. -1 for unlimited
  "storage": "file", // The type of storage backend, file and memory
  "discard": "old", // discard old messages when stream is full
  "num_replicas": 1, // How many replicas to keep for each message in a clustered JetStream, maximum 5
  "duplicate_window": 600000000000 // The window within which to track duplicate messages, expressed in nanoseconds.
}
```

{{< note >}}
More [information](https://docs.nats.io/jetstream/concepts/streams) about stream configuration.
{{< /note >}}

And then apply the configuration to the nats-server via:

```bash
nats str add EVENTS --config /configs/jetstream.json
```

### Deploy JetStream Controller for K8S

Creates NATS Server with JetStream enabled as a leaf node connection.

```bash
kubectl apply -f https://raw.githubusercontent.com/nats-io/k8s/master/nats-server/nats-js-leaf.yml
```

Now install the JetStream CRDs and Controller:

```bash
helm repo add nats https://nats-io.github.io/k8s/helm/charts/
helm install nats nats/nats --set nats.image=synadia/nats-server:nightly --set=nats.jetstream.enabled=true
helm install nack nats/nack --set=jetstream.nats.url=nats://nats:4222
```

Create one events stream

```yaml
---
apiVersion: jetstream.nats.io/v1beta1
kind: Stream
metadata:
  name: events
spec:
  name: events
  subjects: ["plgd.>"]
```

### Enable JetStream at Resource Aggregate

Set `clients.eventBus.nats.jetstream` to true value.

```yaml
...
clients:
  eventBus:
    nats:
      jetstream: true
...
```
