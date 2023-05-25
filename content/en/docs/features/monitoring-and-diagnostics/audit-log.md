---
title: 'Audit Log'
description: 'What is Audit Log and how does it work?'
date: '2021-06-11'
categories: [features]
keywords: ['duplicated events', 'audit log', 'device twin']
weight: 90
---

An audit log in the context of the Device Hub system is a comprehensive record of events and actions that occur within the system. It serves as a valuable tool for tracking, monitoring, and analyzing user activities and system events. Each event captured in the audit log contains essential components such as `AuditContext` and `EventMetadata`.

The `AuditContext` component provides important contextual information about the user involved in the event. It includes the `user_id` field, which stores the identifier of the user responsible for the action. This allows administrators to track and attribute specific events to individual users. Additionally, the `correlation_id` field is used to establish a correlation between related events or actions. This enables grouping and analysis of events that are part of a larger workflow or process.

The `EventMetadata` component provides additional details and metadata about each event. The `version` field within the `EventMetadata` represents a monotonically increasing number without gaps. It is specifically used for event sourcing events, allowing for ordering and consistency within the event stream. The `timestamp` field records the exact date and time when the event occurred, facilitating chronological sequencing and analysis. The `connection_id` field identifies the specific connection associated with the event, providing insights into the network or session context in which the event took place. It helps in identifying the source or origin of the event. Additionally, the `sequence` field, while not filled for all event types, may be used by certain events, such as the ResourceChanged event, to skip older events and process only the most recent data efficiently.

By maintaining an audit log with the provided components, administrators can effectively monitor and review system activities, track user actions, ensure compliance with security policies, and investigate any security breaches or policy violations. The log serves as a valuable source of information for analysis, troubleshooting, and maintaining the integrity and security of the Device Hub system.

## Event Deduplication

### ResourceChanged

There are two occasions when the `ResourceChange` event is skipped - when it is out of sequence and when it is a duplicate. Metadata and data of the latest event are stored in a snapshot and are used to check whether a problematic case occurred.

The order of events is not guaranteed, thus duplicate events can occur. To avoid this each event carries metadata. In the metadata there is a unique identifier for the connection that triggered the event, and a sequence number of the event for the given connection. A simple algorithm uses these two values and ensures that older data doesn't overwrite. To check whether an event is not out-of-sequence for given connection the following simple algorithm is used:

```pseudocode
isTheSameConnection := compare the connection identifier from the event with the connection identifier from the snapshot metadata
if isTheSameConnection {
  isValidSequence := sequence number from the event is greater than the sequence number from the snapshot metadata
  if not isValidSequence {
      return false
  }
}
overwrite metadata snapshot with the metadata from the current event
return true
```

Thus subsequent events from the same connection, with no interweaving connections, and with invalid sequence numbers are skipped.

The second circumstance under which events are skipped is when the data of the event itself didn't change. To check whether data was updated the following algorithm is used:

```pseudocode
hasTheSameData := compare event data with the snapshot event data
if hasTheSameData {
    return false
}
overwrite event data snapshot with the current event data
return true
```

If both the not out-of-sequence check and the data changed check succeed then the event is added to event store.

### ResourceLinksPublished

Duplicate publishing of a resource should be avoided. To ensure this a snapshot with map of published resources is stored. The map stores resources in key-value pairs, where `href` of the resource is used as the key and the resource itself as the value. Resources are added to or updated in the map whenever a `ResourceLinksPublished` event occurs. Resources are removed from the map whenever a `ResourceLinksUnpublished` event occurs.

The resources in the `ResourceLinksPublished` event are examined and only newly published or updated resources will be kept in the event. Resources that have already been published or have no changes in the data will be filtered out. The algorithm is as follows:

```pseudocode
for Resource in the event {
    if Resource is in the snapshot map and
      Resource data is equal to the resource data in the snapshot map {
        remove Resource from the event
    } else {
      add or update Resource in the snapshot map
    }
}
```

If no resources to be published remain in the event then the event is skipped and it's not added to the event store.

### ResourceLinksUnpublished

The `ResourceLinksUnpublished` event contains a list of resource `href`s to be unpublished. To avoid the storing of unnecessary unpublish events in the event store, the snapshot map from `ResourceLinksPublished` is used to determine which resources are currently published and thus can be unpublished. The algorithm works as follows:

```pseudocode
for Resource-Href in the event {
    if Resource-Href exists as key in the snapshot map {
        remove Resource-Href from the snapshot map
    } else {
        remove Resource-Href from the event
    }
}
```

After the algorithm only `href`s that were previously published remain in the event which is added to the event store. Also the unpublished `href`s are removed from the snapshot map.

Additionally, there is one special case if the `ResourceLinksUnpublished` event contains an empty list of `href`s then all currently published resources are unpublished and the snapshot map is cleared.

### DeviceMetadataUpdated

A similar technique to the one utilized for `ResourceChanged` is used to avoid adding duplicate subsequent `DeviceMetadataUpdated` events to the event store. The last handled `DeviceMetadataUpdated` event is saved and used to skip unnecessary events.

```pseudocode
hasTheSameData := compare event data with the saved event data
if hasTheSameData {
    return false
}
overwrite the saved event data with the current event data
return true
```

Thus subsequent `DeviceMetadataUpdated` events without any data changes are skipped and not added to the event store.
