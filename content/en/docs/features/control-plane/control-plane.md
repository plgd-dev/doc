---
title: 'Control Plane'
description: 'What are the control plane features of plgd system?'
date: '2023-05-23'
categories: [zero-touch, provisioning. features]
keywords: [provisioning, TPM, device-twin]
isMainSectionPage: true
weight: 1
---

The control plane of the system provides various interfaces and functionalities for managing devices, controlling their operations, and handling maintenance tasks. Here is an overview of the different components and features of the control plane:

## Device to IoT Hub

The Device to IoT Hub communication is facilitated by the CoAP Gateway, allowing bidirectional communication between devices and the IoT Hub.

### HTTP API

The HTTP API is a RESTful API that allows users to manage devices in the system. It provides a user-friendly interface for performing maintenance tasks, managing device metadata, and controlling devices either from the cloud or the local network. The HTTP API is built on top of the CoAP Gateway and Resource Directory, which handle communication with the devices.

[Link to HTTP API Documentation](/docs/features/control-plane/http-api)

### GRPC API

The GRPC API is a protocol buffer-based API that enables users to manage devices in the system. It offers a simple and intuitive interface for performing maintenance tasks, managing device metadata, and controlling devices from the cloud or local network. The GRPC API is also built on top of the CoAP Gateway and Resource Directory.

[Link to GRPC API Documentation](/docs/features/control-plane/grpc-api)

### Device Twin

The device twin in IoT Hub represents the current state of each device's resource. Whenever a connected device undergoes any changes, it notifies the IoT Hub using CoAP Gateway observations. These observations are initiated as soon as the device successfully connects and authenticates with the hub. All changes made by the device are stored as an audit log in the EventStore. The latest version of the device twin is then made available to clients through the Resource Directory.

[Learn more about Device Twin](/docs/features/control-plane/device-twin)

### JetStream as an EventBus

IoT Hub services utilize NATS as the default EventBus and MongoDB as the EventStore. However, there are cases where direct subscription to the internal messaging system is necessary, bypassing the IoT Hub gateways. To simplify data reconciliation and enable easier scaling of consumers, IoT Hub supports an alternative EventBus called JetStream. JetStream is built on top of NATS and persists all published events. By utilizing JetStream as an EventBus, users can access older, as-yet-unprocessed messages without directly accessing the EventStore.

[Learn more about JetStream as an EventBus](/docs/features/control-plane/jetstream)

### Pending Commands

Each command issued is converted into an event and placed in a pending state, awaiting processing by one of the gateways, primarily the CoAP Gateway. When a pending event is processed by a gateway, it triggers the execution of a confirmation command, which is then converted into a confirmation event. If a device is offline, the event remains pending until it can be processed. A `time_to_live` parameter can be set for each command to limit the waiting time and specify its expiration. Once an event expires, the hub no longer processes it. It is also possible to cancel a resource command, resulting in a confirmation event with the status set to `Canceled`. However, if a cancellation command is issued after confirmation, the cancellation fails. If a pending command expires or is canceled before the confirmation command is executed, the confirmation command fails.

[Learn more about Pending Commands](/docs/features/control-plane/pending-commands)

## Device to Device

The Device to Device (D2D) feature enables users to control devices directly from their local network. It allows users to access devices without going through the cloud, which is useful for performing maintenance tasks and managing devices conveniently. The D2D feature is built on top of the `plgd d2d client`, which handle communication with the devices.

[Learn more about Device to device](/docs/device-to-device-client/)

## Automated Device Configuration

Ensure devices are always set up correctly and efficiently with the Snippet Service. Predefined configuration profiles are automatically applied in response to events like changes in resources. This automation minimizes manual intervention, enhances operational reliability, and ensures consistent device settings across your environment.

[Learn more about Automated Device Configuration](/docs/services/snippet-service/)
