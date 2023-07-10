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

## Process Events to the Device

During the device twin synchronization, the CoAP gateway subscribes to events from the IoT hub and translates them into CoAP commands to be sent to devices. If any event is awaiting the device's online status, the CoAP gateway immediately sends it to the device.
