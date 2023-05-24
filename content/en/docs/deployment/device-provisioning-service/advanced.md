---
title: 'Advanced Configuration'
description: 'How to customize the Device Provisioning Service deployment'
docsOthersDisplay: true
date: '2023-05-24'
categories: [deployment, kubernetes]
keywords: [deployment, kubernetes, k8s, helm, chart]
weight: 40
---

## Customize OAuth client credential flow

In production it is expected to have own OAuth server where you create OAuth client for DPS that support client credential flow. In this case you need add append OAuth client configuration to the `global.oauth.device` section:

```yaml
global:
  oauth:
    device:
    - name: "plgd.dps"
      clientID: "<MY_CLIENT_ID>"
      clientSecret: "<MY_CLIENT_SECRET>"
      grantType: "clientCredentials"
      scopes: ['openid']
      audience: "https://api.example.com"
    ...
```

## Customize client certificates for DPS

As first you need to set certificates to the secret:

```sh
kubectl -n plgd create secret generic dps-my-client-certs --from-file=ca.crt --from-file=tls.crt --from-file=tls.key
```

where `ca.crt` is the CA certificate, `tls.crt` is the client certificate and `tls.key` is the client private key. Then you need add `extraVolume` and `extraVolumeMounts` to the `deviceProvisioningService` section v:

```yaml
deviceProvisioningService:
  extraVolumeMounts:
    - name: dps-client-certs
      mountPath: /certs/my-client-certs
  extraVolumes:
    - name: dps-client-certs
      secret:
        secretName: dps-my-client-certs
```

And finally you need to set a client `.tls` section in the `deviceProvisioningService` section eg. for mongo:

```yaml
deviceProvisioningService:
  clients:
    storage:
      mongo:
        tls:
          ca: /certs/my-client-certs/ca.crt
          cert: /certs/my-client-certs/tls.crt
          key: /certs/my-client-certs/tls.key
```
