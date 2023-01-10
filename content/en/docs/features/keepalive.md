---
title: 'KeepAlive'
description: 'How keepalive works with plgd hub'
date: '2023-01-10'
lastmod: '2023-01-10'
categories: [features]
keywords: ['keepalive', 'timeout', 'iotivity-lite', 'coap-gateway']
menu:
  docs:
    parent: features
    weight: 60
toc: true
---


## Device (iotivity-lite) Keepalive to CoAP Gateway

When a device is connected to coap-gateway and signed in, it will send a ping (a signal to check the connection) every 20 seconds. This delay is set by a defined macro [PING_DELAY](https://github.com/iotivity/iotivity-lite/blob/6ffd7aa0a259a59632de1e491131db242a62938a/api/cloud/oc_cloud_manager.c#L43).
After sending a ping, the device will wait for a response (pong) for 1 second. If a response is not received within this time, the delay between pings will be increased to 4 seconds. If the device fails to receive a response for six consecutive pings, it will initiate a reconnect process.

## CoAP Gateway KeepAlive to Device

The CoAP gateway also has a keepalive function to check the connection with the device. The interval for sending pings and the timeout for waiting for a response (pong) can be configured through a variable in the CoAP gateway's helm settings [coapgateway.apis.coap.keepAlive.timeout](https://github.com/plgd-dev/hub/blob/0b040c1b83d08429519f1c968f045943fb358fd5/charts/plgd-hub/values.yaml#L848). By default, this interval is set to 20 seconds. If the CoAP gateway does not receive any data from the connection for the set ping interval, it will send a ping to the device. If the device does not respond within the timeout period (`coapgateway.apis.coap.keepAlive.timeout`), it will be disconnected.
