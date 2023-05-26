---
title: 'KeepAlive'
description: 'How keepalive works with plgd hub'
date: '2023-01-10'
categories: [features]
keywords: ['keepalive', 'timeout', 'iotivity-lite', 'coap-gateway']
weight: 40
---

## Device (iotivity-lite) KeepAlive to CoAP Gateway

Once a device is connected to the coap-gateway and signed in, it sends a ping signal every `20` seconds to verify the connection. After sending a ping, the device waits for a response (known as pong) for `4` seconds. If no response is received during this time, the delay between pings is decreased to `4` seconds. If the device misses six consecutive pings without any response, it will initiate a [reconnect process](/docs/architecture/component-overview/#reconnect-a-device). The behavior of this process can be configured through the set callback function using [oc_cloud_set_keepalive](http://iotivity.org/iotivity-lite-doxygen/oc__cloud_8h.html#ac24bd82a6c24b565b3f5509dcee43519).

## CoAP Gateway KeepAlive to Device

The CoAP gateway also has a keepalive function to check the connection with the device. The interval for sending pings and the timeout for waiting for a response (pong) can be configured through a variable in the CoAP gateway's helm settings [coapgateway.apis.coap.keepAlive.timeout](https://github.com/plgd-dev/hub/blob/0b040c1b83d08429519f1c968f045943fb358fd5/charts/plgd-hub/values.yaml#L848). By default, this interval is set to 20 seconds. If the CoAP gateway does not receive any data from the connection for the set ping interval, it will send a ping to the device. If the device does not respond within the timeout period (`coapgateway.apis.coap.keepAlive.timeout`), it will be disconnected.
