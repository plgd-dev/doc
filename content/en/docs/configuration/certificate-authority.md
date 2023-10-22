---
title: 'Certificate Authority'
description: 'Service configuration overview'
date: '2021-10-01'
categories: [configuration, deployment]
keywords: [configuration]
weight: 7
---

This service is used to sign identity certificates for devices and [plgd-dev/device](https://github.com/plgd-dev/device) client.

Initialization of the SDK requires an Identity Certificate issued by this service after successful user authorization. The SDK is then able to discover and configure the ownership as described [here](https://openconnectivity.org/specs/OCF_Security_Specification_v2.2.3.pdf#page=37). During this process, a CSR (Certificate Signing Request) is sent to the Certificate Authority which returns device's Identity Certificate.

## Docker Image

```bash
docker pull ghcr.io/plgd-dev/hub/certificate-authority:latest
```

## YAML Configuration

A configuration template is available on [certificate-authority/config.yaml](https://github.com/plgd-dev/hub/blob/main/certificate-authority/config.yaml).

### Logging

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `log.level` | string | `Logging enabled from level.` | `"info"` |
| `log.encoding` | string | `Logging format. The supported values are: "json", "console"` | `"json"` |
| `log.stacktrace.enabled` | bool | `Log stacktrace.` | `false` |
| `log.stacktrace.level` | string | `Stacktrace from level.` | `"warn"` |
| `log.encoderConfig.timeEncoder` | string | `Time format for logs. The supported values are: "rfc3339nano", "rfc3339".` | `"rfc3339nano"` |

### gRPC API

gRPC API of the Certificate Authority service as defined [here](https://github.com/plgd-dev/hub/blob/main/certificate-authority/pb/service_grpc.pb.go#L19).

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `apis.grpc.address` | string | `Listen specification <host>:<port> for grpc client connection.` | `"0.0.0.0:9100"` |
| `apis.grpc.enforcementPolicy.minTime` | string | `The minimum amount of time a client should wait before sending a keepalive ping. Otherwise the server close connection.` | `5s`|
| `apis.grpc.enforcementPolicy.permitWithoutStream` | bool |  `If true, server allows keepalive pings even when there are no active streams(RPCs). Otherwise the server close connection.`  | `true` |
| `apis.grpc.keepAlive.maxConnectionIdle` | string | `A duration for the amount of time after which an idle connection would be closed by sending a GoAway. 0s means infinity.` | `0s` |
| `apis.grpc.keepAlive.maxConnectionAge` | string | `A duration for the maximum amount of time a connection may exist before it will be closed by sending a GoAway. 0s means infinity.` | `0s` |
| `apis.grpc.keepAlive.maxConnectionAgeGrace` | string | `An additive period after MaxConnectionAge after which the connection will be forcibly closed. 0s means infinity.` | `0s` |
| `apis.grpc.keepAlive.time` | string | `After a duration of this time if the server doesn't see any activity it pings the client to see if the transport is still alive.` | `2h` |
| `apis.grpc.keepAlive.timeout` | string | `After having pinged for keepalive check, the client waits for a duration of Timeout and if no activity is seen even after that the connection is closed.` | `20s` |
| `apis.grpc.tls.caPool` | string | `File paths to the root certificates in PEM format. The file may contain multiple certificates.` |  `[]` |
| `apis.grpc.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `apis.grpc.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `apis.grpc.tls.clientCertificateRequired` | bool | `If true, require client certificate.` | `true` |
| `apis.grpc.authorization.ownerClaim` | string | `Claim used to identify owner of the device.` | `"sub"` |
| `apis.grpc.authorization.authority` | string | `Authority is the address of the token-issuing authentication server. Services will use this URI to find and retrieve the public key that can be used to validate the token’s signature.` | `""` |
| `apis.grpc.authorization.audience` | string | `Identifier of the API configured in your OAuth provider.` | `""` |
| `apis.grpc.authorization.http.maxIdleConns` | int | `It controls the maximum number of idle (keep-alive) connections across all hosts. Zero means no limit.` | `16` |
| `apis.grpc.authorization.http.maxConnsPerHost` | int | `It optionally limits the total number of connections per host, including connections in the dialing, active, and idle states. On limit violation, dials will block. Zero means no limit.` | `32` |
| `apis.grpc.authorization.http.maxIdleConnsPerHost` | int | `If non-zero, controls the maximum idle (keep-alive) connections to keep per-host. If zero, DefaultMaxIdleConnsPerHost is used.` | `16` |
| `apis.grpc.authorization.http.idleConnTimeout` | string | `The maximum amount of time an idle (keep-alive) connection will remain idle before closing itself. Zero means no limit.` | `30s` |
| `apis.grpc.authorization.http.timeout` | string | `A time limit for requests made by this Client. A Timeout of zero means no timeout.` | `10s` |
| `apis.grpc.authorization.http.tls.caPool` | string | `File paths to the root certificates in PEM format. The file may contain multiple certificates.` |  `[]` |
| `apis.grpc.authorization.http.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `apis.grpc.authorization.http.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `apis.grpc.authorization.http.tls.useSystemCAPool` | bool | `If true, use system certification pool.` | `false` |

### HTTP API

 HTTP Certificate Authority APIs as defined by [swagger](https://petstore.swagger.io/?url=https://raw.githubusercontent.com/plgd-dev/hub/main/certificate-authority/pb/service.swagger.json).
 The configuration `apis.http.tls` and `apis.http.authorization` are inherited from the gRPC API.

 | Property | Type | Description | Default |
 | ---------- | -------- | -------------- | ------- |
 | `apis.http.address` | string | `Listen specification <host>:<port> for http client connection.` | `"0.0.0.0:9101"` |
 | `apis.http.readTimeout` | string | `The maximum duration for reading the entire request, including the body by the server. A zero or negative value means there will be no timeout.` | `8s` |
 | `apis.http.readHeaderTimeout` | string | `The amount of time allowed to read request headers by the server. If readHeaderTimeout is zero, the value of readTimeout is used. If both are zero, there is no timeout.` | `4s` |
 | `apis.http.writeTimeout` | string | `The maximum duration before the server times out writing of the response. A zero or negative value means there will be no timeout.` | `16s` |
 | `apis.http.idleTimeout` | string | `The maximum amount of time the server waits for the next request when keep-alives are enabled. If idleTimeout is zero, the value of readTimeout is used. If both are zero, there is no timeout.` | `30s` |

### Signer

Signer configuration to issue identity certificates for devices or client application.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `signer.keyFile` | string | `File path to the signer private key in PEM format.` |  `""` |
| `signer.certFile` | string | `File path to the signer certificate in PEM format.` |  `""` |
| `signer.validFrom` | string | `The time from when the certificate is valid. (Format: https://github.com/karrick/tparse)` |  `"now-1h"` |
| `signer.expiresIn` | string | `The time up to which the certificate is valid.` |  `"87600h"` |
| `signer.hubID` | string | `Hub ID which is stored in coap-gw certificate and it cannot be used in the common name in the CSR.` | `""` |

{{< note >}}

Note that the string type related to time (i.e. timeout, idleConnTimeout, expirationTime) is decimal numbers, each with optional fraction and a unit suffix, such as "300ms", "1.5h" or "2h45m". Valid time units are "ns", "us", "ms", "s", "m", "h".

{{< /note >}}
