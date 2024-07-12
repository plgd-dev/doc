---
title: 'Features'
description: 'What features does the Snippet Service provide?'
date: '2024-07-12'
categories: [snippet-service, automation, configuration]
keywords: [device-configuration, snippet-service]
weight: 2
---

## Description

This feature allows users to pre-configure devices by specifying desired configurations that are automatically applied when a ResourceChanged event occurs. The newly introduced `snippet service` is responsible for managing this process.

### Implemented features

- **&#x2611; Device Configuration:** Users can create and store configurations for devices. These configurations are automatically applied when a ResourceChanged event occurs.
- **&#x2611; Event-Driven Configuration:** The configuration is triggered by the `ResourceChanged` event. Users can set filters to define the conditions under which configurations are applied.
- **&#x2611; Filter Conditions:** Users must set at least one of the following filters: DeviceID filter (trigger based on specific device IDs), Resource-Type filter (trigger when all specified resource types are present), href filter (trigger based on specific href values). All specified filters must be met for the configuration to be applied. For DeviceID and href filters, any listed value can trigger the filter. For the Resource-Type filter, all listed values must be present.
- **&#x2611; Optional JQ Expression:** Users can optionally use a JQ expression to filter the content of the `ResourceChanged` event. The JQ expression must evaluate to true for the configuration to be applied.
- **&#x2611; User Interface:** A user-friendly UI is provided to manage configurations and set filters easily.
- **&#x2611; APIs:** The snippet service provides both a [gRPC API](https://github.com/plgd-dev/hub/blob/main/snippet-service/pb/service.proto) and an [HTTP API](https://petstore.swagger.io/?url=https://raw.githubusercontent.com/plgd-dev/hub/main/snippet-service/pb/service.swagger.json), allowing users to interact with the service and perform configuration-related operations.

### Planned features

- **&#x2610; Enhanced Filtering Mechanisms:** Additional filter conditions and more complex filtering logic will be supported to offer even greater flexibility and control over configuration application.
- **&#x2610; Apply Configuration to Different Devices:** Users will be able to apply configurations to devices other than the one that triggered the `ResourceChanged` event.
