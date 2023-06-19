---
title: 'Features'
description: 'What features provides gRPC Gateway?'
date: '2023-06-14'
categories: [gateway, client]
keywords: [api, grpc, features]
weight: 2
---

### Implemented Features

- **&#x2611; Device Management:** The gRPC Gateway provides comprehensive device management capabilities, including retrieving devices, deleting devices, managing resource links, accessing specific resources, retrieving device twin information, updating resources, deleting resources, and creating resources.
- **&#x2611; Event Handling and Subscription:** Clients can subscribe to device events and retrieve events based on device ID, resource ID, and event types.
- **&#x2611; Device Metadata and Configuration:** The gRPC Gateway allows retrieval of hub configuration, updating device metadata (e.g., device twin), retrieving devices' metadata (including online/offline status and device twin synchronization), and canceling pending metadata updates.
- **&#x2611; Command Handling:** The gRPC Gateway enables the retrieval of pending commands for devices and cancellation of resource commands.
- **&#x2611; Protocol Buffers (Protobuf) Support:** gRPC Gateway utilizes Protocol Buffers for defining service interfaces and message structures, enabling efficient and language-agnostic communication.

### Planned Features

- **&#x2610; Metrics:** The gRPC Gateway will provide metrics and monitoring capabilities, allowing visibility into its performance, resource utilization, and operational insights.
