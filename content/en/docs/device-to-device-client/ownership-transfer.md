---
title: 'Ownership Transfer and Provisioning'
description: 'How does security work with d2d?'
date: '2022-08-18'
lastmod: '2022-09-14'
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

Supported [credentials](https://openconnectivity.org/specs/OCF_Security_Specification_v2.2.5.pdf#page=40) which are provisioned on the device to **verify** the client are:

- Pre-shared key
- Identity Certificate

{{< note >}}
With the default configuration, the plgd d2d client provisions the device with the pre-shared key, generated during the startup. This one is then used for every device.
{{< /note >}}

### Own device

Part of the device ownership transfer is the credentials provisioning. As you know from the [d2d client initialization](../client-initialization), there are more options, depending on your setup, on how to request and provision credentials. The same requirements and flow applies also for device credential provisioning.

#### Devices and single client

{{% note %}}
More on this deployment option [here](../client-initialization/#devices-and-single-client).
{{% /note %}}

The pre-shared key is setup on the device by the D2D client service.

{{< plantuml id="own-device-devices-and-single-client" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

box "D2D Client"
participant S as "Web App\n(running in the browser)"
participant C as "Service\n(local or remote host)"
end box

participant D as "Device\n(in the local network with Service)"

S -> C ++: Own device
C -> D++: Set PreSharedKey
return
return Device owned
@enduml
{{< /plantuml >}}

#### Client requests the identity certificate on its own

{{% note %}}
More on this deployment option [here](../client-initialization/#devices-plgd-hub-and-1n-clients).
{{% /note %}}

The d2d client service requests the identity certificate from the plgd Certificate Authority for the device on its own.

{{< plantuml id="own-device-client-requests-the-identity-certificate-on-it-s-own" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

box "D2D Client"
participant S as "Web App\n(running in the browser)"
participant C as "Service\n(local or remote host)"
end box

participant D as "Device\n(in the local network with Service)"

box "plgd hub"
participant CA as "Certificate Authority"
end box

S -> C++: Own device\n(User token)
C -> D++: Get Identity CSR
return Identity CSR
C -> CA++: Sign Device Identity Certificate\n(CSR, User token)
return Device Identity chain
C -> D++: Set Identity chain\n(Device Identity chain)
return
C -> S: Device owned
deactivate C

@enduml
{{< /plantuml >}}

#### User agent mediates CSR from the client

{{% note %}}
More on this deployment option [here](../client-initialization/#devices-plgd-hub-and-1n-clients).
{{% /note %}}

The d2d client service is unable to reach the plgd Certificate Authority service. Therefore the plgd d2d web app gets the Identity CSR from the d2d client service and requests the certificate from the plgd Certificate Authority on its own, which is then sent back to the d2d client service. The web app is in the mediator role, assuming the PC where it's loaded has access to both, plgd hub as well as to the d2d client service.

{{< plantuml id="direct-csr" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

box "D2D Client"
participant S as "Web App\n(running in the browser)"
participant C as "Service\n(local or remote host)"
end box

participant D as "Device\n(in the local network with Service)"

box "plgd hub"
participant CA as "Certificate Authority"
end box

S -> C++: Own device\n(User token)
activate S
C -> D++: Get Identity CSR
return Identity CSR
C -> S: Identity certificate challenge\n(Device Identity CSR, state)
S -> CA ++:  Sign Device Identity CSR\n(CSR, User token)
return Identity Certificate Chain
S -> C: Finish Own\n(Identity Certificate Chain, User token, state)
C -> D++: Set Identity chain\n(Device Identity chain)
return
return Device owned
deactivate C
deactivate S

@enduml
{{< /plantuml >}}