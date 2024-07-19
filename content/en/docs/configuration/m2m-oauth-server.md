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

### Open telemetry exporter

The plgd open telemetry exporter configuration.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.openTelemetryCollector.grpc.enabled` | bool | `Enable OTLP gRPC exporter` | `false` |
| `clients.openTelemetryCollector.grpc.address` | string | `The gRPC collector to which the exporter is going to send data` | `""` |
| `clients.openTelemetryCollector.grpc.keepAlive.time` | string | `After a duration of this time if the client doesn't see any activity it pings the server to see if the transport is still alive.` | `10s` |
| `clients.openTelemetryCollector.grpc.keepAlive.timeout` | string | `After having pinged for keepalive check, the client waits for a duration of Timeout and if no activity is seen even after that the connection is closed.` | `20s` |
| `clients.openTelemetryCollector.grpc.keepAlive.permitWithoutStream` | bool | `If true, client sends keepalive pings even with no active RPCs. If false, when there are no active RPCs, Time and Timeout will be ignored and no keepalive pings will be sent.` | `true` |
| `clients.openTelemetryCollector.grpc.tls.caPool` | string | `File path to the root certificate in PEM format which might contain multiple certificates in a single file.` |  `""` |
| `clients.openTelemetryCollector.grpc.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `clients.openTelemetryCollector.grpc.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `clients.openTelemetryCollector.grpc.tls.useSystemCAPool` | bool | `If true, use system certification pool.` | `false` |

### OAuth Signer

Signer configuration to access tokens of OAuth provider for services.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `oauthSigner.privateKeyFile` | string | `File path to a private ECDSA key in PEM format required for access token signing.` | `""` |
| `oauthSigner.domain` | string | `Domain address <host>:<port> for OAuth APIs.` | `""` |
| `oauthSigner.ownerClaim` | string | `Claim name in the token which contains the owner of the token.` | `"sub"` |
| `oauthSigner.deviceIDClaim` | string | `Claim name in the token which contains the deviceID of the token.` | `""` |
| `oauthSigner.clients[].id` | string | `client id which is used by oauth clients.` | `"test"` |
| `oauthSigner.clients[].secret` | string | `client secret which is used by oauth clients. Can be empty when .jwtPrivateKey is set` | `""` |
| `oauthSigner.clients[].accessTokenLifetime` | string | `validity of generated access token lifetime. 0s means forever.` | `"0s"` |
| `oauthSigner.clients[].allowedGrantTypes` | []string | `grant types which are allowed for the client. Only 'client_credentials' is supported.` | `[ "client_credentials" ]` |
| `oauthSigner.clients[].allowedAudiences` | []string | `audiences which are allowed for the client.` | `[]` |
| `oauthSigner.clients[].allowedScopes` | []string | `scopes which are allowed for the client.` | `[]` |
| `oauthSigner.clients[].insertTokenClaims` | object | `claims which are added to the token.` | `{}` |
| `oauthSigner.clients[].jwtPrivateKey.enabled` | bool | `allow JWT private key for client` | `false` |
| `oauthSigner.clients[].jwtPrivateKey.authorization.endpoints[].authority` | string | `address to OAuth authority` | `""` |
| `oauthSigner.clients[].jwtPrivateKey.authorization.endpoints[].maxIdleConns` | int | `It controls the maximum number of idle (keep-alive) connections across all hosts. Zero means no limit.` | `16` |
| `oauthSigner.clients[].jwtPrivateKey.authorization.endpoints[].maxConnsPerHost` | int | `It optionally limits the total number of connections per host, including connections in the dialing, active, and idle states. On limit violation, dials will block. Zero means no limit.` | `32` |
| `oauthSigner.clients[].jwtPrivateKey.authorization.endpoints[].maxIdleConnsPerHost` | int | `If non-zero, controls the maximum idle (keep-alive) connections to keep per-host. If zero, DefaultMaxIdleConnsPerHost is used.` | `16` |
| `oauthSigner.clients[].jwtPrivateKey.authorization.endpoints[].idleConnTimeout` | string | `The maximum amount of time an idle (keep-alive) connection will remain idle before closing itself. Zero means no limit.` | `30s` |
| `oauthSigner.clients[].jwtPrivateKey.authorization.endpoints[].timeout` | string | `A time limit for requests made by this Client. A Timeout of zero means no timeout.` | `10s` |
| `oauthSigner.clients[].jwtPrivateKey.authorization.endpoints[].tls.caPool` | []string | `File paths to the root certificates in PEM format. The file may contain multiple certificates.` |  `[]` |
| `oauthSigner.clients[].jwtPrivateKey.authorization.endpoints[].tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `oauthSigner.clients[].jwtPrivateKey.authorization.endpoints[].tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `oauthSigner.clients[].jwtPrivateKey.authorization.endpoints[].tls.useSystemCAPool` | bool | `If true, use system certification pool.` | `false` |

{{< note >}}

Note that the string type related to time (i.e. timeout, idleConnTimeout, expirationTime) is decimal numbers, each with optional fraction and a unit suffix, such as "300ms", "1.5h" or "2h45m". Valid time units are "ns", "us", "ms", "s", "m", "h".

{{< /note >}}
