---
title: 'Configuring Experimental Deployment with CqlDB (Scylla) for IoT Hub'
description: 'Learn how to deploy an IoT Hub with Scylla using experimental configuration.'
docsOthersDisplay: true
date: '2023-12-14'
categories: [deployment, kubernetes, db]
keywords: [deployment, kubernetes, k8s, helm, chart]
weight: 30
---

{{< warning >}}
This feature is experimental and is not recommended for production use.
{{< /warning >}}

Scylla is specifically designed for applications requiring **high-performance** and **low-latency**. Built on the Apache Cassandra architecture, it excels in handling large datasets with efficient read and write operations. Offering **seamless horizontal scalability**, Scylla accommodates increased loads effortlessly, with automatic data distribution across nodes. Its architecture, based on a **wide-column store**, is ideal for scenarios demanding storage and retrieval of large data volumes with high throughput.

To understand how records/data are distributed across Scylla nodes, refer to the [ScyllaDB documentation](https://university.scylladb.com/courses/data-modeling/lessons/basic-data-modeling-2/topic/primary-key-partition-key-clustering-key/).

The **primary key** uniquely identifies a row and consists of a partition key (one or more columns) and an optional clustering key (one or more columns). The **partition key** determines the node storing the data, while **clustering keys** aid in data filtering and sorting within a partition.

## Deploying plgd/hub with CqlDB

While the default database for plgd/hub is MongoDB, you can use CqlDB (Scylla) as an alternative. To do this, install the [Scylla Operator](https://github.com/scylladb/scylla-operator/blob/master/docs/source/generic.md#deploy-scylla-operator) on your cluster. Then, deploy plgd/hub with the following configuration:

```yaml
global:
  useDatabase: "cqlDB"
scylla:
  # Deploy ScyllaDB cluster with 3 nodes
  enabled: true
```

### CqlDB Usage in EventStore (Resource-Aggregate & Resource-Directory)

Each event is aggregated to a snapshot event stored in EventStore for each resource. The **primary key** consists of a partition key `deviceID` (groupID) and a clustering key `id` (aggregateID), ensuring all resources of one device are stored on the same partition. Additionally, a **Secondary Index** with `serviceID` is employed to mark devices with a specific service as offline when the [service goes offline](/docs/features/monitoring-and-diagnostics/monitoring-device-connectivity).

### CqlDB Usage in IdentityStore

IdentityStore stores information about device ownership, utilizing a **primary key** with a partition key `deviceID` and a **Secondary Index** on the `owner` key to identify ownership. This design choice aims to distribute data evenly, leveraging the higher cardinality of devices over owners. Typically, there will be only one owner for all devices, allowing the use of a Secondary Index to retrieve all devices of an owner efficiently.

### CqlDB Usage in CertificateAuthority

CertificateAuthority manages signingRecords holding information on signed Certificate Signing Requests (CSRs). The **primary key** comprises a partition key `id` and clustering keys **owner** and **commonName**. The partition key, determined by a formula utilizing certificate properties, ensures even data distribution. Signing of identity certificate requests is allowed only if the common_name of the CSR matches the `deviceId` with the same owner in the database, or if the record does not exist.
