---
title: 'Domain Overview'
description: 'plgd domain overview'
date: '2021-05-13'
categories: [architecture]
keywords: [domain]
weight: 1
---

> The **Internet of Things (IoT)** is the network of physical devices, which are embedded with electronics, software, sensors, actuators, and connectivity for the purpose of enabling these things to connect and exchange data. Thus creating opportunities for more direct integration of the physical world into computer-based systems, resulting in efficiency improvements, economic benefits and reduced human intervention. _(Wikipedia, Internet of Things)_

An IoT device senses change, representing the world around it. The developer determines how the world is represented through the device and processed by the application for your specific domain. Technology should support the transfer of data in a standardized and secured way. The IoT platform should not limit you nor set boundaries – this would limit the evolution of your system.

```json
{
  "menu": {
    "id": "file",
    "value": "File",
    "popup": {
      "menuitem": [
        {"value": "New", "onclick": "CreateNewDoc()"},
        {"value": "Open", "onclick": "OpenDoc()"},
        {"value": "Close", "onclick": "CloseDoc()"}
      ]
    }
  }
}
```

**Domain** is a sphere of knowledge, influence, or activity.

IoT itself is most likely not a domain of your business; it is a group of technology achievements from the last decades of the 20th century, which is now opening doors to new possibilities for your business domain. IoT facilitates modelling of the external world specific to your needs, in the form of resources and events, which are then transferred in a secure and traceable manner to your application, located off-premise or even on-premise. The goal is for developers to focus primarily on the domain of their business.

## Architectural Drivers

### Technical Contraints

- **CoAP**
    OCF mandates [CoAP](https://coap.technology/) support for compliant [devices](https://github.com/iotivity/iotivity-lite).
- **CoAP over TCP**
    While UDP may be preferred for messaging over local networks where "chattiness" is highly detrimental due to power or bandwidth constraints, CoAP over TCP is preferred for situations where a device is communicating with a remote server, due to the greater QoS guarantees; and TCP has substantially better support than UDP in cloud-native use cases.
- **TLS**
    is responsible for security and data integrity between the new component and a connecting device.
- **CBOR**
    Default media type used in communication between [OCF compliant devices](https://github.com/iotivity/iotivity-lite) and components is [CBOR](https://cbor.io/). This format must be supported by default.

### Quality Attributes

- **Scalable**
    Ericsson forecast that there will be around 18 billion IoT devices online in 2022. The system needs to be able to not only handle large scale connectivity and load, but also rapidly scale up and down in response to load.
- **High Availability**
    IoT devices are often crucial to the safety and performance of the system they’re used in. While these devices may be in inherently low QoS environments, it is the responsibility of the hub to always be available for when the devices need it; and otherwise not be the weakest link.
- **Traceable**
    Many users and devices transceiving data at the same time can result in many types of errors, negatively impacting business operations. It is beneficial to track activities within the system for more effective error solving and future prediction based on recognised patterns.
- **Cost Efficient**
    Many future users won't have the knowledge about infrastructure and operations of the whole system. They might not have their own data center for hosting of the solution. This increases the importance of ease of use and cost efficiency. Most cloud providers offer a similar set of services from a functional point of view. A solution should be able to take advantage of these services to save money, alleviating the burden of missing know-how and increasing runtime optimizations.
- **Multitenant**
    Solution providers which have multiple customers should have the ability to use "one" instance of the system for all customers in a secured way. It is important that a client is only able to access the devices it’s authorized to access.

## Domain Decomposition

### Resources Bounded Context

Servers (IoT Devices) which are OCF enabled are represented in the form of **resources**. (similar to REST)  Resources are hosted by a server (IoT Device) and if it is connected to the [plgd hub](https://github.com/plgd-dev/hub/), it is able to publish those resources, which should then be accessible "remotely" through this decentralized component. That means, the [plgd hub](https://github.com/plgd-dev/hub/) works both as the gateway and the resource directory for all connected and authorized servers / clients.

> To understand more about what a resource is, read chapter 7 - [Resource model](https://openconnectivity.org/specs/OCF_Core_Specification.pdf)

**Connected server / client can:**

- Publish / Unpublish resources
   - A resource is represented by a URI and properties (resource types and interfaces)
- Browse Resources
   - Browse resources published by servers to the Resource Directory
- Retrieve Resource
   - Resource Bounded context keeps up-to-date representation of each remote resource
- Update resource representation
   - Resource Bounded context propagates each update to the device's resource
- Observe Resource
   - Each change of the resource creates an event to which client or device can be subscribed

### Identities Bounded Context

Only authorized client _(application interested in data)_ connected to the plgd hub _(IoT Device)_ is able to perform an action on the device or access device's data. That means, only authorized client and server are able to browse / CRUDN resource published to the Resource Directory.

A server and client are required to successfully sign-up and sign-in right after connecting to the [plgd hub](https://github.com/plgd-dev/hub/). During the sign-up process, which can be thought of as a registration, a one time use [authorization code](https://tools.ietf.org/html/rfc6749#section-1.3.1) is exchange for an access token, which uniquely represents this server / client. Returned access token is used in the sign-in request. Before the server / client is signed in, requests are not forwarded to the plgd system.

The connected server / client belongs to the user who requested the authorization code.
**Connected server / client can:**

- Sign-up
   - Registration to the plgd hub with a valid authorization code
- Sign-in
   - Authorize connection with provided access token
- Sign-out
