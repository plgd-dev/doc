---
title: 'Client Initialization'
description: 'How to initialize the client?'
date: '2022-08-25'
lastmod: '2022-08-25'
categories: [d2d, client]
keywords: [client, initialization, identity]
menu:
  docs:
    parent: device-to-device-client
    weight: 20
toc: true
---

## Client Application Ownership

There are more ways on how to setup authentication between devices, clients and plgd hub, as various deployment models are supported. It's all about distribution of right credentials. Same applies for the client applications, which needs to authenticate to communicate with devices.

### Deployment scenarios
There are more options on how to initialize the client and provision it with the right security credentials. All depends on the deployment scenarios and use-cases. Let's go through 2 deployment scenarios:

#### Devices and single client

**Requirements:**

- Devices are owned and provisioned by a single client installation
- Devices are accessed by single client installation
- No centralized identity provider is in place

The device client can be your custom application using a [device/client Go library](https://github.com/plgd-dev/device/tree/main/client), or a [plgd d2d client](https://github.com/plgd-dev/client-application). In such a scenario, the client generates (or it might be preconfigured) credentials for the device it's going to provision. The client needs to persist these credentials securely (a pre-shared key). In case credentials are lost (reinstallation, disk failure), a factory reset on the device need to be executed; using it's hardware button or other device native mechanism. It's API is not accessible, as the client cannot authenticate.

{{% note %}}
This is the default configuration of the plgd d2d client. Pre-shared key is generated during startup and no authentication to the plgd d2d client is in place.
{{% /note %}}

![](/images/device-to-device-client/device-client.drawio.svg)

#### Devices, plgd hub and 1..n clients

**Requirements:**

- Devices are provisioned and optionally onboarded to the plgd hub
- Ownership transfer is done either by the DPS or [by the client](../ownership-transfer)
- Device ownership is centralized, driven by the identity provider
- Devices can be accessed remotely, through the plgd hub API
- Devices can be discovered and configured locally using a client

This scenario allows all clients which are part of the same security domain as well as the plgd hub to communicate securely with devices. Goal is to have all devices, clients as well as plgd hub trusting each other. Comparing to previous use-case, where the client issues it's own pre-shared key, this use-case requires distribution of Identity Certificates to all entities (devices, plgd hub, clients). The Identity Certificate, used for authentication identifies users / customers and devices. IDs of these entities are known as `owner IDs`.

{{% note %}}
If your deployment scenario requires plgd hub and 1..n clients, read further to understand how to provision client with the Identity Certificate.
{{% /note %}}

![](/images/device-to-device-client/device-hub-client-cert.drawio.svg)

### Client credential provisioning

The Identity Certificate is issued by the Certificate Authority service, which is part of the plgd hub deployment. Default certificate can be delivered with the hub deployment package or the operator can use custom certificate. This certificate is used for authentication as well as data encryption between plgd hub, clients and devices.

But how to provision the client with the Identity Certificate, issued by the Certificate Authority service?

{{% note %}}
What is the security domain? All entities, the plgd hub, d2d client or devices, which have the Identity Certificate issued by the same Certificate Authority are considered to be in the same security domain.
{{% /note %}}

##### Client requests the identity certificate on it's own

This option requires that the [plgd d2d client](https://github.com/plgd-dev/client-application) or other application using [device/client Go library](https://github.com/plgd-dev/device/tree/main/client), can access plgd hub API. The client is required to issue the CSR and send it to the certificate signing service to get the Identity Certificate. After retrieving the certificate and initialization, the client becomes the member of the same security domain. This allows him to successfuly authenticate and interact with any device which is member of that security domain.

{{< plantuml id="direct-csr" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

actor U as "User"
box "D2D Client"
participant S as "SPA\n(running in the browser)"
participant C as "Service\n(local or remote host)"
end box

box "plgd hub"
participant CA as "Certificate Authority"
participant OA as "OAuth 2.0 Server"
end box

U -> C ++: Open Web App
activate U
C -> S: Serve UI
deactivate C
activate S
S -> U: Request plgd hub endpoint
U -> S: plgd hub endpoint

S -> OA ++: Request authorization code
OA -> U: Provide credentials
U -> OA: Username and password
return Authorization code

S -> OA ++: Exchange for the user token
return User token

S -> C ++: Initialize\n(User token)
C -> CA ++: Sign D2D Client Identity Certificate\n(CSR, User token)
return Identity Certificate chain
C -> C: Initialize
return Initialized
S -> U: Initialized

U -> S: Discover devices
S -> C ++: Discover devices
@enduml
{{< /plantuml >}}

#### User agent mediates CSR from the client

In case the [plgd d2d client](https://github.com/plgd-dev/client-application) or other application using [device/client Go library](https://github.com/plgd-dev/device/tree/main/client) cannot access the plgd hub API directly (e.g. it's running in a different network as a web service), but the user's PC can, the CSR can be propagated through the browser to the plgd hub API and the response from the certificate authority service can be returned back over the browser to the client. This flow delivers the Identity Certificate to the client, without private key leaving the client. The client can communicate securely with any devices which is part of the same security domain right after receiving the Identity certificate and its initialization.

##### Redirect Flow

{{< plantuml id="mediated-csr-redirect" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

actor U as "User"
box "D2D Client"
participant S as "SPA\n(running in the browser)"
participant C as "Service\n(local or remote host)"
end box

box "plgd hub"
participant CA as "Certificate Authority"
participant OA as "OAuth 2.0 Server"
end box

U -> C ++: Open Web App
activate U
C -> S: Serve UI
deactivate C
activate S
S -> U: Request plgd hub endpoint
U -> S: plgd hub endpoint

S -> C ++: Initialize\n(plgd hub endpoint)
C -> S: Sign D2D Client Identity Certificate\n(Redirect to hub with CSR, Client redirect url)
S -> CA ++: Sign D2D Client Identity Certificate\n(Redirect to hub with CSR, Client redirect url)
CA -> S++: Authenticate\n(Redirect to OAuth 2.0 Server)
S -> OA ++: Request authorization code
OA -> U: Provide credentials
U -> OA: Username and password
return Authorization code\n(Redirect to Certificate Authority)
return Authorization code\n(Redirect to Certificate Authority)
CA -> OA ++: Exchange for the user token
return User token
CA -> CA: Sign D2D Client Certificate\n(CSR, User token)
return Identity Certificate chain\n(Redirect to D2D Client)
S -> C: Initialize\n(Identity Certificate Chain)
return Initialized
S -> U: Initialized
U -> S: Discover devices
S -> C ++: Discover devices
@enduml
{{< /plantuml >}}

##### JS driven flow

{{< plantuml id="mediated-csr-js" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

actor U as "User"
box "D2D Client"
participant S as "SPA\n(running in the browser)"
participant C as "Service\n(local or remote host)"
end box

box "plgd hub"
participant CA as "Certificate Authority"
participant OA as "OAuth 2.0 Server"
end box

U -> C ++: Open Web App
activate U
C -> S: Serve UI
deactivate C
activate S
S -> U: Request plgd hub endpoint
U -> S: plgd hub endpoint

S -> OA ++: Request authorization code
OA -> U: Provide credentials
U -> OA: Username and password
return Authorization code

S -> OA ++: Exchange for the user token
return User token

S -> C ++: Request Certificate Signing Request
return Certificate Signing Request

S -> CA ++: Sign D2D Client Identity Certificate\n(CSR, User token)
return Identity Certificate chain


S -> C ++: Initialize\n(Identity Certificate Chain)
return Initialized
S -> U: Initialized
U -> S: Discover devices
S -> C ++: Discover devices
@enduml
{{< /plantuml >}}