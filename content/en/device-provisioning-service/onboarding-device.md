---
title: 'Onboarding device to DPS'
description: 'How to configure the DPS device with onboarding agent ?'
date: '2021-03-24'
lastmod: '2021-03-24'
categories: [zero-touch, provisioning]
keywords: [provisioning]
menu:
  docs:
    parent: device-provisioning-service
    weight: 60
toc: true
---


A DPS device is still an OCF device containing a device provisioning client which provides a configuration resource. First, you need to own the device through your onboarding tool. Upon owning, you can configure device provisioning service endpoint in format `coaps+tcp://{domain}:{port}` via update resource. Observing the configuration resource allows you to track the device provisioning status via the property `ps`.

## Device provisioning configuration resource

The resource is not registered at the device by default. The `plgd_dps_set_configuration_resource` method must be called during registration resources in order to register a resource.

### Properties of the "x.plgd.dps.conf" resource

| Property Title | Property Name | Type | Access Mode | Mandatory | Description |
| -------------- | ------------- | -----| ----------- | --------- | ----------- |
| Endpoint       | endpoint            | string | RW | No | Device provisioning server endpoint in format `coaps+tcp://{domain}:{port}` |
| Last error code | lastErrorCode         | string | R  | No | Provides a last error code when provision status is in `failed` state.<br> `0` - OK<br> `1` - error response<br> `2` - cannot connect to dps<br> `3` - cannot apply credentials configuration<br> `4` - cannot apply acls configuration `5` - cannot apply cloud configuration |
| Self owned       | selfOwned           | bool   | R  | No | For true, device is owned by itself via "C-API" |
| Force reprovision        | forceReprovision           | bool   | RW | No | Connects to DPS service and reprovision all resource (credentials, ACLs, cloud configuration,..) |
| Provision status| provisionStatus       | enum(string) | R  | No | `uninitialized` - ep is not set or dps manger has not been started yet<br> `initialized` - ep is set and manager starting requests<br> `provisioning credentials` - provisioning credentials has been stared<br> `provisioning acls` - provisioning acls has been stared<br> `provisioning cloud` - provisioning cloud has been stared<br> `provisioned` - device is fully provisioned and configured<br> `failed` - provisioning fails, more information is stored in last error code |

## Onboarding device to DPS

For onboarding owned devices, you need to update the configuration resource by the endpoint.

```golang
   dps := "coaps+tcp://try.plgd.cloud:25684"
   err = c.UpdateResource(ctx, deviceID, "/plgd/dps", map[string]interface{}{
    "ep": dps,
  }, nil)
  if err != nil {
    panic(err)
  }
```

After that, the device provisioning client will provision configuration from the device provision service and onboard the device to the hub.

### Setup DPS trust anchor to the device

The device by default verifies the endpoint certificate by the installed trust anchors using the C-API `oc_pki_add_trust_anchor` or by provisioning the certificate through the onboarding tool. To set up trust anchors, change operation mode from normal to provisioning, update the credential resource, and then return to the normal operation mode.

```golang
  trustAnchor := "-----BEGIN CERTIFICATE-----\n...\n-----END CERTIFICATE-----\n"
  d, links, err := c.GetRefDevice(ctx, deviceID)
  if err != nil {
    panic(err)
  }
  defer d.Release(ctx)
  // change operation mode from normal to provisioning
  p, err := d.Provision(ctx, links)
  if err != nil {
    panic(err)
  }
  // update the credential resource
  err = p.AddCredentials(Credentials: []credential.Credential{
    {
      Subject: uuid.NewSHA1(uuid.NameSpaceDNS, []byte("try.plgd.cloud")).String(),
      Type:    credential.CredentialType_ASYMMETRIC_SIGNING_WITH_CERTIFICATE,
      Usage:   credential.CredentialUsage_TRUST_CA,
      PublicData: &credential.CredentialPublicData{
        DataInternal: trustAnchor,
        Encoding:     credential.CredentialPublicDataEncoding_PEM,
      },
    },
  })
  // return to normal operation mode
  errPro = p.Close(ctx)
  if err != nil {
    panic(err)
  }

  if errPro != nil {
    panic(err)
  }

```
