---
title: 'plgd on K8S'
description: 'Running plgd hub on Kubernetes'
date: '2021-11-01'
lastmod: '2021-11-01'
categories: [deployment, kubernetes]
keywords: [deployment, kubernetes, k8s, helm, chart]
menu:
  docs:
    parent: deployment
    weight: 10
toc: true
---

In this section, you can find several examples of how to deploy plgd hub with external OAuth2.0 Server or with mocked one for development purposes.

## Getting started

In our GitHub repository you can find `charts/` folder with [Helm 3 based chart](https://helm.sh/docs/topics/charts/) to install the [plgd hub](https://github.com/plgd-dev/hub/tree/main/charts/plgd-hub). The plgd hub chart is automatically published to the Helm registry `https://charts.plgd.dev` automatically with every release. Version of the chart is in sync with the appversion, matching the release tag _(e.g. 2.1.1)_.

### Install Certificate Manager

A communication between devices, clients and plgd hub is secured as well as the communication between the plgd hub services. To simplify the deployment and certificate management, [Certificate Manager](https://cert-manager.io/docs/), required dependency, have to be deployed in your Kubernetes cluster. 

```sh
> kubectl apply -f https://github.com/jetstack/cert-manager/releases/download/v1.6.0/cert-manager.yaml
```

### Register plgd Helm Chart Registry

```sh
> helm repo add plgd https://charts.plgd.dev
> helm repo update
Hang tight while we grab the latest from your chart repositories...
...Successfully got an update from the "plgd" chart repository

> helm repo list
NAME    URL                               
plgd    https://charts.plgd.dev   

> helm search repo plgd
NAME            CHART VERSION   APP VERSION     DESCRIPTION              
plgd/plgd-hub   2.1.1           2.1.1           A Helm chart for plgd-hub

```

### Deployment with Mock OAuth2.0 Server

Quickest way how to install your own instance of the plgd hub is to use the mock OAuth2.0 Server **used for tests and development**. When using this option, no user management and authentication is available. Default user is automatically logged in. 

```sh
> echo "global:
  domain: \"example.com\"
  hubId: \"d03a1bb4-0a77-428c-b78c-1c46efe6a38e\"
mockoauthserver:
  enabled: true" > withMock.yaml

> helm install -f withMock.yaml hub plgd/plgd-hub
```

The plgd hub is then being deployed to the Kubernetes cluster. Status of the deployment can be verified by calling `kubectl get all`. When all pods are up and running, plgd Dasboard will be available on `https://example.com` domain.

### Deployment with Auth0 OAuth2.0 Server

Our [try.plgd.cloud](https://try.plgd.cloud) instance which is available for free uses [Auth0 Identity Provider](https://auth0.com). Sample configuration which enables you to use any OAuth2.0 Server would be as follows:

```sh
> echo "global:
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
      redirectURL: \"https://example.com/things\"
      useInUi: true
    web:
      clientID: \"98y239hu94hr2ohu3e23eh\"" > withMock.yaml

> helm install -f withMock.yaml hub plgd/plgd-hub
```

### Using Let's encrypt certificates

By default, plgd hub Helm chart issues a self-signed CA certificate, used to sign domain certificates of all exposed services. To encrypt the external communication with the certificates signed by the Let's Encrypt CAs, create an issuer:

```sh
> echo "apiVersion: cert-manager.io/v1
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

> kubectl apply -f issuer.yaml
```

Required values for the plgd hub Helm chart which make use of a newly created signer are:
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

- plgd Dashboard returns an error that it cannot get the data from the ./well-known endpoint
  - Self-signed certificate is used. Import of plgd CA to your browser is therefore required. Get the public key by calling `kubectl get secret plgd-ca -o 'go-template={{index .data "ca.crt"}}' | base64 -d`