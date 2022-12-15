---
title: 'Change the device provisioning status'
description: 'How to change device provisioning status'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [tutorials, provisioning]
keywords: [tutorials, provisioning]
menu:
  docs:
    parent: tutorials
    weight: 20
toc: true
---

For update any secure device resource such at credentials/ACLs we need to change device from normal state to provisioning state. To get current state of device we can use `GET` request on device provisioning status resource(/oic/sec/pstat). The property `dos.s` is state of device.

| State name | Value | Notes |
| --------------------------- | --------------------- |
| Reset | 0 | Factory reset - device will change state after that to RFOTM |
| Ready for Device owner transfer method (RFOTM) | 1 | Device is ready to be owned |
| Ready for Device provisioning (RFPRO) | 2 | Update some properties in secure resources by resource owner and owner |
| Ready for Device normal operation (RFNOP) | 3 | Device is ready to be used |
| The Device is in a soft reset (SRESET) | 4 | Update properties secure resources by owner |

{{< note >}}
More about this topic you can find in [OCF specification](https://openconnectivity.org/specs/OCF_Security_Specification.pdf) in section 8 Device Onboarding state definitions.
{{< /note >}}

To change state of device we can use `POST` request on device provisioning status resource we need to be a owner of the device or resource owner which is stored in `rowneruuid` property.

```json
{
  "dos": {
    "s": 2
  }
}
```

After changing state of device we can update device secure sources such as credentials, ACLs, etc. After updating device secure sources we need to change state of device to normal operation.

```json
{
  "dos": {
    "s": 3
  }
}
```

{{< plantuml id="update-secure-resources" >}}
@startuml Sequence
skinparam backgroundColor transparent
hide footbox

box "D2D Client"
participant S as "Web App\n(running in the browser)"
participant C as "Service\n(local or remote host)"
end box

participant D as "Device\n(in the local network with Service)"

S -> D++: Update state to provisioning state\n(POST /oic/sec/pstat {"dos": {"s": 2}})
activate S
return Device is in provisioning state

S -> D++: Update credentials, ACLs, etc.
return Updated
S -> D++: Update state to normal state\n(POST /oic/sec/pstat {"dos": {"s": 3}})

return Device is in normal state
deactivate S

@enduml
{{< /plantuml >}}
