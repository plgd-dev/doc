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

There are two occasions when the `ResourceChange` event is skipped - when it is out of sequence and when it is a duplicate. Metadata and data of the latest event is cached and used to check whether a problematic case occured.

The order of events is not guaranteed, thus duplicate events can occur. To avoid this each event carries metadata. In the metadata there is a unique identifier for the connection that triggered the event, and a sequence number of the event for the given connection. A simple algorithm uses these two values and ensures that older data doesn't overwrite. To check whether an event is not out-of-sequence for given connection the following simple algorithm is used:

```pseudocode
isTheSameConnection := compare the connection identifier from the event with the connection identifier from the cached metadata
if isTheSameConnection {
  isValidSequence := sequence number from the event is greater than the sequence number from the cached metadata
  if not isValidSequence {
      return false
  }
}
overwrite cached metadata with the metadata from the current event
return true
```

Thus subsequent events from the same connection, with no interweaving connections, and with invalid sequence numbers are skipped.

The second circumstance under which events are skipped is when the data of the event itself didn't change. To check whether data was updated the following algorithm is used:

```pseudocode
hasTheSameData := compare event data with the cached event data
if hasTheSameData {
    return false
}
overwrite cached event data with the current event data
return true
```

If both the not out-of-sequence check and the data changed check succeed then the event is added to event store.

## ResourceLinksPublished

TODO

## ResourceLinksUnpublished

TODO

## DeviceMetadataUpdated

TODO
