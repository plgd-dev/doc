---
title: 'Pending command'
description: 'What is pending command?'
date: '2021-08-04'
categories: [features]
keywords: [command, history]
weight: 50
---

Each command of hub is converted to events. eg:

- create resource is converted to the `ResourceCreatePending` event.
- retrieve resource is converted to the `ResourceRetrievePending` event.
- update resource is converted  to `ResourceUpdatePending` event.
- delete resource is converted  to `ResourceDeletePending` event.
- update device metadata is converted  to the `DeviceMetadataUpdatePending` event.

All these events wait for processing by one of a gateway (mostly coap-gateway).

When one of the gateways process a pending event it executes one confirmation of command which converted to event:

- confirm resource create is converted to confirmation event `ResourceCreated`.
- confirm resource retrieve is converted to confirmation event `ResourceRetrieved`.
- confirm resource update is converted to confirmation event `ResourceUpdated`.
- confirm resource delete is converted to confirmation event `ResourceDeleted`.
- confirm device metadata update is converted to confirmation event `DeviceMetadataUpdated`.

## Expiration and cancellation

When the device is not online the event waits for processing.
To limits wait:

- you can set `time_to_live` for each command, which defines the expiration. When an event expires hub doesn't process it anymore.
- cancel resource command, which produces confirmation event with status Canceled.
- cancel update device metadata, which produces confirmation event with set Canceled.

### Races

If the cancellation command occurs after confirmation, the cancellation fails.
If the pending command expires or was canceled before execution of the confirmation command, the confirmation command fails. In this case, there are 3 scenarios:

- for retrieve, it doesn't change any state of device or hub
- for the update, the hub is notified about the current state of resources through observation
- for the creation, the hub is notified about created resource via publishing it
- for delete, the hub is notified about deleted resource via unpublishing it
- for update device metadata, the gateway must revert to the previous state
