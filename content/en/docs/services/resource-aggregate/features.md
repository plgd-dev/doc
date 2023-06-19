---
title: 'Features'
description: 'What features provides Resource Aggregate?'
date: '2023-06-14'
categories: [events, iot]
keywords: [cqrs, features]
weight: 2
---

### Implemented Features

- **&#x2611; Event-driven Transaction Handling:** The Resource Aggregate operates based on fine-grained event streams, enabling transactional operations on a device's resource.
- **&#x2611; Event Persistence:** Events describing actions within the Resource Aggregate are persisted in the EventStore, creating an append-only log of the resource's state changes.
- **&#x2611; Event Deduplication:** The Resource Aggregate deduplicates events to prevent duplicate events from being emitted when a command arrives.
- **&#x2611; Event Publication:** Persisted events in the EventStore are published to the EventBus for distribution to other system components or services interested in resource-related events.
- **&#x2611; Optimistic Concurrency Control:** The Resource Aggregate utilizes the optimistic concurrency control method to prevent conflicts during writes to the EventStore.
- **&#x2611; Snapshot-based State Optimization:** Implementing snapshot functionality to capture and store periodic snapshots of the resource's state, enhancing performance and reducing the need for event replay during recovery processes.
- **&#x2611; Command Validation:** Incoming commands to the Resource Aggregate are validated to ensure their correctness and adherence to defined rules.
- **&#x2611; Command Expiration:** Each command has a valid until time and requires a confirmation event. If the confirmation event is not received before the valid until time, the pending command is considered invalid and discarded.
- **&#x2611; Cancel Commands:** The Resource Aggregate supports the cancellation of pending commands, allowing the gateway to cancel a command that has not yet been processed by the Resource Aggregate.
- **&#x2611; Pending Command Execution on Device Online:** Commands that are awaiting the device's online status are immediately processed when the device comes online.

### Planned Features

- **&#x2610; Hub Identification in Events:** Add the hub ID to events to enable the identification of the hub that generated the event.
