---
title: 'Monitoring and Diagnostics'
description: 'What are the monitoring and diagnostic features of plgd system?'
date: '2023-05-23'
categories: [zero-touch, provisioning. features]
keywords: [provisioning, TPM, device-twin]
weight: 1
isMainSectionPage: true
---

## Audit Log

The audit log is a comprehensive record of events and actions within the system. It tracks user activities and system events, providing valuable information for monitoring, analysis, and security. The audit log consists of two main components: `AuditContext` and `EventMetadata`. The `AuditContext` contains contextual information about the user involved in the event, such as the user ID and correlation ID. The `EventMetadata` provides additional details about each event, including version, timestamp, connection ID, and sequence. The audit log helps administrators track user actions, ensure compliance, and investigate security breaches.

[Learn more about Audit Log](/docs/features/control-plane/audit-log)

## Traceability

Tracing is used to track the progression of a request within a system. In the context of distributed tracing, spans represent units of work within a trace, forming a tree-like structure. Spans contain a span context with globally unique identifiers, and they provide metrics related to request, error, and duration (RED). Tracing helps debug availability and performance issues.

[Learn more about Traceability](/docs/features/control-plane/open-telemetry-tracing)

## Monitoring Device Connection

Monitoring device connections involves two aspects: device keepalive to CoAP gateway and CoAP gateway keepalive to devices. The device, using the iotivity-lite protocol, sends a ping signal to the CoAP gateway every 20 seconds. If no response is received within 4 seconds, the ping interval is decreased. If the device misses six consecutive pings, a reconnect process is initiated. Similarly, the CoAP gateway sends pings to devices, and if no response is received within the specified timeout period, the device is disconnected.

[Learn more about Monitoring Device Connection](/docs/features/control-plane/keepalive)

## Disaster and Recovery

Disaster recovery is crucial for maintaining system resilience and data integrity. plgd hub offers various techniques to handle failures and errors. In case of event data loss, where events might be missed due to service disruptions or network outages, data reconciliation processes can be initiated. Missing events can be detected by comparing event versions or through obvious scenarios such as service restarts. Data reconciliation can be performed using gRPC calls to retrieve the latest resource content or all missed events. JetStream, as an EventBus, provides an alternative option for reading stored events after service outages.

[Learn more about Disaster and Recovery](/docs/features/control-plane/disaster-recovery)
