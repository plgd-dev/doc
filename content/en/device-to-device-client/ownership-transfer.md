---
title: 'Ownership Transfer and Provisioning'
description: 'How does security work with d2d?'
date: '2022-08-18'
lastmod: '2022-08-18'
categories: [d2d, client]
keywords: [d2d, client, proximity, udp]
menu:
  docs:
    parent: device-to-device-client
    weight: 30
toc: true
---

## Device Ownership

The plgd d2d client can become an owner of a new, `unowned` device. Supported [ownership transfer methods](https://openconnectivity.org/specs/OCF_Security_Specification_v2.2.5.pdf#page=48) by the plgd d2d client are:

- Just Works
- Manufacturer Certificate

Supported [credetials](https://openconnectivity.org/specs/OCF_Security_Specification_v2.2.5.pdf#page=40) which are provisioned on the device to **verify** the client are:

- Pre-shared key
- Identity Certificate

{{% note %}}
With the default configuration, the plgd d2d client provisions the device with the pre-shared key, generated during the startup. This one is then used for every device.
{{% /note %}}



