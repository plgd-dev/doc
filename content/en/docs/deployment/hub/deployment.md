---
title: 'Deployment'
description: 'Running hub on Kubernetes'
docsOthersDisplay: true
date: '2021-11-01'
categories: [deployment, kubernetes]
keywords: [deployment, kubernetes, k8s, helm, chart]
weight: 10
---

![Deployment diagram](/docs/deployment/hub/static/hub-deployment-diagram.drawio.svg "medium-zoom-image")

Quickest way how to install your own instance of the plgd hub is to use the Mock OAuth2.0 Server. With this option enabled, no authentication is available. The JWT token is automatically issued for the default user.

{{< warning >}}
The Mock OAuth Server should only be utilized for **testing** and **development purposes**. Exercise additional caution when using it! For production environments, please follow the deployment instructions provided in the [Advanced Configuration](../advanced) section to ensure proper setup.
{{< /warning >}}

```sh
echo "
global:
  domain: \"example.com\"
  hubId: \"d03a1bb4-0a77-428c-b78c-1c46efe6a38e\"
mockoauthserver:
  enabled: true" > withMock.yaml

helm upgrade -i -n plgd --create-namespace -f withMock.yaml hub plgd/plgd-hub
```

{{< note >}}

To use `microk8s` or a similar kubernetes system, ensure that the `ingress`, `dns`, and `storage` addons are enabled. Additionally, configure your Kubernetes cluster to resolve the private domain `*.example.com` through the designated DNS server. For microk8s, use [microk8s enable dns:{DNS_SERVER}](https://microk8s.io/docs/addon-dns) to set up the DNS server.

{{< /note >}}

Deployment of the plgd hub to the Kubernetes cluster is then initiated. Status of the deployment can be verified by calling `kubectl -n plgd get all`. When all pods are up and running, the plgd Dashboard will become available on your configured domain (e.g. `https://example.com`).

## NodePort for CoAP Gateway

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

## Deploying to Production

In order to deploy the Device Provisioning Service to a production environment, please refer to the [Advanced Configuration](../advanced) section for detailed instructions.
