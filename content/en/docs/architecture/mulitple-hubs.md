---
title: 'Multiple Hubs'
description: 'How to interconnect multiple hubs'
date: '2023-08-07'
categories: [architecture]
keywords: [hubs]
weight: 3
---

There are 2 options:

1. Hubs shares eventstore and eventbus among deployment.

## Sharing eventstore and eventbus

Using Shards zones + Super cluster in NATs

https://www.mongodb.com/docs/manual/tutorial/manage-shard-zone/

https://docs.nats.io/running-a-nats-service/configuration/gateways


2. Hubs are independent and the application builded over them address them through `HubID` identifier.


Shared data