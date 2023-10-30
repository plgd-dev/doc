---
title: 'Device Provisioning Service'
description: 'What is plgd Device Provisioning Service?'
search: 'Device Provisioning Service test search'
date: '2022-01-26'
categories: [zero-touch, provisioning]
keywords: [provisioning, TPM, manufacturer-certificate]
weight: 1
isMainSectionPage: true
---

Provisioning millions of devices in a secure and scalable manner without requiring any human interaction is what our plgd Device Provisioning Service (DPS) solves. Doesn't matter if you're running the plgd hub on-premise, off-premise or a fully managed instance, the DPS makes sure it trusts only your devices and provision them to the right plgd hub instance. Exactly no human interaction, zero-touch to provision and pre-configure millions of devices just-in-time and securely, using manufacturer certificates or TPMs.

## When to use Device Provisioning Service

plgd ecosystem supports various provisioning scenarios, including Zero-touch provisioning handled by the DPS. It's your perfect choice when you try to solve:

- Provision huge number of devices without hard-coding plgd hub connection
- Distribute devices across different plgd hub instances based on custom identifier (e.g. serial number)
- Distribute devices to customer specific environments while securing the communication using customer's custom certificate
- Distribute devices based on a use-case to support solution isolation use-cases
- Drive custom ACL and Identity Certificate for devices based on a security requirements or use-cases
- Reprovision the device when it is not able to connect to the plgd hub
- Blacklist devices which were compromised, discontinued, ...
- Certificate rotation
- ... and more

## Provisioning process

Securely provisioned device by the DPS precedes 3 distinct steps:

- The **manufacturing step** in which the device is prepared at the factory
- The **operation setup step** in which the DPS is configured for automated provisioning
- The **provisioning step** in which the device is enrolled after it's manufacturer identity was verified

### Manufacturing step

To successfully attest the device's identity during the provisioning process, a manufacturer certificate or/and a TPM need to be available on the device. It's a key to verify if the device is trusted and belongs to the enrollment group created in the DPS.
Creation of the device's manufacturer identity usually occurs towards the end of the manufacturing process. At this point, hardware assembly is complete and the initial software has been loaded. A _manufacturing PC_ is connected to the device and requests from the PKI a unique manufacturer certificate using the unique device identifier (e.g. serial number, depends on the manufacturer). In case the device contains the TPM, _manufacturing PC_ stores the TPM's endorsement key, required for it's individual enrollment.
{{< note >}}
Process described above varies from manufacturer to manufacturer. Nevertheless, the unique device's manufacturer identity is expected as the result. Additionally, formulation "Creation of the device's manufacturer identity" was used to not confuse the reader if using more accurate term - "Provisioning of the device's manufacturer identity".
{{< /note >}}
DPS does not introduce a new step in the manufacturing process. It ties into the existing step creating the unique identifying key information - the device's manufacturer certificate or TPM's endorsement key used for device attestation during the zero-touch provisioning by the DPS.

### Operation setup step

[Creating an Enrollment Group]() in your plgd DPS instance is required in order to make sure the DPS can properly attest to the device's identity when it comes looking for its provisioning configuration. The operator is responsible to include the identifying key information described in the manufacturer step and add it to the enrollment group. Part of the enrollment group configuration is among others also initial resource and [ACL]() configuration, useful for the [device-to-device scenarios]() where the plgd hub is not involved at all. Once the enrollment group is configured, DPS is ready to automatically provision devices. Unless the use-case or list of devices changes, the DPS service / enrollment group does not have to be modified. Number of enrollment groups is not limited; different set of devices can be attested by different enrollment groups containing different provisioning configuration.

### Provisioning step

The Enrollment Group created during the operation setup state has a set of required and optional configuration options, supporting various use-cases or security requirements. The following sequence diagram describes what goes on under the hood to get a device securely provisioned. This flow is dependent on the Enrollment Group configuration, which is a prerequisite.

{{< plantuml id="dps-overview" >}}
@startuml
skinparam backgroundColor grey
hide footbox

actor "Solution Operator" as op
collections "Devices" as d
participant "Device Provisioning\nService" as dps
participant "plgd hub" as hub

note over op, hub
Enrollment Entry including Devices key identifiers was created
end note
d -> d: **1** Initiate device ownership transfer
activate d
d -> dps: **2** Connect and authenticate
dps -> dps: **3** Find matching Enrollment Group
activate dps
d -> dps: **4** Get DPS service time to sync clocks
dps --> d
d -> dps: **5** Get owner of the device
dps --> d
d -> dps: **6** Send Identity Certificate Signing Request
dps --> d
d -> dps: **7** Retrieve ACL configuration
dps --> d
d -> dps: **8** Get initial configuration
dps --> d
d -> dps: **9** Get plgd hub connection configuration
dps --> d
d ->x dps: Disconnect
deactivate dps
deactivate d
d -> hub: **10** Connect and authenticate
activate d

@enduml

{{< /plantuml >}}

1. The device application or external tool which discovered the device using CoAP multicast requests the DPS Client to start the provisioning process by setting the DPS endpoint. The DPS endpoint can be also configured at the factory.
2. The device opens the connection to the DPS and proves it's identity using a Manufacturer Certificate or TPM's endorsement key.
3. The DPS finds the Enrollment Group with matching Manufacturer Certificate CA or TPM's endorsement key.
4. In order to validate TLS certificates and rotate them, the device synchronizes its clock with the DPS by obtaining the current time.
5. The device's owner is granted access by the DPS, enabling the device to operate in Device-to-Device scenarios.
6. The device issues Certificate Signing Request (CSR) for the unique device Identity and requests the DPS to sign it. The CSR is signed by the separate service, running next to the DPS or within the plgd hub deployment. Custom Identity CA can be used. Identity Certificate is then securely stored on the device and used to for unique identification and secure connection to the plgd hub.
7. The device requests resources' ACLs for the Device-to-Device as well as Device-to-Cloud communication and applies them.
8. If the operator provided initial configuration for device resources, the devices retrieves and applies it.
9. The device retrieves connection configuration and OAuth2.0 access token which authorizes the device to communicate with the plgd hub APIs.
10. The device connects to the configured plgd hub instance, authenticates and encrypts the session using Identity Certificate and authorizes using the OAuth2.0 access token.

{{< note >}}
Step number 4 and 10 are optional.

4. Time synchronization is not required when device is synchronized time by another method like NTP. In such a case, the device can skip this step.
10. Device provisioning doesn't require to connect the device to the plgd hub. In such a case, device is ready to be securely used for your Device-to-Device scenarios.

{{< /note >}}
