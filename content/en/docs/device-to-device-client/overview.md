---
title: 'Overview'
description: 'What is plgd Device to Device client?'
date: '2022-08-18'
categories: [d2d, client]
keywords: [d2d, client, proximity, udp]
weight: 1
---

## Communicate with the device directly, without any backend

Machine to machine communication is required for many interoperable scenarios, where one device needs to notify another one or where the device needs to retrieve a configuration from a different one. The plgd platform together with the IoTivity support all deployment scenarios and communication models. The easiest way on how to develop machine to machine interactions or to test your device, is to use the [plgd d2d client](https://github.com/plgd-dev/client-application). The plgd d2d client is using the [device/client Go library](https://github.com/plgd-dev/device/tree/main/client) and exposes it's API through [HTTP](https://github.com/plgd-dev/client-application/blob/main/pb/service.swagger.json) and [gRPC](https://github.com/plgd-dev/client-application/blob/main/pb/service.proto) endpoints.

The built-in UI allows you to discover devices, add them by IP, own them and interact over it's resources.
