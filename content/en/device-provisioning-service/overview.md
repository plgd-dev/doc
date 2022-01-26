---
title: 'Overview'
description: 'What is plgd Device Provisioning Service?'
date: '2021-01-26'
lastmod: '2021-01-26'
categories: [zero-touch, provisioning]
keywords: [provisioning, TPM, manufacturer-certificate]
menu:
  docs:
    parent: device-provisioning-service
    weight: 50
toc: true
---

If you're running plgd hub on-premise, off-premise or it's fully managed by us, our Device Provisioning Service (DPS) enables you to securely provision your trusted devices to the right plgd hub. Exactly no human interaction, zero-touch to provision and pre-configure milions of devices just-in-time and securely, using manufacturer certificates or TPMs.

## Behind the scenes

The DPS's main entity is the Entrollment entry, which might be a Group Enrollment or an Individual Enrollment. Each of this entry has a set of required and optional configuration options, supporting various use-cases or security requirements. The following diagram describes what goes on behind the scenes to get a device provisioned. The first step requires manual interaction with the DPS service.

{{< plantuml id="resource-publish" >}}
@startuml
autonumber
skinparam backgroundColor transparent
hide footbox

actor "Device Manufacturer" as dmf
collections Devices as d
participant "Device Provisioning\nService" as dps
participant "plgd hub" as hub

dmf -> dps: Create new Enrollment entry

d -> dps: Connect and authenticate
activate d
d -> dps: Send Identity Certificate Signing Request
d -> dps: Retrieve ACL configuration
d -> d: Provisioning for Device-to-Device communication
deactivate d
d -> dps: Connect and authenticate using Identity Certificate
activate d
d -> dps: Get access token for plgd hub API
d -> dps: Get initial configuration
deactivate d
d -> hub: Connect and authenticate using Identity Certificate
activate d

@enduml

{{< /plantuml >}}
1. Device manufacturer, vendor or end-customer (DPS Admin) creates group or individual enrollment in the Device Provisioning Service (DPS)
2. The device contacts the DPS endpoint configured by the device application or set at the factory. Devices proves it's identity using Manufacturer Certificate or TPM and the DPS Service finds the matching enrollment group.
3. The device issues Certificate Signing Request for the unique device Identity and requests DPS to sign. The CSR is signed using the default or by the DPS Admin configured Identity CA / Intermediate certificate.
4. The device requests resources ACLs for Device-to-Device as well as Device-to-Cloud communication.
5. The device starts the provisioning process by configuring the device ownership, securely storing the Identity certificate and applying ACLs.
6. The device reconnects to the DPS using the Identity Certificate. The data transport is encrypted using the Identity Certificate, trusted by the DPS Admin.
7. The device retrieves the OAuth2.0 access token and related configuration which authorizes the device to interact with the plgd hub APIs.
8. If the DPS Admin provided pre-configuration for device resources, the devices retrieves it and applies.
9. The devices disconnects from the DPS and connects to the configured plgd hub instance, encrypts the session and authenticates using the Identity Certificate as well as authorizes using the token.

