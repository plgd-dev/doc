---
title: 'Device Provisioning Service'
description: 'Running Device Provisioning Service on Kubernetes'
docsOthersDisplay: true
date: '2023-05-18'
categories: [deployment, kubernetes]
keywords: [deployment, kubernetes, k8s, helm, chart]
weight: 10
hiddenInMenu: true
---

![Deployment diagram](/docs/deployment/device-provisioning-service/static/dps-deployment-diagram.drawio.svg "medium-zoom-image")

To deploy the Device Provisioning Service, you will need to follow the instructions outlined in the provided page to extend the hub helm configuration.

{{< warning >}}
The basic deployment uses Mock OAuth Server, so it shall be used only for test/development purposes. Use with extra care! For production, follow the instructions in [Advanced configuration](/docs/deployment/device-provisioning-service/advanced).
{{< /warning >}}

{{< note >}}
Before deploying the Device Provisioning Service on Kubernetes, make sure to follow the steps in [Hub](/docs/deployment/device-provisioning-service/hub) first. Then apply the changes from this page to the configuration. Once done, you can deploy the hub with the Device Provisioning Service.
{{< /note >}}

For Device Provisioning Service, all configuration values are documented [here](https://github.com/plgd-dev/device-provisioning-service/blob/main/charts/device-provisioning-service/README.md#values).

## Device provider for Device Provisioning Service

When a device is successfully attested against an enrollment group by the Device Provisioning Service, the device's owner needs to be set. The owner is configured in the enrollment group. Therefore, the Device Provisioning Service needs to obtain the authorization code from the OAuth service, which is associated with the owner. When the device contacts the hub, the hub will exchange the authorization code for a JWT access token that contains the owner claim with the owner.

### Client credential flow

In the client credential flow, the OAuth client is linked to its owner. To establish this connection, the client must include the owner's details in the JWT access token by setting the `<OWNER_CLAIM>` owner claim. This access token serves multiple purposes: as a sign-in token, an authorization code for sign-up, and a refresh token to extend the device's access to the hub.

When the hub receives the access token, it extracts the owner's information from it. To obtain a new access token from the OAuth server, the hub appends the owner as query parameters, using the format `<OWNER_CLAIM>=<OWNER>`. This enables the hub to authenticate and retrieve the appropriate access token associated with the owner from the OAuth server.

In the `clientCredentials` flow, you can't use the `sub` claim because the owner is the OAuth client itself. Therefore, you need to utilize another claim to identify the owner. By setting the `<OWNER_CLAIM>` to the same value in both the DPS OAuth client and the WWW OAuth client, you establish a connection between the user and their associated DPS devices. The same connection via  the `<OWNER_CLAIM>` needs to be applied to all OAuth clients used for accessing hub devices.

For a mock OAuth2.0 server, we need to override default the hub configuration's section with new `<OWNER_CLAIM>`, DPS and WWW OAuth clients.

```yaml
global:
  ownerClaim: "https://plgd.dev/owner" # for mockoauthserver
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

{{< warning >}}

For production, you need to set the OAuth server client credential flow, as is described in [Customize OAuth server client credential flow](/docs/deployment/device-provisioning-service/advanced).

{{< /warning >}}

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

In the `deviceProvisioningService.enrollmentGroups[].attestationMechanism.x509.certificateChain` field, please provide the ECDSA certificate chain in PEM format that was used to sign the device manufacturer certificate (IDevId). The certificate chain should include the intermediate CA certificates and the root CA certificate in the order from the closest intermediate CA leaf up to the root CA. In [verify the onboarding device](/docs/deployment/device-provisioning-service/verify-device-onboarding#generate-certificates), in step `4.` this value is set.

{{< note >}}

More information about the enrollment group configuration can be found [here](/docs/configuration/device-provisioning-service/#enrollment_groups).

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

{{< tip >}}

To successfully manage the device from local network, make sure to set the `owner` field to the corresponding `SubjectId` value and configure the `preSharedKey` as `Key` in the login screen of the [plgd/client-application](/docs/quickstart/local-discovery).

![Login to Client Application](/docs/deployment/device-provisioning-service/static/psk-login.png)

If `preSharedKey` is not set, the device will be managed only via certificate authentication.

{{< /tip >}}

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

## Final configuration with mock oauth server

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

## Deployment to production

To deploy the Device Provisioning Service to production, you need to follow [Advanced configuration](/docs/deployment/device-provisioning-service/advanced).
