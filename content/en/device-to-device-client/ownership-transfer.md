---
title: 'Ownership Transfer and Provisioning'
description: 'How does security work with d2d?'
date: '2022-08-18'
lastmod: '2022-08-18'
categories: [d2d, client]
keywords: [d2d, client, proximity, udp]
menu:
  docs:
    parent: device-to-device-client
    weight: 20
toc: true
---

Machine to machine communication is required for many interoperable scenarios, where one device needs to notify another one or where the device needs to retrieve a configuration from a different one. The plgd platform together with the IoTivity support all deployment scenarios and communication models. Easiest way how to develop machine to machine interactions, or test your device is to use the [plgd d2d client](https://github.com/plgd-dev/client-application). The plgd d2d client is using a [device/client Go library](https://github.com/plgd-dev/device/tree/main/client) and exposes it's API through [HTTP](https://github.com/plgd-dev/client-application/blob/main/pb/service.swagger.json) and [gRPC](https://github.com/plgd-dev/client-application/blob/main/pb/service.proto) endpoints. 

The built-in UI allows you to discover devices, add them by IP, own them and interact over it's resources.

## Device Discovery

Devices are discovered by sending the CoAP multicast message and waiting for the configured timeout for responses from devices, which are available in the subnet. In case you want to interact with the device which cannot be discovered using multicast, either it's not in the same subnet or the multicast is blocked, you can add the device by IP.

## Device Ownership

The plgd d2d client can become an owner of a new, unowned device. Supported [ownership transfer methods](https://openconnectivity.org/specs/OCF_Security_Specification_v2.2.5.pdf#page=48) by the plgd d2d client are:

- Just Works
- Manufacturer Certificate

Supported [credetials](https://openconnectivity.org/specs/OCF_Security_Specification_v2.2.5.pdf#page=40) which are provisioned on the device to verify the client are:

- Pre-shared key
- Identity Certificate

With the default configuration, the plgd d2d client during provisions the device with the pre-shared key, generated during the startup. This one is then used for every device.

### Device Ownership use-cases

There are more ways on how to setup authentication between devices, clients and plgd hub, as various deployment models are supported. It's all about distribution of right credentials and figuring out mechanisms which are used to provision these credentials on devices.

#### Devices and single client installation

**Requirements:**

- Devices are owned and provisioned by a single client installation
- Devices are accessed by single client installation

The device client can be your custom application using a [device/client Go library](https://github.com/plgd-dev/device/tree/main/client), or a [plgd d2d client](https://github.com/plgd-dev/client-application). In such a scenario, the client generates (or it might be preconfigured) credentials for the device it's going to provision. The client needs to persist these credentials securely (a pre-shared key, or a root certificate used to issue an identity certificate). In case credentials are lost (reinstallation, disk failure), factory reset on the device need to be triggered; using it's hardware button or other device native mechanism. It's API is not accessible, as the client cannot authenticate.

![](/images/device-to-device-client/device-client.drawio.svg)

#### Devices and plgd hub

**Requirements:** 

- Devices are provisioned and onboarded to the plgd hub, using identity certificates
- Devices are accessed only remotely, through the plgd hub API

Part of the plgd hub is the certificate authority service, which issues the identity certificates for devices. Root certificate configured by the operator is used to authenticate the device as well as plgd hub. The identity certificate is provisioned on the device during ownership transfer process. The device issues the CSR which needs to be delivered to the plgd hub certificate authority service. There are more ways how to achieve it. Either you implement your own onboarding tool, which is mediating the ownership trasnfer and provisioning process, or you use the Device Provisioning Service and the device is automatically onboarded and provisioned to the pre-configured plgd hub.

![](/images/device-to-device-client/device-hub.drawio.svg)

#### Devices, plgd hub and client

**Requirements:**

- Devices are provisioned and onboarded to the plgd hub using Identity Certificates
- Devices are owned either by the DPS or by the client
- Devices can be access remotely, through the plgd hub API
- Devices can be discovered and configured locally using a client

This scenario allows all clients which are part of the same security zone as well as the plgd hub to communicate with devices, which are owned either by the DPS or by the client. After reading previous scenarios it's now clear to you that it's all about credentials sharing.
The device owned by the DPS issues by default the identity certificate, that is stored on the device. All clients, including plgd hub (which uses the certificate signed by the same root) are able to authenticate and communicate with the device. There are three options on how to add clients to the same security zone:

##### Client requests the identity certificate on it's own

This option requires that the client, [plgd d2d client](https://github.com/plgd-dev/client-application) or other application using [device/client Go library](https://github.com/plgd-dev/device/tree/main/client), can access plgd hub API. The client is required to issue the CSR and send it to the certificate signing service to get the identity certificate. After this process, client is part of the same security zone and can successfuly authenticate and interact with any device, provisioned using the DPS.

![](/images/device-to-device-client/device-hub-client-cert.drawio.svg)

##### User agent mediates CSR from the client

In case the [plgd d2d client](https://github.com/plgd-dev/client-application) or other application using [device/client Go library](https://github.com/plgd-dev/device/tree/main/client) cannot access the plgd hub API directly (e.g. it's running in a different network as a web service), but the user's PC can, the CSR can be propagated through the browser to the plgd hub API and the response from the certificate authority service can be returned back over the rowser to the client. This flow is used also in the OAuth 2.0, known as [Authorization Code Flow](https://auth0.com/docs/get-started/authentication-and-authorization-flow/authorization-code-flow). This flow delivers the identity certificate to the client, without the private key leaving the client. The client can right after receiving the identity certificate communicate with devices in a secure way.


![](/images/device-to-device-client/device-hub-client-agent-cert.drawio.svg)

##### Pre-shared key set by the DPS

The Device Provisioning Service is a component enabling zero-touch provisioning. More information available [here](../device-provisioning-service/overview). Part of the DPS configuration is the credentials section, where the operator configures what credentials are during the zero-touch provisioning provisioned on the device. By default it's the identity certificate. If you have an option to issue the identity certificate also for your client, it's best to follow the previous paragraph. If you are looking for a different solution, the DPS can be configured to provision more credentials on the device. You can configure the DPS to generate a pre-shared key for each device as an additional credential type. In case you want to communicate with the device using a client, which has no identity certificate, you can copy this pre-shared key from the DPS and set it in your client as a credential type to communicate with the desired device.


![](/images/device-to-device-client/device-hub-client-agent-key.drawio.svg)

{{% note %}}
If you are interested in certificates, but you don't want to use single Root CA for local clients and for remote communication over plgd hub, the DPS can be configured to issue more identity certificates, signed by different Root CAs. This allows you to distribute to your clients e.g. maintenance identity certificates while your plgd hub instance uses customer one, configured by the operator.
{{% /note %}}