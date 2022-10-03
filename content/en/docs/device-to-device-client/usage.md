---
title: 'Client Usage'
description: 'How to work with the d2d client?'
date: '2022-08-18'
lastmod: '2022-08-18'
categories: [d2d, client]
keywords: [d2d, client, discovery, udp, ownership]
menu:
  docs:
    parent: device-to-device-client
    weight: 40
toc: true
---

## Device Discovery

Devices are discovered by sending the CoAP multicast message and waiting for the configured timeout for responses from devices, which are available in the subnet. In case you want to interact with the device which cannot be discovered using multicast, either it's not in the same subnet or the multicast is blocked, you can add the device by the IP.
Discovered devices might be in different device ownership states. Some devices might be new - `unowned` and some of them already `owned`. The ownership of devices that are in the `unowned` state can be using the configured ownership transfer method transfered to you. But how exactly is `you` defined? More information about `ownerID` can be found [here](). In this document, let's focus more on ownership transfer use-cases and device credentials.

WIP