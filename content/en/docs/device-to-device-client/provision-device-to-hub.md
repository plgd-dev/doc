---
title: 'Provision device to plgd hub'
description: 'How to onboard the device to plgd hub?'
date: '2022-12-13'
lastmod: '2022-12-13'
categories: [d2d, client]
keywords: [d2d, client, proximity, udp]
menu:
  docs:
    parent: device-to-device-client
    weight: 40
toc: true
---

## Onboard device to plgd hub

To provision device to plgd hub, we need to provide to the device these parameters:

- coapGatewayAddress - address of the plgd hub in the format `<scheme>://<host>:<port>`
- authorizationCode - authorization code which is used to authorize the device on the plgd hub
- authorizationProviderName - authorization provider which is used to authorize the device on the plgd hub
- hubID - id of the plgd hub
- certificateAuthorities - certificates authorities which is used to verify coap-gateway certificate by the device

After the device is provisioned, the plgd hub can be used to access the device.

If plgd hub requires mTLS authentication, the device needs to have the identity/manufacturer certificate signed by the certificate authority which is known in the plgd hub.

### Compatibility credential matrix with plgd hub

Describes how device establish the connection to the plgd hub and if mTLS authentication can be used.

|  PSK      | Manufacturer Certificate | Identity Certificate  | Device uses              | mTLS coap-gateway(plgd-hub) |
| ----------| ------------------------ | --------------------- | ------------------------ | --------------------------- |
| yes/no    | no                       | no                    | without certificate      | no                          |
| yes/no    | yes                      | no                    | manufacturer certificate | yes                         |
| yes/no    | yes/no                   | yes                   | identity certificate     | yes                         |

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

To offboard the device from the plgd hub, the device can be in the any state. Device can still exist in the plgd hub after the offboarding process, because deletion by self can fails or it is not possible. In this case, the device need to be deleted from the plgd hub.

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
