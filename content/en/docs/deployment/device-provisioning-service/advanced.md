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
      clientID: "<MY_DPS_CLIENT_ID>"
      clientSecret: "<MY_DPS_CLIENT_SECRET>"
      grantType: "clientCredentials"
      scopes: ['openid']
      audience: "https://api.example.com"
    ...
    web:
      clientID: "<MY_WWW_CLIENT_ID>"
      audience: "https://example.com"
```

### Integration with KeyCloak OAuth server

This section provides instructions on how to integrate with the KeyCloak OAuth server. The integration involves configuring the DPS OAuth client and the WWW OAuth client to use the same value for the `<OWNER_CLAIM>` claim. By following these steps, you will ensure that the appropriate user is identified as the owner of DPS devices.

To begin, you need to locate the `user id` within the KeyCloak OAuth server. This user will be designated as the owner of the DPS devices. Once you have obtained the `user id`, you will substitute `<OWNER>` with this value in the subsequent instructions.

{{< note >}}

At the end of the integration process, you will establish a connection between the OAuth DPS client and the user. By default, the association is based on the `<OWNER_CLAIM>` value. However, if you wish to change this behavior, you have the option to create a custom mapper.

To create a custom mapper, you can define a specific logic that sets the `<OWNER_CLAIM>` to the appropriate value for your requirements. This allows you to tailor the association between the user and the OAuth DPS client according to your specific needs.

In the process of acquiring an device access token from the OAuth server, the Device Provisioning Service incorporates a query parameter in the request called `<OWNER_CLAIM>=<OWNER>`. This parameter specifies the desired `<OWNER_CLAIM>` value to be associated with the access token.

{{< /note >}}

1. Create an OAuth client for DPS in KeyCloak with the following configuration:

   - Settings:
     - Enabled: On
     - Client Protocol: openid-connect
     - Access Type: confidential
     - Service Accounts Enabled: On
     - Authorization Enabled: On

   - Credentials:
     - Client Authenticator: Client Id and Secret
     - Secret: `<MY_DPS_CLIENT_SECRET>`

   - Mapper:
     - Create a custom `Hardcoded claim` mapper:
       - Token Claim Name: `<OWNER_CLAIM>`
       - Claim value: `<OWNER>`
       - Claim JSON Type: String
       - Add to access token: On
       - Add to userinfo: On

2. Create a WWW OAuth client with a mapper that adds the `<OWNER_CLAIM>` claim to the user JWT token. You can use the `User Property` mapper with the following configuration to map the `id` property to the `<OWNER_CLAIM>` claim:

   - Settings:
     - Enabled: On
     - Client Protocol: openid-connect
     - Access Type: public
     - Standard Flow Enabled: On
     - Valid Redirect URIs: `[ https://www.example.com/*,... ]`
     - Backchannel Logout Session Required: On
     - OpenID Connect Compatibility Modes:
       - Use Refresh Tokens: On

   - Mapper:
     - Property: id
     - Token Claim Name: `<OWNER_CLAIM>`
     - Claim JSON Type: String
     - Add to ID token: On
     - Add to access token: On
     - Add to userinfo: On

In the helm chart, add the following configuration:

```yaml
global:
  ownerClaim: `<OWNER_CLAIM>`
  authority: `<KEYCLOAK_REALM_ADDRESS>`
  oauth:
    device:
    ...
    - name: "plgd.dps"
      clientID: "<MY_DPS_CLIENT_ID>"
      clientSecret: "<MY_DPS_CLIENT_SECRET>"
      grantType: "clientCredentials"
      scopes: ['openid']
      audience: "https://api.example.com"
      redirectURL: "cloud.plgd.dps://login-callback"
    web:
      clientID: "<MY_WWW_CLIENT_ID>"
      audience: "https://example.com"
mockoauthserver:
  enabled: false
deviceProvisioningService:
  enrollmentGroups:
  - owner: "<OWNER>"
    hub:
      authorization:
        provider:
          name: "plgd.dps"
          clientId: <MY_DPS_CLIENT_ID>
          clientSecret: <MY_DPS_CLIENT_ID>
          audience: "https://api.example.com"
          scopes: ["openid"]
```

Make sure to replace `<KEYCLOAK_REALM_ADDRESS>`, `<MY_CLIENT_SECRET>`, `<OWNER_CLAIM>`, `<OWNER>`, `<MY_DPS_CLIENT_ID>`, and `<MY_DPS_CLIENT_SECRET>` with the appropriate values for your setup.

To apply the modifications, please follow the instructions provided in the [Deploy the hub with Device Provisioning Service](/docs/deployment/device-provisioning-service/deployment.md#deploy-the-hub-with-device-provisioning-service).

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
