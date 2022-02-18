---
title: '[MOCK] OAuth Server'
description: 'Service configuration overview'
date: '2021-10-01'
lastmod: '2021-10-01'
categories: [configuration, deployment]
keywords: [configuration]
menu:
  docs:
    parent: configuration
    weight: 100
toc: true
---

OAuth2.0 Mock Server used for automated tests, [bundle container](../quickstart/deploy-plgd-hub/#plgd-bundle) and optionally deployment using the HELM chart for the **development purposes**.

## Docker Image

```bash
docker pull plgd/mock-oauth-server:latest
```

## YAML Configuration

A configuration template is available on [test/oauth-server/config.yaml](https://github.com/plgd-dev/hub/blob/main/test/oauth-server/config.yaml).

### Logging

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `log.level` | string | `Logging enabled from level.` | `"info"` |
| `log.encoding` | string | `Logging format. The supported values are: "json", "console"` | `"json"` |
| `log.stacktrace.enabled` | bool | `Log stacktrace.` | `"false` |
| `log.stacktrace.level` | string | `Stacktrace from level.` | `"warn` |
| `log.encoderConfig.timeEncoder` | string | `Time format for logs. The supported values are: "rfc3339nano", "rfc3339".` | `"rfc3339nano` |

### HTTP API

HTTP API of the OAuth Server service as defined [here](https://github.com/plgd-dev/hub/blob/main/test/oauth-server/uri/uri.go)

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `api.http.address` | string | `Listen specification <host>:<port> for http client connection.` | `"0.0.0.0:9100"` |
| `api.http.tls.caPool` | string | `File path to the root certificate in PEM format which might contain multiple certificates in a single file.` |  `""` |
| `api.http.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `api.http.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `api.http.tls.clientCertificateRequired` | bool | `If true, require client certificate.` | `true` |

### OAuth Signer

Signer configuration to issue ID/access tokens of OAuth provider for mock testing.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `oauthSigner.idTokenKeyFile` | string | `File path to a private RSA key in PEM format required for ID token signing.` | `""` |
| `oauthSigner.accessTokenKeyFile` | string | `File path to a private ECDSA key in PEM format required for access token signing.` | `""` |
| `oauthSigner.domain` | string | `Domain address <host>:<port> for OAuth APIs.` | `""` |
| `oauthSigner.clients[0].id` | string | `client id which is used by oauth clients.` | `"test"` |
| `oauthSigner.clients[0].authorizationCodeLifetime` | string | `validity of generated authorization code` | `"10m0s"` |
| `oauthSigner.clients[0].accessTokenLifetime` | string | `validity of generated access token lifetime. 0s means forever.` | `"0s"` |
| `oauthSigner.clients[0].codeRestrictionLifetime` | string | `define period how long cannot be used again authorization code in exchange after first use.` | `"0s"` |

> Note that the string type related to time (i.e. timeout, idleConnTimeout, expirationTime) is decimal numbers, each with optional fraction and a unit suffix, such as "300ms", "1.5h" or "2h45m". Valid time units are "ns", "us", "ms", "s", "m", "h".
