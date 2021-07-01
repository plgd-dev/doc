---
title: 'JetStream'
description: 'How to persist plgd events in the JetStream'
date: '2021-07-01'
lastmod: '2021-07-01'
categories: [features]
keywords: ['persistance', 'message log', 'nats']
menu:
  docs:
    parent: features
    weight: 60
toc: true
---

By default, plgd cloud services use NATS as an EventBus and MongoDB as an EventStore. Some use-cases require subscription directly to the internal messaging system instead of communicating with the plgd using it's gateways. To simplify the data reconciliation and scale consumers easier, plgd supports [JetStream
(https://github.com/nats-io/jetstream) technology as an alternative EventBus. JetStream is built on top of NATS, persisting all published events. Using JetStream as an EventBus allows you to access older, not yet processed messages without accessing the EventStore.

{{ % note % }}
There are still some edge-cases when the plgd event couldn't be published to the JetStream but it was stored to the EventStore. In such a case you need to identify that one event was lost and if needed, retrieve it using plgd gRPC Gateway.
{{ % /note % }}

{{ % note % }}
More information about the JetStream can be found [here](https://docs.nats.io/jetstream).
{{ % /note % }}


## NATS subjects overview
- `events.{deviceID}.resource-links.{eventType}` publishes resource-links events of types `resourcelinkspublished`, `resourcelinksunpublished`,
`resourcelinkssnapshottaken` for device `deviceID`
- `events.{deviceID}.metadata.{eventType}` publishes metadata events of types `devicemetadataupdatepending`, `devicemetadataupdated`,
`devicemetadatasnapshottaken` for device `deviceID`
- `events.{deviceID}.resources.{resourceID}.{eventType}` publishes resources events of types `resourcechanged`, `resourcecreated`,
`resourcecreatepending`, `resourcedeleted` `resourcedeletepending`, `resourceretrieved`, `resourceretrievepending`, `resourcestatesnapshottaken`, `resourceupdated`, `resourceupdatepending` for resource `resourceID`, which is calculated as `uuid.NewV5(uuid.NamespaceURL, deviceID+href)` for device `deviceID`.

Each event is compressed by [snappy](https://github.com/google/snappy) and encoded in protobuf [event envelope](https://github.com/plgd-dev/cloud/blob/v2/resource-aggregate/cqrs/eventbus/pb/eventbus.proto). The event envelope consist of `Event.data` containing the event and `Event.event_type` describing type of the event.

### Consumer subscriptions options
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

## Enable JetStream
{{ % note % }}
Deployment of the JetStream as an EventBus will be controlled by a single configuration option available in the plgd HELM chart. This is currently WIP. 
{{ % /note % }}

{{ % warning % }}
It's required from you to create event streams before the JetStream can be used as the plgd EventBus. If streams are not created, plgd services won't work.
{{ % /warning % }}

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

### Enable jetstream at plgd #bundle
Set env variable `JETSTREAM=true` of bundle

```bash
docker run -it --rm -e JETSTREAM=true --network=host -v `pwd`/.tmp/data:/data plgd/bundle:v2next)
```

