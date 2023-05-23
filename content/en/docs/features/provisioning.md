---
title: 'Device Provisioning'
description: 'How to securely provision the device?'
date: '2022-02-12'
categories: [features]
keywords: [provisioning, zero-touch]
weight: 10
---

When to use Device Provisioning Service
There are many provisioning scenarios in which DPS is an excellent choice for getting devices connected and configured to IoT Hub, such as:
Zero-touch provisioning to a single IoT solution without hardcoding IoT Hub connection information at the factory (initial setup)
Load-balancing devices across multiple hubs
Connecting devices to their owner's IoT solution based on sales transaction data (multitenancy)
Connecting devices to a particular IoT solution depending on use-case (solution isolation)
Connecting a device to the IoT hub with the lowest latency (geo-sharding)
Reprovisioning based on a change in the device
Rolling the keys used by the device to connect to IoT Hub (when not using X.509 certificates to connect)
