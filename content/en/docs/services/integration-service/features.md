---
title: 'Features'
description: 'What features does the Integration Service provide?'
date: '2024-06-14'
categories: [events, iot, integration]
keywords: [event streaming, integration service, plgd hub]
weight: 3
---

### Planned Features

- **&#x2610; Event Stream Management:** Seamlessly stream events from the plgd hub to a wide range of external services. Configure rules to control which events are transmitted and where they are sent.

- **&#x2610; Advanced Filtration Mechanism:** Enhance event filtering with options including:
   - **Device ID:** Filter events based on the unique identifier of the originating device.
   - **Href:** Utilize the href attribute for event selection.
   - **Resource Types:** Specify resource types to target events relevant to particular data types.
   - **Event Type:** Differentiate events based on their type.
   - **Content Filtering:** Apply jq expressions for advanced content-based filtering.

- **&#x2610; Supported Target Services:**
   - **HTTP:** Integrate with web services and APIs via HTTP endpoints.
   - **NATS:** Use the NATS messaging system for distributed event streaming.

- **&#x2610; Dynamic Target Parameters:** Define target parameters using templates with placeholders to dynamically populate values from event content, ensuring accurate data delivery to external services.
