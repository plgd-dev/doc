---
title: 'CoAP Gateway'
description: 'What is plgd CoAP Gateway?'
date: '2023-06-16'
categories: [gateway, iot]
keywords: [coap, iot]
weight: 1
isMainSectionPage: true
---

The CoAP gateway serves as both a server and a client, facilitating communication with IoT devices through a single established connection. These devices act as CoAP servers, adhering to the OCF (Open Connectivity Foundation) specification for IoT devices.

The primary functions of the CoAP gateway are authentication and authorization of devices to access cloud services. Authentication is based on certificates, while authorization relies on JWT (JSON Web Token) tokens.

## Authentication

Devices connecting to the CoAP gateway must verify its certificates. The CoAP gateway offers three authentication methods:

1. OCF Identity Certificate: Devices are authenticated using an OCF identity certificate issued by the PLGD Certificate Authority. This option enforces the validation of a JWT token containing a claim with the device ID. It involves a mutual TLS (Transport Layer Security) connection, as the CoAP gateway also verifies the device certificate.
2. Any Certificate: Devices can use any certificate for authentication, including manufacturer certificates. Similar to the previous method, it requires a mutual TLS connection, but it does not check if the certificate is an OCF identity certificate.
3. Disable Authentication: All devices are allowed to connect to the IoT hub without authentication. This option is similar to accessing a bank account and is considered a TLS connection.

After authentication, the CoAP gateway proceeds to authorize the device's access to the IoT hub through the JWT token.

## Authorization

Authorization is essential for enabling communication with the IoT hub. The CoAP gateway authorizes devices to access the IoT hub using the JWT token. There are two ways in which the CoAP gateway can perform authorization:

1. Valid Token with Device ID Claim: The token must contain a claim with the device ID for successful authorization.
2. Valid Token Only: Authorization is granted based solely on the validity of the token.

This authorization is tied to the connection, meaning that if a device gets disconnected, it must be authorized again. Each command sent from the device to the IoT hub is associated with the device's JWT token.

## Synchronize Device Twin

Once device authentication is completed, the synchronization of the device twin begins. The device publishes resources, and the CoAP gateway subscribes to these resources, forwarding any changes to the IoT hub to update the device twin.

### Efficient Device Twin Synchronization using Entity Tag (ETAG)

The CoAP gateway employs the Entity Tag (ETAG) mechanism to monitor resource changes. An ETAG, which is a hash of a resource's content, is used to determine if a resource has been modified on the device. The Hub stores the ETAG for each resource along with the timestamp of the last change.

The CoAP gateway supports two types of resource observation:

- **Batch observation**: In this mode, the ETAG is associated with the overall state of resources. Whenever a change occurs, the Hub updates the ETAG for all affected resources with the same timestamp. Consequently, if the device goes offline and reconnects later, the Hub can send the stored latest ETAG of the resources to the device.
  {{< note >}}
  To use this option, the device needs to support batch observation mode. For iotivity-lite, it should be enabled via the `-DOC_DISCOVERY_RESOURCE_OBSERVABLE_ENABLED=ON` cmake option.
  {{< /note >}}
- **Per resource observation**: In this mode, the ETAG for each resource needs to be stored. Similar to batch observation, when the device goes offline and reconnects, the Hub can send the corresponding ETAG for each observed resource.

When the CoAP gateway initiates resource observation, it sends the ETAG to the device. Subsequently, when the resource undergoes changes, the device sends the updated ETAG back to the CoAP gateway.

## Process Events to the Device

During the device twin synchronization, the CoAP gateway subscribes to events from the IoT hub and translates them into CoAP commands to be sent to devices. If any event is awaiting the device's online status, the CoAP gateway immediately sends it to the device.
