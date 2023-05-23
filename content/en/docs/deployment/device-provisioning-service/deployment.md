---
title: 'Deployment'
description: 'Running Device Provisioning Service on Kubernetes'
docsOthersDisplay: true
date: '2023-05-18'
categories: [deployment, kubernetes]
keywords: [deployment, kubernetes, k8s, helm, chart]
weight: 10
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
      clientID: "<MY_CLIENT_ID>"
      clientSecret: "<MY_CLIENT_SECRET>"
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

To allow download the Device Provisioning Service docker image by k8s, the following configuration needs to extend the configuration:

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

The enrollment groups can be configured via deployment, utilizing the setup from the hub configuration to populate the values.

{{< note >}}

In the deviceProvisioningService.enrollmentGroups[].attestationMechanism.x509.certificateChain field, please provide the ECDSA certificate chain in PEM format that was used to sign the device manufacturer certificate (IDevId). The certificate chain should include the intermediate CA certificates and the root CA certificate in the order from the closest intermediate CA leaf up to the root CA.

{{< /note >}}

```yaml
deviceProvisioningService:
  enrollmentGroups:
    - id: 98be12de-3991-4567-aeea-edd9a656e6df
      owner: "1" # for mockoauthserver
      preSharedKey: "0123456789012345" # for the client application
      attestationMechanism:
        x509:
          # certificate for validation the device manufacturer certificate (IDevId)
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

{{< note >}}

To successfully manage the device from local network, make sure to set the `owner` field to the corresponding `SubjectId` value and configure the `preSharedKey` as `Key` in the login screen of the [plgd/client-application](../../../quickstart/local-discovery).

![Login to Client Application](../static/psk-login.png)

If `preSharedKey` is not set, the device will be managed only via certificate authentication.

{{< /note >}}

### Custom CA for Certificate Authority

When utilizing a PKI certificate for `example.com`, it is necessary to append the custom CA to the `authorizationCAPool`:

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
              # set CA with your custom CA configured in global.authorizationCAPool
              caPool: "/certs/extra/ca.crt"
```

{{< note >}}

More information about the enrollment group configuration can be found [here](../../../configuration/device-provisioning-service/#enrollment_groups).

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
helm upgrade -i -n plgd --create-namespace -f withMock.yaml hub plgd/plgd-hub
helm upgrade -i -n plgd --create-namespace -f withMock.yaml dps plgd/plgd-dps
```

You can execute these commands multiple times to update the configuration. In such cases, you will need to restart the pods by deleting them:

```sh
kubectl -n plgd delete $(kubectl -n plgd get pods -o name | grep "hub-plgd")
kubectl -n plgd delete $(kubectl -n plgd get pods -o name | grep "dps-plgd")
```

The final configuration with mock oauth server should look like this:

```yaml
global:
  domain: "example.com"
  hubId: "d03a1bb4-0a77-428c-b78c-1c46efe6a38e"
  ownerClaim: "https://plgd.dev/owner"
mockoauthserver:
  enabled: true
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
coapgateway:
  service:
    type: NodePort
    nodePort: 5684
deviceProvisioningService:
  image:
    dockerConfigSecret: |
      {
        "auths": {
          "ghcr.io": {
              "auth": "amty...CMFh3"
          }
        }
      }
  service:
    type: NodePort
    nodePort: 15684
  enrollmentGroups:
    - id: 98be12de-3991-4567-aeea-edd9a656e6df
      owner: "1" # for mockoauthserver
      preSharedKey: "0123456789012345" # must be 16 characters
      attestationMechanism:
        x509:
          certificateChain: |-
            -----BEGIN CERTIFICATE-----
            MIIBaDCCAQ6gAwIBAgIQM6HFZ+BMuyBspnPJEhPX9DAKBggqhkjOPQQDAjAUMRIw
            EAYDVQQDEwlNZmdSb290Q0EwHhcNMjIwOTE0MTQyNDEwWhcNMjMwOTE0MTQyNDEw
            WjAUMRIwEAYDVQQDEwlNZmdSb290Q0EwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNC
            AAQJWVyzAr0ugzn1rMsPxZiVBvSlegjdnS7m46uN2hKB5UR00oWlE9qj/PmGd9fQ
            TZp5Bc6JPkTG2QL07CBEIAm2o0IwQDAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/
            BAUwAwEB/zAdBgNVHQ4EFgQUJEcpCxDACml6nF65kIQiIVpxxb4wCgYIKoZIzj0E
            AwIDSAAwRQIgISZ/tjmeXieQt4sV/7qbSTjlhDlFC5sU4thtpSFGfq4CIQDez2+g
            7vHsqw2PCgAm9Qs3ZEQaKMZ2EGuPBvvXZXjPDA==
            -----END CERTIFICATE-----
      hub:
        authorization:
          provider:
            name: "plgd.dps"
            clientId: "test"
            clientSecret: "test"
```
