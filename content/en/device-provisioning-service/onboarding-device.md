---
title: 'Starting zero-touch provisioning'
description: 'What are the possibilites on triggering the zero-touch provisioning?'
date: '2022-06-20'
lastmod: '2022-06-20'
categories: [zero-touch, provisioning]
keywords: [provisioning, dps]
menu:
  docs:
    parent: device-provisioning-service
    weight: 60
toc: true
---

IoTivity devices can be extended by the Device Provisioning Client library, which enables you to use the zero-touch provisioning feature. Integrating this library into your device enables you to start the automated provisioning through 2 APIs.
- DPS Client Library API of the [DPS Client library](../client-library#dps-client-api)
- Configuring the [DPS resource](../client-library#dps-configuration-resource)

## DPS Client Library API

Provisioning process can be initiated by your application or immediateny when the device is started. This can be all managed by your device firmware, without any external interaction. To read more how to interact and configure DPS Client Library, read [here](../client-library).

## DPS Resource

If DPS resouce was enabled using the DPS Client Library API, users or deamons have a possibilty to trigger the automated provisioning by configuring this resource over network.
The device needs to be before the self-provisioning owned by the daemon / tool, to configure the DPS endpoint on the resource.

Resource can be observed and is published also the the plgd hub. This enables you to track the provisioning progress by observing the value of the `ps` property.

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
