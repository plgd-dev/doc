---
title: 'Disaster Recovery'
description: 'Getting back online and in-sync'
date: '2024-02-06'
categories: [features]
keywords: ['disaster recovery', 'data reconciliation' , 'jetstream']
weight: 70
---

Resiliency is understood as a way to readapt to a "crisis situation", which applies to both infrastructure and the data. What it means varies from user to user. After service disruptions, some users are okay retrieving the latest resource value while others require all the changes which occurred during the time their application or internal messaging system was down. Another failure scenario is when the node hosting the plgd CoAP Gateway crashes. Thousands of devices will try to reconnect and keep the device twin up to date, which creates a huge load and rapidly slows down the system if not handled properly.

plgd hub offers to users various techniques on how to approach disaster recovery in case an error or system failure was detected. Let's have a look at them.

## Event Data Loss

{{< note >}}
plgd hub is an event-driven system, implemented using [CQRS](https://docs.microsoft.com/en-us/azure/architecture/patterns/cqrs) and [EventSourcing](https://docs.microsoft.com/en-us/azure/architecture/patterns/event-sourcing) design patterns. Each event that occurs in the system, _e.g. when the content of resource changes, when a new resource is published, or when a new device is registered with the plgd hub,_ is stored in the EventStore and published to the EventBus.

plgd Gateways are subscribed to the EventBus to notify you through the gRPC stream or WebSockets about requested changes. Using plgd Gateways is a straightforward option how to communicate with the system, which can provide you both: the current value of the resource or a set of events representing changes of that particular resource (audit log).
{{< /note >}}

An active subscription to plgd events, using gRPC, WebSockets, NATS, or JetStream might fail. Same as the publish operation executed by the plgd service. There are multiple reasons which we cannot prevent - infrastructure failure when the node went down, service crash, or operator failure during the roll-out. To be able to get your system back to the in-sync state, failure has to be detected and the data reconciliation process started.

### Missing Events Detection

Sometimes it might be obvious that some events might be lost. If your service consuming plgd events restarted due to a crash, network outage, or infrastructure failure, there is a high chance that you missed some events. In such a scenario, the service shall start right away it's up and running with the data reconciliation process.

Another, not so obvious event data loss might occur due to EventBus service disruptions or during a very short network outage. Your service wasn't restarted; your messaging client just missed one event.
If your service is subscribed to events and requires the processing of all events in the correct order - skipping one event is not accepted, your service needs to track the version of each [event](https://github.com/plgd-dev/hub/blob/main/resource-aggregate/pb/events.proto). In case the **received event's version isn't incremented by one compared to your latest event**, your service shall start the resource reconciliation process.

### Data Reconciliation using gRPC

#### Interest in the latest resource content only

The plgd gRPC Gateway exposes `GetEvents` RPC call to retrieve the latest version of the resource. In case you are not interested in all the changes which occurred during your outage, this is the right way how to get in sync. Additionally, to optimize this operation, timestamp ETag can be specified. Just take the latest event you have persisted in and pass the `timestamp` from the `EventMetadata` as an argument. If no update of that resource occurred after the specified time, you won't receive any resource data.

#### Interest in all missed events

If you require retrieval of all events which occurred during your outage, the same `GetEvents` RPC call can be used. Your responsibility is to find the newest `timestamp` among your events and retrieve all resources with global timestamp ETag set to this value. The expected format of the timestamps is Unix time in nanoseconds. As a response, you get all events that were published after the specified time.

{{< note >}}
Described RPC call of the plgd gRPC Gateway supports both global ETag as well as ETag per resource. Additionally, you can apply device id or resource id filters to limit your request to the predefined set of devices and/or resources.
{{< /note >}}

### Data Reconciliation using JetStream

The plgd hub uses NATS as an EventBus while keeping the events persisted in our EventStore, the MongoDB. There are use-cases where plgd hub users are interested in using JetStream as an EventBus and subscribe to it. This option is built-in in the deployment and can be easily enabled by configuring the helm chart (TODO).

Having JetStream as an EventBus gives you the possibility to read stored events after your service outage right from the JetStream instead of requesting the data from the plgd gRPC Gateway.

{{< warning >}}
plgd hub doesn't guarantee delivery of all events to the EventBus. It guarantees that all events are stored in the EventStore in the correct order. In case there is a JetStream / NATS failure and plgd hub was not able to publish some events, they won't be published again and your service has to anyway fallback to reconciliation using plgd gRPC Gateway.
{{< /warning >}}

## Data Management and Failover Strategies

The plgd hub is a stateful event-driven system, meaning that data is stored in the EventStore, which serves as the authoritative source of truth and is implemented using MongoDB. In this section, we will describe how to back up and restore data in a scenario where two clusters are running in different locations (e.g., East US / West US). The first cluster is utilized for normal operations, while the secondary cluster serves as a backup for disaster recovery.

### Backup Databases

To back up the database, two approaches can be used:

* **Passive Backup**
  
  ![passive-backup](/docs/features/monitoring-and-diagnostics/static/disaster-recovery-passive-backup.drawio.svg)

  The database is regularly backed up to a different location and can be used in case of failure. Although this approach is simple and requires fewer resources, the data may become outdated, and the restoration process may take some time. For MongoDB, utilize the `mongodump` tool to create a export of the database contents, store it securely, and use it in case of failure. Regular backups are essential to keep the data up-to-date. For more details on this approach, refer to the [MongoDB documentation](https://www.mongodb.com/docs/database-tools/mongodump/).

* **Active Backup**

  ![active-backup](/docs/features/monitoring-and-diagnostics/static/disaster-recovery-active-backup.drawio.svg)

  The database actively synchronizes data between two clusters in realtime. This approach is more complex and resource-intensive but is more reliable for disaster recovery. For MongoDB, use `cluster-to-cluster-sync` feature to synchronize data between two MongoDB clusters. For more details on this approach, refer to the [MongoDB documentation](https://www.mongodb.com/docs/cluster-to-cluster-sync/current/connecting/onprem-to-onprem/).

{{< warning >}}

**Using a backup from EventBus (JetStream) is not recommended in a restored cluster**, as it is not the source of truth for the plgd hub. This could result in data inconsistency because creating backups is not synchronized for both EventStore and EventBus (JetStream). The EventBus can contain events that are not stored in the EventStore, and vice versa.

{{< /warning >}}

### OAuth2 Server

Devices connected to the hub have access tokens used to authorize device access. The access tokens are generated by the OAuth2 server, and its database needs to be backed up regularly. In case of OAuth2 server failure, devices won't connect to the hub. To prevent this, regularly back up the OAuth2 server database as described in the [Backup Databases](#backup-databases) section.

### Certificates

![certificates](/docs/features/monitoring-and-diagnostics/static/disaster-recovery-certificates.drawio.svg)

The CoAP-Gateway and Device Provisioning Service depend on certificates validated by devices, and these certificates must be signed in the certificates chain by the same Root CAs. It is crucial that the Root CAs used for the primary and secondary clusters are identical. Additionally, the hub ID configured through the [plgd helm chart](https://github.com/plgd-dev/hub/blob/4c4861a4bc483ba4080a1d448063da392eff4026/charts/plgd-hub/values.yaml#L6) must remain consistent.

### Devices

If a primary cluster failure occurs and you cannot dynamically modify the endpoint on the devices, they will be unable to establish a connection with the hub. Devices are set up with a single endpoint to link with either the CoAP-Gateway or the Device Provisioning Service, which may include an IP address or DNS address. To guarantee connectivity to the secondary cluster, adopt one of the provided options:

* **DNS Address as endpoint**
  
  In case of primary cluster failure, update the DNS record on the DNS server. It is recommended to set the time to live (TTL) of the DNS record to a low value, e.g., 30 minutes.

* **IP Address as endpoint**
  
  ![load-balancer](/docs/features/monitoring-and-diagnostics/static/disaster-recovery-load-balancer.drawio.svg)

  Changing the IP address could be challenging in case of primary cluster failure, as the public IP address is often assigned to the Internet Service Provider (ISP). However, using an IP load balancer near devices allows changing the IP address of the load balancer to the secondary cluster. For this, you can use HAProxy, which supports layer 4 load balancing. For more information, refer to the [HAProxy documentation](https://www.haproxy.com/documentation/haproxy-configuration-tutorials/load-balancing/tcp/) and [Failover & Worst Case Management With HAProxy](https://www.haproxy.com/blog/failover-and-worst-case-management-with-haproxy).

* **Update Device Provisioning Service endpoint**

  Under these circumstances, you have the option to update the DPS endpoint to the secondary cluster by utilizing the DHCP server to supply the devices with the updated endpoint. The device retrieves a new configuration from the DPS service, obtaining updated:

  * Time(optional)
  * Owner
  * Credentials - Identity certificate, root CA certificate and Pre-shared key(optional)
  * Access control lists (ACLs)
  * Cloud configuration - Authorization code, Hub ID, Hub URL, etc.
  
  Subsequently, the module connects to the cloud, with the first operation being to sign up for self-registration.

  **From the Hub perspective:**

  The Hub detects that the module has already been registered (from the restored database) because the DeviceID and owner haven't changed, indicating no factory reset occurred. Consequently, the device events will continue from the restored state. If your application relies on event versions, please **be mindful that the version may be in the past**, depending on when the backup was performed.
