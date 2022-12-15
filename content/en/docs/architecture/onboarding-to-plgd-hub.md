---
title: 'Onboarding to plgd hub'
description: 'How to onboard the device to the plgd hub?'
date: '2022-12-13'
lastmod: '2022-12-13'
categories: [architecture, d2c, provisioning, onboarding]
keywords: [architecture, d2c, provisioning, onboarding, oauth]
menu:
  docs:
    parent: device-to-device-client
    weight: 40
toc: true
---

Device can be remotely accessed when it's onboarded and connected to the plgd hub. The connection between the device and the hub is a secure CoAP over TCP/UDP connection. Combination of JWT token and Identity certificate guarantees zero trust security on this end-to-end integration.
To onboard the device to the plgd hub, following configuration data are needed:

- coapGatewayAddress - address of the plgd hub in the format `<scheme>://<host>:<port>`
- authorizationCode - [OAuth2.0 authorization code](https://www.rfc-editor.org/rfc/rfc6749#section-4.1), used to get the device JWT token
- authorizationProviderName - authorization provider which is used to authorize the device on the plgd hub
- hubID - id of the plgd hub
- certificateAuthorities - trusted certificates used to verify the authenticty of the plgd hub

{{< warning >}}
Only the device owner, [Zero-touch Provisioning service](../../device-provisioning-service/overview/) or an authorized client are allowed to provision the device with all the configuration data required for the device to cloud connectivity.
{{< /warning >}}

The [device onboarding process](../../architecture/component-overview/#device-onboarding) starts right after successful provisioning.

## How to onboard?

To enable device to cloud connectivity, following tasks need to be successfuly completed:

- Certificate Authority configured so the device can successfuly verify authenticity of the plgd hub
- ACLs are configured so the device authorizes incoming requests from the plgd hub
- The `hub onboarding configuration` is set so the device knows where to connect and how to authorize the connection

{{< note >}}
These steps are reduced to one click when using [Device to Device Client](../../device-to-device-client/overview/) or fully automated when using [Zero Touch Provisioning](../../device-provisioning-service/overview/).
{{< /note >}}

### Certificate Authority configuration

Certificates can be configured only in the provisioning state. More information about this process can be found [here](../../tutorials/change-provision-status).

When device is in provisioning state we can add the certificate authority to the device through credentials resource with body:

```json
{
  "creds": [
    {
      "subjectuuid": "plgd/hub ID",
      "credtype": 8,
      "credusage": "oic.sec.cred.trustca",
      "publicdata": {
        "data": "-----BEGIN CERTIFICATE-----\n...\n-----END CERTIFICATE-----\n",
        "encoding": "oic.sec.encoding.pem"
      }
    }
  ]
}
```

{{< note >}}
More information about the credentials resource can be found [here](https://openconnectivity.org/specs/OCF_Security_Specification.pdf) in section 9.
{{< /note >}}

{{< plantuml id="add-ca" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

box "D2D Client"
participant S as "Web App\n(running in the browser)"
participant C as "Service\n(local or remote host)"
end box

participant D as "Device\n(in the local network with Service)"

S -> D++: Update credentials\n(POST /oic/sec/cred {"creds": [...]})
activate S
return Credentials updated
deactivate S

@enduml
{{< /plantuml >}}

### Device ACLs

As the device acts as a server, it uses the concepts of ACLs for the client authorization. More information about ACLs and how to configure them can be found [here](#jozo prosim docku s vysvetlenim co su acl, diagramom ako sa dostat do toho stavu[toto zasa asi separatna docka na ktoru sa odkazes aj z tohto aj predosleho stepu] a ako nastavit ACL].

### Hub Onboarding Configuration
Each device hosts the `coapcloudconf`resource where the `hub onboarding configuration` has to be set. Resource definition can be found [here](https://github.com/openconnectivityfoundation/cloud-services/blob/master/swagger2.0/oic.r.coapcloudconf.swagger.json#L215). Setting the resource with required values will trigger the onboarding process, which is in detail described [here](../../architecture/component-overview/#device-onboarding.

## Connection authentication and encryption

Connection to the plgd hub is secured using a X.509 certificate. If the device mutually authenticates with the cloud and what certificate is used depends on credentials which are available on the device. Following table gives you more information if mTLS can be enabled and which certificate is used for the connection.


| Manufacturer Certificate | Identity Certificate  | mTLS to plgd hub possible   | Device authenticates using |
| ------------------------ | --------------------- | --------------------------- | -------------------------- |
| no                       | no                    | no                          | mTLS not possible          |
| yes                      | no                    | yes                         | manufacturer certificate   |
| yes/no                   | yes                   | yes                         | identity certificate       |

{{< note >}}
Manufacturer certificate is flashed (or contained within the TPM)) on the device during the production. The Identity Certificate is one option of device credentials set during the device ownership transfer. Other is PSK, that's why Identity Certificate doesn't have to be present to connect to the plgd hub. 
{{< /note >}}

If the plgd hub requires mTLS authentication, the device needs to have the identity/manufacturer certificate signed by the certificate authority which is known in the plgd hub.

### How to onboard the device to the plgd hub

Before the device can be provisioned, the authorization code must be acquired and device need to be owned. The authorization code is used to authorize the device on the plgd hub.

{{< plantuml id="onboard-device" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

box "D2D Client"
participant S as "Web App\n(running in the browser)"
participant C as "Service\n(local or remote host)"
end box

participant D as "Device\n(in the local network with Service)"

box "plgd hub"
participant OA as "OAuth 2.0 Server"
end box

S -> OA++: Get authorization code\n(deviceID, clientID, scopes, audience)
activate S
return Authorization code
S -> D++: Onboard to plgd hub\n(coapGatewayAddress, authorizationCode, authorizationProviderName, hubID, certificateAuthorities)
return Device starts to connect to the plgd hub
loop until device is not in registered/fail state
    S -> D: get cloud provision status
end
deactivate S

@enduml
{{< /plantuml >}}

When device provisioning ends with fail state, the device is not registered on the plgd hub. The device needs to be provisioned again.

#### State machine for the device "CloudProvisioningStatus"

{{< plantuml id="cloud-provision-status" >}}
@startuml
skinparam state {
  BackgroundColor<<Shadow>> LightGray
}
skinparam backgroundColor transparent
hide footbox

state NOT_OWNED <<Shadow>> {
  state unknown {
  }
}

state OWNED <<Shadow>> {
  state uninitialized: Device is owned without onboard configuration
  
  state ONBOARDING_IN_PROGRESS <<Shadow>> {
    state registering: Try sign up/sign in to the plgd hub
    state registering: Connection lost - reconnect
    state ready_to_register: Device is owned with onboard configuration
  }

  state failed: Error occurs during registering
  state failed: Refresh token failed

  state ONBOARDED <<Shadow>> {
    state registered: Device is registered at the plgd hub
  }
}

unknown --> uninitialized : Own device
OWNED --> unknown : Disown device
uninitialized --> ready_to_register: Provision device to plgd hub

ready_to_register -[dotted,#gray]-> registering :  sign up/sign in to the plgd hub
registering -[dotted,#gray]-> failed : error
registering -down[dotted,#gray]-> registered : success
registered -up[dotted,#gray]-> registering  : connection lost
registered -[dotted,#gray]-> failed: Refresh token failed
failed -[dotted,#gray]-> registering: retry
@enduml
{{< /plantuml >}}

### How to offboard the device from the plgd hub

To offboard the device from the plgd hub, the device can be in the any state. Device can still exist in the plgd hub after the offboarding process, because deregistration can fail. In this case, the device need to be [deleted from the plgd hub](https://github.com/plgd-dev/hub/blob/main/http-gateway/swagger.yaml#L52).

{{< plantuml id="offboard-device" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

box "D2D Client"
participant S as "Web App\n(running in the browser)"
participant C as "Service\n(local or remote host)"
end box

participant D as "Device\n(in the local network with Service)"

S -> D++: Offboard from plgd hub
return Device starts process offboarding from the plgd hub
loop until device is not in uninitialized state
    S -> D: get cloud provision status
end
deactivate S

@enduml
{{< /plantuml >}}
