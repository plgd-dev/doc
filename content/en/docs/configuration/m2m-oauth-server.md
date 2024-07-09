---
title: 'Machine to Machine OAuth Server'
description: 'Service configuration overview'
date: '2024-07-04'
categories: [configuration, deployment]
keywords: [configuration]
weight: 10
---

Machine to Machine OAuth Server (M2M OAuth Server) is used to generate API tokens to access the plgd services by other services. These tokens are acquired via client credentials flow with client assertion type JWT or secret in the case of service.

## Docker Image

```bash
docker pull ghcr.io/plgd-dev/hub/m2m-oauth-server:latest
```

## YAML Configuration

A configuration template is available on [m2m-oauth-server/config.yaml](https://github.com/plgd-dev/hub/blob/main/m2m-oauth-server/config.yaml).

### Logging

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `log.level` | string | `Logging enabled from level.` | `"info"` |
| `log.encoding` | string | `Logging format. The supported values are: "json", "console"` | `"json"` |
| `log.stacktrace.enabled` | bool | `Log stacktrace.` | `false` |
| `log.stacktrace.level` | string | `Stacktrace from level.` | `"warn"` |
| `log.encoderConfig.timeEncoder` | string | `Time format for logs. The supported values are: "rfc3339nano", "rfc3339".` | `"rfc3339nano"` |

### HTTP API

HTTP API of the OAuth Server service as defined [here](https://github.com/plgd-dev/hub/blob/main/m2m-oauth-server/uri/uri.go)

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `apis.http.address` | string | `Listen to specification <host>:<port> for http client connection.` | `"0.0.0.0:9100"` |
| `apis.http.readTimeout` | string | `The maximum duration for reading the entire request, including the body by the server. A zero or negative value means there will be no timeout.` | `8s` |
| `apis.http.readHeaderTimeout` | string | `The amount of time allowed to read request headers by the server. If readHeaderTimeout is zero, the value of readTimeout is used. If both are zero, there is no timeout.` | `4s` |
| `apis.http.writeTimeout` | string | `The maximum duration before the server times out writing of the response. A zero or negative value means there will be no timeout.` | `16s` |
| `apis.http.idleTimeout` | string | `The maximum amount of time the server waits for the next request when keep-alives are enabled. If idleTimeout is zero, the value of readTimeout is used. If both are zero, there is no timeout.` | `30s` |
| `apis.http.tls.caPool` | []string | `File paths to the root certificates in PEM format. The file may contain multiple certificates.` |  `[]` |
| `apis.http.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `apis.http.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `apis.http.tls.clientCertificateRequired` | bool | `If true, require client certificate.` | `true` |
| `oauthSigner.clients[].secret` | string | `client secret which is used by oauth clients. It can be empty when .jwtPrivateKey is set` | `""` |
{{< note >}}

Note that the string type related to time (i.e. timeout, idleConnTimeout, expirationTime) is decimal numbers, each with optional fraction and a unit suffix, such as "300ms", "1.5h" or "2h45m". Valid time units are "ns", "us", "ms", "s", "m", "h".

{{< /note >}}
