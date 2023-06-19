---
title: 'Resource Aggregate'
description: 'What is plgd Resource Aggregate?'
date: '2023-06-16'
categories: [storage]
keywords: [users, owners]
weight: 1
isMainSectionPage: true
---

The Resource Aggregate is a fundamental concept in plgd that scopes every transaction on a device's resource. It builds its internal state based on a single fine-grained event stream, ensuring consistency and integrity.

When a command is received by the Resource Aggregate from any of the plgd gateways, it undergoes validation. Upon successful validation, an event describing the action is created and persisted in the [EventStore](#event-store). Subsequently, the event is published to the [EventBus](#event-bus).

To prevent conflicts during writes to the EventStore, the Resource Aggregate utilizes the [Optimistic concurrency control](https://en.wikipedia.org/wiki/Optimistic_concurrency_control) method.

### Event Store

The EventStore is a critical component in the Resource Aggregate's architecture as it ensures the persistence of events. It operates by maintaining an append-only log, which guarantees the integrity and preserves the historical changes of the resource's state. To enhance performance, the EventStore also creates periodic snapshots of the resource state based on a configured `N` number of events. It's worth mentioning that the deduplication mechanism within the EventStore may result in zero events being emitted when a command arrives. Finally, it's important to acknowledge that a single command has the potential to generate multiple events within the Resource Aggregate.

### Event Bus

The EventBus facilitates the distribution of events to various components or services within the system. By leveraging the EventBus, the Resource Aggregate can seamlessly communicate resource-related events to other system parts, promoting decoupling and flexibility.

## Resource Aggregate Events

Each pending event has a valid until time and requires a confirmation event. If the confirmation event is not received before the valid until time, the pending event is considered invalid and discarded. The Resource Aggregate produces the following events:

### Resource Links Events

These events notify about new and removal resource links.

- `ResourceLinksPublished`: Indicates that resource links have been published.
- `ResourceLinksUnpublished`: Indicates that resource links have been unpublished.
- `ResourceLinksSnapshotTaken`: Indicates that a snapshot of resource links has been taken.

### Resource Events

These events notify about resource-related events.

- `ResourceChanged`: Indicates that the resource has changed.
- `ResourceUpdatePending`: Indicates that a resource update is pending.
- `ResourceUpdated`: Indicates that the resource has been updated.
- `ResourceRetrievePending`: Indicates that a resource retrieval is pending.
- `ResourceRetrieved`: Indicates that the resource has been retrieved.
- `ResourceDeletePending`: Indicates that a resource deletion is pending.
- `ResourceDeleted`: Indicates that the resource has been deleted.
- `ResourceCreatePending`: Indicates that a resource creation is pending.
- `ResourceCreated`: Indicates that the resource has been created.
- `ResourceStateSnapshotTaken`: Indicates that a snapshot of the resource state has been taken.

### Device Metadata Events

The Device Metadata events provide information about device metadata-related events, including the device's `online`/`offline` status and `device twin` status. For `online` status, a valid time is associated, which needs to be refreshed by the gateway. If the gateway fails to refresh the online status before the valid time expires, the device is considered offline. This mechanism ensures that the online status accurately reflects the device's connectivity.

- `DeviceMetadataUpdatePending`: Indicates that a device metadata update is pending. It is used to enable or disable the device twin functionality.
- `DeviceMetadataUpdated`: Indicates that the device metadata has been successfully updated.
- `DeviceMetadataSnapshotTaken`: Indicates that a snapshot of the device metadata has been taken.
