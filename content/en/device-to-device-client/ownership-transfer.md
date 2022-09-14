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

{{% note %}}
With the default configuration, the plgd d2d client provisions the device with the pre-shared key, generated during the startup. This one is then used for every device.
{{% /note %}}

### Own device

The plgd d2d client can be initialized in 3 different ways, for each of them the own device setup credential different:

- [Devices and single client](../client-initialization/#devices-and-single-client)

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

- [Client requests the identity certificate on it's own](../client-initialization/#client-requests-the-identity-certificate-on-its-own)

In the D2D client service, the identity certificate is setup for the device and signed by the certificate authority.

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

- [User agent mediates CSR from the client](../client-initialization/#user-agent-mediates-csr-from-the-client)

The D2D client service sets up the identity certificate to be signed by the certificate authority to the device, but the signing process is handled by the D2D client Web App.

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
