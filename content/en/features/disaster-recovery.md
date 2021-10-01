---
title: 'Disaster Recovery'
description: 'Getting back online and in-sync'
date: '2021-06-16'
lastmod: '2021-06-16'
categories: [features]
keywords: ['disaster recovery', 'data reconciliation' , 'jetstream']
menu:
  docs:
    parent: features
    weight: 50
toc: true
---

Resiliency is understood as a way to readapt to a "crisis situation", which applies to both infrastructure and the data. What it means varies from user to user. After service disruptions, some users are okay retrieving the latest resource value while others require all the changes which occurred during the time their application or internal messaging system was down. Another failure scenario is when the node hosting the plgd CoAP Gateway crashes. Thousands of devices will try to reconnect and keep the shadows up to date, which creates a huge load and rapidly slows down the system if not handled properly.

plgd Cloud offers to users various techniques on how to approach disaster recovery in case an error or system failure was detected. Let's have a look at them.

## Event Data Loss

{{% note %}}
plgd Cloud is an event-driven system, implemented using [CQRS](https://docs.microsoft.com/en-us/azure/architecture/patterns/cqrs) and [EventSourcing](https://docs.microsoft.com/en-us/azure/architecture/patterns/event-sourcing) design patterns. Each event that occurs in the system, _e.g. when the content of resource changes, when a new resource is published, or when a new device is registered with the plgd cloud,_ is stored in the EventStore and published to the EventBus.

plgd Gateways are subscribed to the EventBus to notify you through the gRPC stream or WebSockets about requested changes. Using plgd Gateways is a straightforward option how to communicate with the system, which can provide you both: the current value of the resource or a set of events representing changes of that particular resource (audit log).
{{% /note %}}

An active subscription to plgd events, using gRPC, WebSockets, NATS, or JetStream might fail. Same as the publish operation executed by the plgd service. There are multiple reasons which we cannot prevent - infrastructure failure when the node went down, service crash, or operator failure during the roll-out. To be able to get your system back to the in-sync state, failure has to be detected and the data reconciliation process started.

### Missing Events Detection

Sometimes it might be obvious that some events might be lost. If your service consuming plgd events restarted due to a crash, network outage, or infrastructure failure, there is a high chance that you missed some events. In such a scenario, the service shall start right away it's up and running with the data reconciliation process.

Another, not so obvious event data loss might occur due to EventBus service disruptions or during a very short network outage. Your service wasn't restarted; your messaging client just missed one event.
If your service is subscribed to events and requires the processing of all events in the correct order - skipping one event is not accepted, your service needs to track the version of each [event](https://github.com/plgd-dev/cloud/blob/v2/resource-aggregate/pb/events.proto). In case the **received event's version isn't incremented by one compared to your latest event**, your service shall start the resource reconciliation process.

### Data Reconciliation using gRPC

#### Interest in the latest resource content only

The plgd gRPC Gateway exposes `GetEvents` RPC call to retrieve the latest version of the resource. In case you are not interested in all the changes which occurred during your outage, this is the right way how to get in sync. Additionally, to optimize this operation, timestamp ETag can be specified. Just take the latest event you have persisted in and pass the `timestamp` from the `EventMetadata` as an argument. If no update of that resource occurred after the specified time, you won't receive any resource data.

#### Interest in all missed events

If you require retrieval of all events which occurred during your outage, the same `GetEvents` RPC call can be used. Your responsibility is to find the newest `timestamp` among your events and retrieve all resources with global timestamp ETag set to this value. The expected format of the timestamps is Unix time in nanoseconds. As a response, you get all events that were published after the specified time.

{{% note %}}
Described RPC call of the plgd gRPC Gateway supports both global ETag as well as ETag per resource. Additionally, you can apply device id or resource id filters to limit your request to the predefined set of devices and/or resources.
{{% /note %}}

### Data Reconciliation using JetStream

The plgd cloud uses NATS as an EventBus while keeping the events persisted in our EventStore, the MongoDB. There are use-cases where plgd cloud users are interested in using JetStream as an EventBus and subscribe to it. This option is built-in in the deployment and can be easily enabled by configuring the helm chart (TODO).

Having JetStream as an EventBus gives you the possibility to read stored events after your service outage right from the JetStream instead of requesting the data from the plgd gRPC Gateway.

{{% warning %}}
plgd Cloud doesn't guarantee delivery of all events to the EventBus. It guarantees that all events are stored in the EventStore in the correct order. In case there is a JetStream / NATS failure and plgd Cloud was not able to publish some events, they won't be published again and your service has to anyway fallback to reconciliation using plgd gRPC Gateway.
{{% /warning %}}
