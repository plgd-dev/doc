---
title: 'Advanced security'
description: 'How to secure access devices to coap-gateway'
date: '2021-09-17'
lastmod: '2021-09-17'
categories: [developer guide, dashboard]
keywords: [developer guide, dashboard]
menu:
  docs:
    parent: developer-guide
    weight: 10
toc: true
---


## Setup mutual TLS

At coap-gateway you can set up mutual TLS, which verify the signature of the identity certificate of the device via configuration:

```yaml
api:
  coap:
    tls:
      # default
      clientCertificateRequired: true
```

After that, only devices which have signed identity certificates by CA configured in coap-gateway can access the hub.

## How coap-gateway resolves device ID

When the device makes one of the calls sign up, sign in, sign out or sign off, coap-gateway needs to resolve the device ID.

1. If coap-gateway has set `api.coap.authorization.deviceIDClaim` then it will be resolved from JWT token. If JWT token doesn't contain `https://<YOUR_DOMAIN>/deviceId` it returns code `Unauthorized` and closes the connection.
2. If coap-gateway has set mutual TLS `api.coap.tls.clientCertificateRequired` then it will be resolved from the device identity certificate.
3. If none of the previous options are set, device ID will be resolved from the request parameter.

## Don't allow access with a token that doesn't bellow the device

When `api.coap.tls.clientCertificateRequired` and `api.coap.authorization.deviceIDClaim` are set, coap-gateway matches deviceID from certificate and JWT token. If they are not the same, then coap-gateway returns code `Unauthorized` and closes the connection.

## How to push deviceId to the token with auth0

First, you need to create a rule at `Auth pipeline->Rules` with code:

```js
function (user, context, callback) {
  var deviceIdClaim = 'https://<YOUR_DOMAIN>/deviceId';
  var deviceId = (context && context.request && context.request.query && context.request.query.device_id) || null;
  if (deviceId) {
    context.accessToken[deviceIdClaim] = deviceId;
  }
  return callback(null, user, context);
}
```

After that, if you call authorize endpoint to obtain authorization code for a device with query parameter `device_id=<deviceId>`,
and the device makes sign up with that code, the returned JWT access token will contain deviceId claim like `https://<YOUR_DOMAIN>/deviceId: <deviceId>`.

For validation of device ID claim by coap-gateway the `api.coap.authorization.deviceIDClaim` must be set to `https://<YOUR_DOMAIN>/deviceId`.
