---
title: 'Device Provisioning Service'
description: 'Running Device Provisioning Service on Kubernetes'
docsOthersDisplay: true
date: '2023-05-18'
categories: [deployment, kubernetes]
keywords: [deployment, kubernetes, k8s, helm, chart]
weight: 2
---

![Deployment diagram](../static/dps-deployment-diagram.drawio.svg "medium-zoom-image")

To enable the Device Provisioning Service, you need to extend the hub configuration.

{{< note >}}
Before deploying the Device Provisioning Service on Kubernetes, make sure to follow the steps in [Hub](../hub) first. Then apply the changes from this page to the configuration. Once done, you can deploy the hub with the Device Provisioning Service.
{{< /note >}}

For Device Provisioning Service, all configuration values are documented [here](https://github.com/plgd-dev/device-provisioning-service/blob/main/charts/device-provisioning-service/README.md#values).

## Device provider for Device Provisioning Service

When a device is successfully attested against an enrollment group by the Device Provisioning Service, the device's owner needs to be set. The owner is configured in the enrollment group. Therefore, the Device Provisioning Service needs to obtain the authorization code from the OAuth service, which is associated with the owner. When the device contacts the hub, the hub will exchange the authorization code for a JWT access token that contains the owner claim with the owner.

### Client credential flow

In this flow, the OAuth client is associated with the owner. The client must set the owner claim with the owner in the JWT access token.

When using the `clientCredentials` flow, you cannot use `sub` because the owner will be the OAuth client. Therefore, you need to use another claim to identify the owner. We need to override default the hub configuration's section with  new `ownerClaim`, dps and web oauth clients:

```yaml
global:
  ownerClaim: "https://plgd.dev/owner" # for mockoauthserver
  oauth:
    device:
    - name: "plgd.dps"
      clientID: my-client-id
      clientSecret: my-client-secret
      grantType: "clientCredentials"
      scopes: ['openid']
      audience: "https://api.example.com"
```

If you are using a mock OAuth2.0 server, add the following values to the `mockoauthserver` section:

```yaml
mockoauthserver:
  oauth:
  - name: "plgd.dps"
    clientID: "test"
    clientSecret: "test"
    grantType: "clientCredentials"
    scopes: ['openid']
    audience: "https://api.example.com"
    redirectURL: "https://mock.plgd.cloud/things"
  - name: "plgd.web"
    clientID: "test"
    clientSecret: "test"
    redirectURL: "https://mock.plgd.cloud/things"
    scopes: ['openid']
    useInUi: true
```

To enable the Device Provisioning Service, the following configuration needs to extend the hub configuration:

```yaml
deviceProvisioningService:
  image:
    dockerConfigSecret: |
      {
        "auths": {
          "ghcr.io": {
              "auth": "<DOCKER_AUTH_TOKEN>"
          }
        }
      }
```

{{< note >}}

To access ghcr.io, please reach out to us at [connect@plgd.dev](mailto:connect@plgd.dev) in order to request permission for your GitHub account to access the plgd device provisioning server images. You can refer to the [documentation](https://kubernetes.io/docs/tasks/configure-pod-container/pull-image-private-registry/) for instructions on how to allow access.

{{< /note >}}

## Configure Enrollment Groups

To configure enrollment groups, you need to extend the hub configuration. In this example, the `https://example.com` is a public domain signed by the Let's Encrypt CA. The hub configuration will be used to set up the enrollment group.

{{< note >}}

In the `deviceProvisioningService.enrollmentGroups[].attestationMechanism.x509.certificateChain` field, provide the certificate chain in PEM format that has used to sign the device manufacturer certificate (IDevId). The certificate chain should include the root CA certificate and the intermediate CA certificates.

{{< /note >}}

```yaml
deviceProvisioningService:
  enrollmentGroups:
    - id: 98be12de-3991-4567-aeea-edd9a656e6df
      owner: "1" # for mockoauthserver
      preSharedKey: "0123456789012345"
      attestationMechanism:
        x509:
          certificateChain: |-
            -----BEGIN CERTIFICATE-----
            MIIBaDCCAQ6gAwIBAgIQM6HFZ+BMuyBspnPJEhPX9DAKBggqhkjOPQQDAjAUMRIw
            ...
            7vHsqw2PCgAm9Qs3ZEQaKMZ2EGuPBvvXZXjPDA==
            -----END CERTIFICATE-----
      hub:
        authorization:
          provider:
            name: "plgd.dps" # same as global.oauth.device[].name for dps provider
            clientId: "test"  # for mockoauthserver
            clientSecret: "test"  # for mockoauthserver
            scopes: ["openid"]
```

If you are using a self-signed certificate, you need to append the custom CA to `authorizationCAPool`:

```yaml
global:
  authorizationCAPool: |-
    ...
    -----BEGIN CERTIFICATE-----
    your custom authorization CA pool in PEM format
    -----END CERTIFICATE-----
deviceProvisioningService:
  enrollmentGroups:
    - hub:
        certificateAuthority:
          grpc:
            tls:
              caPool: "/certs/extra/ca.crt"
```

{{< note >}}

More information about the enrollment group configuration can be found [here](../../configuration/device-provisioning-service/#enrollment_groups).

{{< /note >}}

## NodePort for Device Provisioning Service

If you install the plgd hub into [microk8s.io/](https://microk8s.io/), you need to also enable the nodePort for the Device Provisioning Service. For the Device Provisioning Service, apply the following values:

```yaml
deviceProvisioningService:
  service:
    type: NodePort
    nodePort: 15684
```

{{< note >}}
The port is set to `15684` because the default port `5684` is already used by the CoAP Gateway Service.
{{< /note >}}

{{< warning >}}
This configuration should be applied only to test environment!
{{< /warning >}}

## Deploy the hub with Device Provisioning Service

To deploy the hub with the Device Provisioning Service, apply the following commands:

```sh
helm upgrade -i -f withMock.yaml hub plgd/plgd-hub
helm upgrade -i -f withMock.yaml dps plgd/plgd-dps
```

These commands you call multiple times to update the configuration. In this case you need to restart the pods by delteing them:

```sh
kubectl delete pods $(kubectl get pods | grep "hub-plgd" | cut -d " " -f 1)
kubectl delete pods $(kubectl get pods | grep "dps-plgd" | cut -d " " -f 1)
```
