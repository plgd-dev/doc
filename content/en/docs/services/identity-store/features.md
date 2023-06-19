---
title: 'Features'
description: 'What features provides Identity Store?'
date: '2023-06-14'
categories: [ownership, iot]
keywords: [owner, validation, features]
weight: 2
---

### Implemented Features

- **&#x2611; Device-Owner Relationship Management:** The Identity Store efficiently manages the association between devices and their owners within the Hub.
- **&#x2611; Device Registration:** Devices can be registered with the Hub, indicating their assignment to an owner in the Identity Store.
- **&#x2611; Device Unregistration:** Devices can be unregistered from the Hub, signifying their disassociation from an owner in the Identity Store.
- **&#x2611; Event Propagation:** Changes in the Identity Store, such as device registrations and unregistrations, are propagated to the Hub through the "DevicesRegistered" and "DevicesUnregistered" events.
- **&#x2611; Customizable Owner Claim Name:** The Identity Store offers the flexibility to customize the name of the owner claim, allowing for personalized owner identification.

### Planned Features

- **&#x2610; Device Sharing among Owners:** Enable devices to be shared among multiple owners, expanding collaboration and access capabilities for devices within the Identity Store.
- **&#x2610; Hub Identification in Events:** Add the hub ID to events to enable the identification of the hub that generated the event.
