---
title: 'plgd on K8S'
description: 'Running plgd hub on Kubernetes'
docsOthersDisplay: true
date: '2021-11-01'
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

In our GitHub repository you can find `charts/` folder with plgd hub [Helm chart](https://helm.sh/docs/topics/charts/). This chart is automatically published to the Helm registry `https://charts.plgd.dev` during the release. Version of the chart is in sync with the appversion, matching the release tag _(e.g. 2.1.1)_. All configuration values are documented [here](https://github.com/plgd-dev/hub/tree/main/charts/plgd-hub#values).

### Install Certificate Manager

A communication between devices, clients and plgd hub is secured as well as between plgd hub services. To simplify the deployment and certificate management, [Certificate Manager](https://cert-manager.io/docs/), required dependency, have to be deployed in your Kubernetes cluster.

```sh
kubectl apply -f https://github.com/jetstack/cert-manager/releases/download/v1.6.0/cert-manager.yaml
```

### Register plgd Helm Chart Registry

```sh
// helm repo add plgd https://charts.plgd.dev
// helm repo update
Hang tight while we grab the latest from your chart repositories...
...Successfully got an update from the "plgd" chart repository

// helm repo list
NAME    URL
plgd    https://charts.plgd.dev

// helm search repo plgd
NAME            CHART VERSION   APP VERSION     DESCRIPTION
plgd/plgd-hub   2.1.1           2.1.1           A Helm chart for plgd-hub

```

### Deployment with Mock OAuth2.0 Server

Quickest way how to install your own instance of the plgd hub is to use the Mock OAuth2.0 Server. With this option enabled, no authentication is available. The JWT token is automatically issued for the default user.

{{< warning >}}
Mock OAuth Server shall be used only for test/development purposes. Use with extra care!
{{< /warning >}}

```sh
echo "global:
  domain: \"example.com\"
  hubId: \"d03a1bb4-0a77-428c-b78c-1c46efe6a38e\"
  mockoauthserver:
  enabled: true" > withMock.yaml

helm install -f withMock.yaml hub plgd/plgd-hub
```

Deployment of the plgd hub to the Kubernetes cluster is then initiated. Status of the deployment can be verified by calling `kubectl get all`. When all pods are up and running, the plgd Dasboard will become available on your configured domain (e.g. `https://example.com`).

### NodePort for CoAP Gateway

In case you install plgd hub into [microk8s.io/](https://microk8s.io/), it's required to enable also
nodePort for the CoAP Gateway Service and extend the port range of the K8S API Server. To extend the port range, add `--service-node-port-range=5683-32767` to `/var/snap/microk8s/current/args/kube-apiserver`. For the CoAP Gateway Service, apply following values:

```yaml
coapgateway:
  service:
    type: NodePort
    nodePort: 5684
```

{{< warning >}}
This configuration should be applied only to test environment!
{{< /warning >}}

### Deployment with Auth0 OAuth2.0 Server

Our [try.plgd.cloud](https://try.plgd.cloud) instance which is available for free uses [Auth0 Identity Provider](https://auth0.com). Example configuration enabling an integration with an external OAuth2.0 Server should contain OAuth2.0 configuration for the device as well as the dasboard. All required values are part of the `global.` index.

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

helm install -f withMock.yaml hub plgd/plgd-hub
```

### Custom Authorization CA pool

By default, the plgd hub Helm chart expect that certificate used by OAuth 2.0 server is signed by the same CA as rest of certificates
used by plgd hub services. For including custom authorization CA pool into authorization section, use following configuration attribute:

```yaml
global:
  # -- Custom CA certificate for authorization endpoint in PEM format
  authorizationCAPool: |
    -----BEGIN CERTIFICATE-----
    your custom authorization CA pool in PEM format
    -----END CERTIFICATE-----
```

### Using Let's encrypt certificates

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
  - Not trusted self-signed certificate is used. Import of plgd CA to your system is required. Get the public key by calling `kubectl get secret plgd-ca -o 'go-template={{index .data "ca.crt"}}' | base64 -d`.
