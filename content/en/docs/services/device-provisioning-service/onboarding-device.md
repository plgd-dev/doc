---
title: 'Starting zero-touch provisioning'
description: 'What are the possibilites on triggering the zero-touch provisioning?'
date: '2022-06-20'
docsOthersDisplay: true
categories: [zero-touch, provisioning]
keywords: [provisioning, dps]
weight: 4
---

IoTivity devices can be extended by the Device Provisioning Client library, which enables you to use the zero-touch provisioning feature. Integrating this library into your device enables you to start the automated provisioning through 2 APIs.

- DPS Client Library API of the [DPS Client library](/docs/services/device-provisioning-service/client-library#dps-client-api)
- Configuring the [DPS resource](/docs/services/device-provisioning-service/client-library#dps-configuration-resource)

## DPS Client Library API

Provisioning process can be initiated by your application or immediately when the device is started. This can be all managed by your device firmware, without any external interaction. To read more about how to interact and configure the DPS Client Library, read [here](/docs/services/device-provisioning-service/client-library).

## DPS Resource

If the DPS resource was enabled using the DPS Client Library API, users or deamons have the possibility to trigger the automated provisioning by configuring this resource over network.
To configure the DPS endpoint on the resource, the device needs to be before the self-provisioning and owned by the onboarding tool.

The resource can be observed and it is published in the plgd hub. This enables you to track the provisioning progress by observing the value of the `ps` property.

### How to configure DPS Resource

To trigger device provisioning on owned devices, you need to update the DPS resource by the endpoint.

```golang
   dps := "coaps+tcp://try.plgd.cloud:25684"
   err = c.UpdateResource(ctx, deviceID, "/plgd/dps", map[string]interface{}{
    "endpoint": dps,
  }, nil)
  if err != nil {
    panic(err)
  }
```

After that, the device provisioning client will provision configuration, ACLs and cloud configuration from the DPS and onboard the device to the plgd hub.

## Setup DPS trust anchor to the device

The device by default verifies the endpoint certificate by the trust anchors which were installed using the C-API `oc_pki_add_mfg_trust_anchor` or by provisioning the certificate through the onboarding tool. To set up trust anchors, change operation mode from normal to provisioning, update the credential resource, and then return to the normal operation mode.

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
      Usage:   credential.CredentialUsage_MFG_TRUST_CA,
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
