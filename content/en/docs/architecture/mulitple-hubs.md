---
title: 'Multiple Hubs'
description: 'How to interconnect multiple hubs'
date: '2023-08-07'
categories: [architecture]
keywords: [hubs]
weight: 3
---

In a scenario where multiple hubs need to be interconnected, several architectural approaches can be considered. This document outlines two main options for interconnecting multiple hubs and describes the corresponding strategies and technologies.

## Option 1: Sharing MongoDB and NATs Among Deployment

This approach involves deploying a shared MongoDB database and NATs messaging system that serves multiple hubs.

### Sharding Database Among Regions

For improved scalability and fault tolerance, the database sharding can be extended across multiple regions. Refer to the MongoDB documentation on [Managing Shard Zones](https://www.mongodb.com/docs/manual/tutorial/manage-shard-zone/) for more details.

#### Super Clustering NATs Among Regions

For enhanced resilience and global communication, consider super clustering NATs instances across multiple regions. More information can be found in the [NATs documentation on Gateways](https://docs.nats.io/running-a-nats-service/configuration/gateways).

## Option 2: Independent Hubs

In this option, each hub operates independently and is responsible for managing its own data.

### Using JETStream to get events from other hubs

To enable communication and data sharing with the application, JETStream can be employed. Each event transmitted between hubs includes a unique HubID, facilitating the routing of gRPC/HTTP commands to the correct hub. This approach ensures eventual consistency while allowing hubs to maintain their autonomy.

### Application Level over gRPC or HTTP Protocol

Application-level communication over gRPC or HTTP can be used to enable communication with independent hubs. In this case, each hub is responsible for its own data, and there is no sharing of data between hubs.
