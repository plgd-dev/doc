---
title: '[MOCK] OAuth Server'
description: 'Service configuration overview'
date: '2021-10-01'
categories: [configuration, deployment]
keywords: [configuration]
weight: 10
---

OAuth2.0 Mock Server used for automated tests, [bundle container](/docs/quickstart/deploy-plgd-hub/#plgd-bundle) and optionally deployment using the HELM chart for the **development purposes**.

## Docker Image

```bash
docker pull ghcr.io/plgd-dev/hub/mock-oauth-server:latest
```

## YAML Configuration

A configuration template is available on [test/oauth-server/config.yaml](https://github.com/plgd-dev/hub/blob/main/test/oauth-server/config.yaml).

### Logging

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `log.level` | string | Logging enabled from level. | `"info"` |
| `log.encoding` | string | Logging format. The supported values are: "json", "console" | `"json"` |
| `log.stacktrace.enabled` | bool | Log stacktrace. | `false` |
| `log.stacktrace.level` | string | Stacktrace from level. | `"warn"` |
| `log.encoderConfig.timeEncoder` | string | Time format for logs. The supported values are: "rfc3339nano", "rfc3339". | `"rfc3339nano"` |

### HTTP API

HTTP API of the OAuth Server service as defined [here](https://github.com/plgd-dev/hub/blob/main/test/oauth-server/uri/uri.go)

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `apis.http.address` | string | Listen specification <host>:<port> for http client connection. | `"0.0.0.0:9100"` |
| `apis.http.readTimeout` | string | The maximum duration for reading the entire request, including the body by the server. A zero or negative value means there will be no timeout. | `8s` |
| `apis.http.readHeaderTimeout` | string | The amount of time allowed to read request headers by the server. If readHeaderTimeout is zero, the value of readTimeout is used. If both are zero, there is no timeout. | `4s` |
| `apis.http.writeTimeout` | string | The maximum duration before the server times out writing of the response. A zero or negative value means there will be no timeout. | `16s` |
| `apis.http.idleTimeout` | string | The maximum amount of time the server waits for the next request when keep-alives are enabled. If idleTimeout is zero, the value of readTimeout is used. If both are zero, there is no timeout. | `30s` |
| `apis.http.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `apis.http.tls.keyFile` | string | File path to the private key in PEM format. | `""` |
| `apis.http.tls.certFile` | string | File path to the certificate in PEM format. | `""` |
| `apis.http.tls.clientCertificateRequired` | bool | If true, require client certificate. | `true` |

### OAuth Signer

Signer configuration to issue ID/access tokens of OAuth provider for mock testing.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `oauthSigner.idTokenKeyFile` | string | File path to a private RSA key in PEM format required for ID token signing. | `""` |
| `oauthSigner.accessTokenKeyFile` | string | File path to a private ECDSA key in PEM format required for access token signing. | `""` |
| `oauthSigner.domain` | string | Domain address <host>:<port> for OAuth APIs. | `""` |
| `oauthSigner.clients[0].id` | string | client id which is used by oauth clients. | `"test"` |
| `oauthSigner.clients[0].authorizationCodeLifetime` | string | validity of generated authorization code. | `"10m0s"` |
| `oauthSigner.clients[0].accessTokenLifetime` | string | validity of generated access token lifetime. 0s means forever. | `"0s"` |
| `oauthSigner.clients[0].codeRestrictionLifetime` | string | define period how long cannot be used again authorization code in exchange after first use. | `"0s"` |

{{< note >}}

Note that the string type related to time (i.e. timeout, idleConnTimeout, expirationTime) is decimal numbers, each with optional fraction and a unit suffix, such as "300ms", "1.5h" or "2h45m". Valid time units are "ns", "us", "ms", "s", "m", "h".

{{< /note >}}
