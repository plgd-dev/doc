---
title: 'Advanced Configuration'
description: 'How to customize the Device Hub deployment'
docsOthersDisplay: true
date: '2021-11-01'
categories: [deployment, kubernetes]
keywords: [deployment, kubernetes, k8s, helm, chart]
weight: 20
---

## Deployment with Auth0 OAuth2.0 Server

Our [try.plgd.cloud](https://try.plgd.cloud) instance which is available for free uses [Auth0 Identity Provider](https://auth0.com). Example configuration enabling an integration with an external OAuth2.0 Server should contain OAuth2.0 configuration for the device as well as the Dashboard. All required values are part of the `global.` index.

```sh
echo "global:
domain: \"example.com\"
hubId: \"d03a1bb4-0a77-428c-b78c-1c46efe6a38e\"
authority: \"https://myinstance.auth0.com\"
audience: \"https://api.example.com\"
oauth:
  device:
  - name: \"onboardingtool.mobile\"
    clientID: \"l12j3oi12j3jlk1904\"
    clientSecret: \"891y2ehpu2he9182heh2ep9128eh\"
    scopes: [\"offline_access\"]
    redirectURL: \"com.example.mobile://login-callback\"
    useInUi: false
  - name: \"onboardingtool.desktop\"
    clientID: \"i0jt0i00xcva0r\"
    clientSecret: \"98219h23uh43t9r4h93rh994th923r\"
    scopes: [\"offline_access\"]
    redirectURL: \"https://example.com/devices\"
    useInUi: true
  web:
    clientID: \"98y239hu94hr2ohu3e23eh\"" > withMock.yaml

helm upgrade -i -n plgd --create-namespace -f withMock.yaml hub plgd/plgd-hub
```

## Custom Authorization CA pool

By default, the plgd hub Helm chart expect that certificate used by OAuth 2.0 server is signed by the same CA as rest of certificates
used by plgd hub services. For including custom authorization CA pool into authorization section, use following configuration attribute:

```yaml
global:
  # -- Custom CA certificate for authorization endpoint in PEM format
  authorizationCAPool: |-
    -----BEGIN CERTIFICATE-----
    your custom authorization CA pool in PEM format
    -----END CERTIFICATE-----
```

{{< warning >}}

Please integrate your own PKI for signing certificates used by plgd hub services in a production environment. For detailed instructions on how to do this, refer to this [link](https://cert-manager.io/docs/configuration/ca/).

{{< /warning >}}

## Using Let's encrypt certificates

By default, the plgd hub Helm chart issues a self-signed CA certificate, used to sign domain certificates of all exposed services. To encrypt the external communication with the certificates signed by the Let's Encrypt CAs, create an issuer:

```sh
echo "apiVersion: cert-manager.io/v1
kind: ClusterIssuer
metadata:
  name: \"letsencrypt-prod\"
spec:
  acme:
    server: \"https://acme-v02.api.letsencrypt.org/directory\"
    email: \"info@example.com\"
    privateKeySecretRef:
      name: \"letsencrypt-prod\"
    solvers:
    - http01:
        ingress:
          class: public" > issuer.yaml

kubectl apply -f issuer.yaml
```

Required values for the plgd hub Helm chart which makes use of a newly created signer are:

```yaml
certmanager:
  external:
    issuer:
      kind: "ClusterIssuer"
      name: "letsencrypt-prod"
global:
  enableWildCartCert: false
...
```

## Troubleshooting

- plgd Dashboard returns "unable to fetch data from from the ./well-known endpoint" error
- Not trusted self-signed certificate is used. Import of plgd CA to your system is required. Get the public key by calling `kubectl -n plgd get secret plgd-ca -o 'go-template={{index .data "ca.crt"}}' | base64 -d`.
