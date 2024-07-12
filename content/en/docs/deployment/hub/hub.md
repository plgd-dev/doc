---
title: 'IoT Hub'
description: 'Running hub on Kubernetes'
docsOthersDisplay: true
date: '2021-11-01'
categories:

- deployment
- kubernetes

keywords:

- deployment
- kubernetes
- k8s
- helm
- chart

weight: 10
isMainSectionPage: true
---

![Deployment diagram](/docs/deployment/hub/static/hub-deployment-diagram.drawio.svg "medium-zoom-image")

The quickest way to install your own instance of the plgd hub is to use the Mock OAuth2.0 Server. With this option enabled, no authentication is available, and a JWT token is automatically issued for the default user.

{{< warning >}}
The Mock OAuth Server should only be used for **testing** and **development purposes**. Exercise caution when using it! For production environments, follow the deployment instructions provided in the [Advanced Configuration](/docs/deployment/hub/advanced) section to ensure proper setup.
{{< /warning >}}

Before proceeding, generate a private key for `m2m-oauth-server`:

```sh
openssl ecparam -name prime256v1 -genkey -noout
```

Then, add the generated private key to the `m2mOAuthServer.privateKey` field in the `values.yaml` file:

```sh
echo "
global:
  domain: \"example.com\"
  hubId: \"d03a1bb4-0a77-428c-b78c-1c46efe6a38e\"
  m2mOAuthServer:
    privateKey: |-
      <GENERATED_PRIVATE_KEY>
mockoauthserver:
  enabled: true" > withMock.yaml

helm upgrade -i -n plgd --create-namespace -f withMock.yaml hub plgd/plgd-hub
```

{{< note >}}
If you are using `microk8s` or a similar Kubernetes system, ensure that the `ingress`, `dns`, and `storage` add-ons are enabled. Also, configure your Kubernetes cluster to resolve the private domain `*.example.com` through the designated DNS server. For `microk8s`, use [microk8s enable dns:{DNS_SERVER}](https://microk8s.io/docs/addon-dns) to set up the DNS server.
{{< /note >}}

The deployment of the plgd hub to the Kubernetes cluster can now be initiated. Verify the status of the deployment using `kubectl -n plgd get all`. Once all pods are up and running, the plgd Dashboard will be accessible at your configured domain (e.g., `https://example.com`).

## NodePort for CoAP Gateway

If you are deploying the plgd hub on [microk8s.io/](https://microk8s.io/), you need to enable the NodePort for the CoAP Gateway Service and extend the port range of the K8S API Server. To extend the port range, add `--service-node-port-range=5683-32767` to `/var/snap/microk8s/current/args/kube-apiserver`. For the CoAP Gateway Service, apply the following values:

```yaml
coapgateway:
  service:
    type: NodePort
    nodePort: 5684
```

{{< warning >}}
Apply this configuration only in a test environment!
{{< /warning >}}

## Deploying to Production

For deploying the Device Provisioning Service in a production environment, refer to the [Advanced Configuration](/docs/deployment/hub/advanced) section for detailed instructions.
