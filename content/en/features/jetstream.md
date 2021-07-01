---
title: 'JetStream'
description: 'What is device jetstream?'
date: '2021-07-01'
lastmod: '2021-07-01'
categories: [features]
keywords: [features, resource-aggregate, jetstream]
menu:
  docs:
    parent: features
    weight: 20
toc: true
---

# Jetstream

By default, the internal cloud services use nats as an event bus and event store for recovery, because nats don't persist messages. Jetstream(nats-jetstream) adds the ability to persist data in event-bus so your application can read all events in the cloud from the stream. Of course, sometimes the data was lost in-stream but for that grpc-gateway provides you API to get events from a certain time.

[`More information about jetstream`](https://docs.nats.io/jetstream).

## Subject of streams/nats

### Publisher

Resource-aggregate publish events to 3 type subject of subjects:
- `events.{deviceID}.resource-links.{eventType}` publishes resource-links events of types `resourcelinkspublished`, `resourcelinksunpublished`,
`resourcelinkssnapshottaken` for device `deviceID`
- `events.{deviceID}.metadata.{eventType}` publishes metadata events of types `devicemetadataupdatepending`, `devicemetadataupdated`,
`devicemetadatasnapshottaken` for device `deviceID`
- `events.{deviceID}.resources.{resourceID}.{eventType}` publishes resources events of types `resourcechanged`, `resourcecreated`,
`resourcecreatepending`, `resourcedeleted` `resourcedeletepending`, `resourceretrieved`, `resourceretrievepending`, `resourcestatesnapshottaken`, `resourceupdated`, `resourceupdatepending` for resource `resourceID`, which is calculated as `uuid.NewV5(uuid.NamespaceURL, deviceID+href)` for device `deviceID`.

Each event is compressed by [snappy](https://github.com/google/snappy) and wrapped by [event protobuf](https://github.com/plgd-dev/cloud/blob/v2/resource-aggregate/cqrs/eventbus/pb/eventbus.proto) in `Event.data` property and event type is described in `Event.event_type` property.

### Consumer

For consumer of events you can subscribe to:
- `events.>` gets all events of all devices
- `events.{deviceID}.>` gets all events of device `deviceID`
- `events.{deviceID}.resource-links.>` go get all resource links events of device `deviceID`
- `events.{deviceID}.resource-links.resourcelinkspublished` gets `resourcelinkspublished` event of device `deviceID`
- `events.*.resource-links.>` gets all resource links events of all devices
- `events.{deviceID}.metadata.>` gets all metadata events of device `deviceID`
- `events.{deviceID}.metadata.devicemetadataupdated` gets `devicemetadataupdated` event of device `deviceID`
- `events.*.metadata.>` to gets all metadata events of all devices
- `events.{deviceID}.resources.>` gets all resources events of device `deviceID`
- `events.{deviceID}.resources.{resourceID}.>` gets all events of resource `resourceID` for device `deviceID`
- `events.{deviceID}.resources.{resourceID}.resourcechanged` gets `resourcechanged` events of resource `resourceID` for device `deviceID`
- `events.{deviceID}.resources.*.resourcechanged` gets `resourcechanged` events of all resources for device `deviceID`
- `events.*.resources.*.resourcechanged` gets `resourcechanged` events of all resources for all devices

## Enable jetstream

At first, you need to set up nats-server 2.3+ to server jetstream.

### Deploy JetStream Controller for K8S
Creates NATS Server with JetStream enabled as a leaf node connection.

```bash
$ kubectl apply -f https://raw.githubusercontent.com/nats-io/k8s/master/nats-server/nats-js-leaf.yml
```

Now install the JetStream CRDs and Controller:
``` 
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
  subjects: ["events.*"]
```

### Enable jetstream at resource-aggregate

Set `clients.eventBus.nats.jetstream` to true value.
```yaml
...
clients:
  eventBus:
    nats:
      jetstream: true
...
```

### Enable jetstream at bundle
Set env variable `JETSTREAM=true` of bundle

```bash
docker run -it --rm -e JETSTREAM=true --network=host -v `pwd`/.tmp/data:/data plgd/bundle:v2next)
```

