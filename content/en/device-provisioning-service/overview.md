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


Provisioning milions of devices in a secure and scalable manner without reqrequiering any human interaction is what our plgd Device Provisioning Service (DPS) is capable of. Doesn't matter if you're running the plgd hub on-premise, off-premise or a fully managed instance, the DPS makes sure it trusts only your devices and provision them to the right plgd hub instance. Exactly no human interaction, zero-touch to provision and pre-configure milions of devices just-in-time and securely, using manufacturer certificates or TPMs.

{{% note %}}
  There are [many provisioning scenarios supported by the plgd ecosystem](https://plgd.dev/features/provisioning/). Make sure you understand all the posibilities to find a best fit for your use-case.
{{% /note %}}

## Provisioning process

Securely provisioned device by the DPS precedes 3 distinct steps:
- The **manufacturing step** in which the device is prepared at the factory
- The **operation setup step** in which the DPS is configured for automated provisioning
- The **provisioning step** in which the device is enrolled after it's manufacturer identity was verified

### Manufacturing step

To successfuly attestate the device's identity during the provisioning process, a manufacturer certificate or/and a TPM need to be available on the device. It's a key to verify if the device is trusted and belongs to the enrollment group created in the DPS.
Creation of the device's manufacturer identity usually occurs towards the end of the manufacturing process. At this point, hardware assembly is complete and the initial software has been loaded. A _manufacturing PC_ is connected to the device and requests from the PKI a unique manufacturer certificate using the unique device identifier (e.g. serial number, depends on the manufacturer). In case the device contains the TPM, _manufacturing PC_ stores the TPM's endorsment key, required to for it's individual enrollment.
{{% note %}}
Process described above varies from manufacturer to manufacturer. Nevertheless, the unique device's manufacturer identity is expected as the result. Additionaly, formulation "Creation of the device's manufacturer identity" was used to not confuse the reader if using more accurate term - "Provisioning of the device's manufacturer identity".
{{% /note %}}
DPS does not introduce a new step in the manufacturing process. It ties into the existing step creating the unique identifying key information - the device's manufacturer certificate or TPM's endorsment key used for device attestation during the zero-touch provisioning by the DPS.

### Operation setup step

[Creating an Enrollment Group]() in your plgd DPS instance is required in order to make sure the DPS can properly attest to the device's identity when it comes looking for its provisioning configuration. The operator is responsible to include the identifying key information described in the manufacturer step and add it to the enrollment group. Part of the enrollment group configuration is among others also initial resource and [ACL]() configuration, useful for the [device-to-device scenarios]() where the plgd hub is not involved at all. Once the enrollment group is configured, DPS is ready to automatically provision devices. Unless the use-case or list of devices changes, the DPS service / enrollment group does not have to be modified. Number of enrollment groups is not limited; different set of devices can be attestated by different enrollment groups containing different provisioning configuration.

### Provisioning step

The Enrollment Group created during the operation setup state has a set of required and optional configuration options, supporting various use-cases or security requirements. The following diagram describes what goes on behind the scenes to get a device securely provisioned. This flow is dependent on the Enrollment Group configuration, which is a prerequisite.

{{< plantuml id="resource-publish" >}}
@startuml
autonumber
skinparam backgroundColor transparent
hide footbox

actor "Solution Operator" as op
collections "Devices" as d
participant "Device Provisioning\nService" as dps
participant "plgd hub" as hub

note over op, hub
Enrollment Entry including Devices key identifiers was created
end note
d -> dps: Connect and authenticate
activate d
dps -> dps: Find matching Enrollment Group
activate dps
d -> dps: Send Identity Certificate Signing Request
d -> dps: Retrieve ACL configuration
d -> d: Device ownership transfered
d -> dps: Get access token for plgd hub API
d -> dps: Get initial configuration
deactivate dps
deactivate d
d -> hub: Connect and authenticate using Identity Certificate
activate d

@enduml
{{< /plantuml >}}

1. The device contacts the DPS endpoint configured by the device application or by a tool which discovered the device or preset at the factory. Devices proves it's identity using a Manufacturer Certificate or TPM's endorsment key.
2. The DPS finds DOS finds the matching Enrollment Group.
3. The device issues Certificate Signing Request for the unique device Identity and requests the DPS to sign it. The CSR is signed using the default or by the operator configured Identity CA.
4. The device requests resources' ACLs for Device-to-Device as well as Device-to-Cloud communication.
5. The device starts the provisioning process by setting the device ownership, securely storing the Identity certificate and applying ACLs.
6. The device retrieves connection configuration and OAuth2.0 access token which authorizes the device to connect with the plgd hub APIs.
7. If the operator provided initial configuration for device resources, the devices retrieves it and applies.
8. The devices disconnects from the DPS and connects to the configured plgd hub instance, authenticates and encrypts the session using Identity Certificate and authorizes using the access token.


