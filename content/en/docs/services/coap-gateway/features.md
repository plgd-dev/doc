---
title: 'Features'
description: 'What features provides CoAP Gateway?'
date: '2023-06-14'
categories: [security, gateway, iot]
keywords: [security, coap, features]
weight: 2
---

### Implemented Features

- **&#x2611; Server and Client Functionality:** The CoAP gateway acts as both a server and a client, facilitating communication with IoT devices through a single connection.
- **&#x2611; OCF Specification Compliance:** The CoAP gateway adheres to the OCF specification for IoT devices, ensuring compatibility and interoperability with other OCF-compliant devices.
- **&#x2611; Device Authentication:** The CoAP gateway supports multiple authentication methods, including OCF identity certificate, any certificate, and disable authentication, providing flexibility in verifying device credentials.
- **&#x2611; Device Authorization:** Devices are authorized to access the IoT hub based on the JWT token provided, either by requiring a device ID claim or validating the overall token's validity.
- **&#x2611; Device Twin Synchronization:** After authentication, the CoAP gateway synchronizes the device twin by subscribing to device resources, forwarding changes to the IoT hub, and ensuring an accurate representation of the device's state.
- **&#x2611; Event Processing:** The CoAP gateway subscribes to events from the IoT hub and converts them into CoAP commands, allowing seamless communication with devices during the device twin synchronization process.
- **&#x2611; Time Resource:** The CoAP gateway provides the current time to authenticated devices, allowing them to synchronize their internal clocks.

### Planned Features

- **&#x2610; Device Twin Synchronization with ETAG:** Implementing the ETAG mechanism will ensure that device twin synchronization occurs only when the device's state has changed since the last synchronization. This optimization reduces unnecessary data transfer between the CoAP gateway and the IoT hub, enhancing performance when the device is online.
- **&#x2610; Device Logging:** Introduce device logging capabilities to record detailed activity logs of each device. The logs can be forwarded to the Open Telemetry collector for further analysis and monitoring purposes.
