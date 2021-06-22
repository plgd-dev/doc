---
title: 'Audit Log'
description: 'What is Audit Log and how does it work?'
date: '2021-06-11'
lastmod: '2021-06-18'
categories: [features]
keywords: ['duplicated events', 'audit log', 'device twin']
menu:
  docs:
    parent: features
    weight: 10
toc: true
---

# Audit Log

TODO

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

## ResourceLinksPublished

Duplicate publishing of a resource should be avoided. To ensure this a snapshot with map of published resources is stored. The map stores resources in key-value pairs, where `href` of the resource is used as the key and the resource itself as the value. Resources are added to or updated in the map whenever a `ResourceLinksPublished` event occurs. Resources are removed from the map whenever a `ResourceLinksUnpublished` event occurs.

The resources in the `ResourceLinksPublished` event are examined and only newly published or updated resources will be kept in the event. Resources that have already been published or have no changes in the data will be filtered out. The algorithm is as follows:

```pseudocode
for Resource in the event {
    if Resource is in the snapshot map and
      Resource data is equal to the resource data in the snapshot map {
        Remove Resource from the event
    } else {
      Add or update Resource in the snapshot map
    }
}
```

If no resources to be published remain in the event then the event is skipped and it's not added to the event store.

## ResourceLinksUnpublished

Event is added to event store only if at least one resource was unpublished.

## DeviceMetadataUpdated

TODO
