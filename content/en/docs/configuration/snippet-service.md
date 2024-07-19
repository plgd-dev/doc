---
title: 'Snippet service'
description: 'Service configuration overview'
date: '2024-07-12'
categories: [configuration, deployment]
keywords: [configuration]
weight: 4
---

Snippet service is used to automatically update resources in the device when conditions are satisfied or on-demand by the user.

## Docker Image

```bash
docker pull ghcr.io/plgd-dev/hub/snippet-service:latest
```

## YAML Configuration

A configuration template is available on [snippet-service/config.yaml](https://github.com/plgd-dev/hub/blob/main/snippet-service/config.yaml).

### Logging

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `log.level` | string | `Logging enabled from level.` | `"info"` |
| `log.encoding` | string | `Logging format. The supported values are: "json", "console"` | `"json"` |
| `log.stacktrace.enabled` | bool | `Log stacktrace.` | `false` |
| `log.stacktrace.level` | string | `Stacktrace from level.` | `"warn"` |
| `log.encoderConfig.timeEncoder` | string | `Time format for logs. The supported values are: "rfc3339nano", "rfc3339".` | `"rfc3339nano"` |

### gRPC API

gRPC API of the Snippet service as defined [here](https://github.com/plgd-dev/hub/blob/main/snippet-service/pb/service.proto).

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `apis.grpc.address` | string | `Listen specification <host>:<port> for grpc client connection.` | `"0.0.0.0:9100"` |
| `apis.grpc.sendMsgSize` | int32 | `Set the max message size in bytes the server can send. 0 means 2147483647` | `4194304` |
| `apis.grpc.recvMsgSize` | int32 | `Set the max message size in bytes the server can receive. 0 means 4194304` | `4194304` |
| `apis.grpc.enforcementPolicy.minTime` | string | `The minimum amount of time a client should wait before sending a keepalive ping. Otherwise the server close connection.` | `5s`|
| `apis.grpc.enforcementPolicy.permitWithoutStream` | bool |  `If true, server allows keepalive pings even when there are no active streams(RPCs). Otherwise the server close connection.`  | `true` |
| `apis.grpc.keepAlive.maxConnectionIdle` | string | `A duration for the amount of time after which an idle connection would be closed by sending a GoAway. 0s means infinity.` | `0s` |
| `apis.grpc.keepAlive.maxConnectionAge` | string | `A duration for the maximum amount of time a connection may exist before it will be closed by sending a GoAway. 0s means infinity.` | `0s` |
| `apis.grpc.keepAlive.maxConnectionAgeGrace` | string | `An additive period after MaxConnectionAge after which the connection will be forcibly closed. 0s means infinity.` | `0s` |
| `apis.grpc.keepAlive.time` | string | `After a duration of this time if the server doesn't see any activity it pings the client to see if the transport is still alive.` | `2h` |
| `apis.grpc.keepAlive.timeout` | string | `After having pinged for keepalive check, the client waits for a duration of Timeout and if no activity is seen even after that the connection is closed.` | `20s` |
| `apis.grpc.tls.caPool` | []string | `File paths to the root certificates in PEM format. The file may contain multiple certificates.` |  `[]` |
| `apis.grpc.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `apis.grpc.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `apis.grpc.tls.clientCertificateRequired` | bool | `If true, require client certificate.` | `true` |
| `apis.grpc.authorization.ownerClaim` | string | `Claim used to identify owner of the device.` | `"sub"` |
| `apis.grpc.authorization.audience` | string | `Identifier of the API configured in your OAuth provider.` | `""` |
| `apis.grpc.authorization.endpoints[].authority` | string | `Authority is the address of the token-issuing authentication server. Services will use this URI to find and retrieve the public key that can be used to validate the token’s signature.` | `""` |
| `apis.grpc.authorization.endpoints[].http.maxIdleConns` | int | `It controls the maximum number of idle (keep-alive) connections across all hosts. Zero means no limit.` | `16` |
| `apis.grpc.authorization.endpoints[].http.maxConnsPerHost` | int | `It optionally limits the total number of connections per host, including connections in the dialing, active, and idle states. On limit violation, dials will block. Zero means no limit.` | `32` |
| `apis.grpc.authorization.endpoints[].http.maxIdleConnsPerHost` | int | `If non-zero, controls the maximum idle (keep-alive) connections to keep per-host. If zero, DefaultMaxIdleConnsPerHost is used.` | `16` |
| `apis.grpc.authorization.endpoints[].http.idleConnTimeout` | string | `The maximum amount of time an idle (keep-alive) connection will remain idle before closing itself. Zero means no limit.` | `30s` |
| `apis.grpc.authorization.endpoints[].http.timeout` | string | `A time limit for requests made by this Client. A Timeout of zero means no timeout.` | `10s` |
| `apis.grpc.authorization.endpoints[].http.tls.caPool` | []string | `File paths to the root certificates in PEM format. The file may contain multiple certificates.` |  `[]` |
| `apis.grpc.authorization.endpoints[].http.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `apis.grpc.authorization.endpoints[].http.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `apis.grpc.authorization.endpoints[].http.tls.useSystemCAPool` | bool | `If true, use system certification pool.` | `false` |

### HTTP API

HTTP Snippet service APIs as defined by [swagger](https://petstore.swagger.io/?url=https://raw.githubusercontent.com/plgd-dev/hub/main/snippet-service/pb/service.swagger.json).
The configuration `apis.http.tls` and `apis.http.authorization` are inherited from the gRPC API.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `apis.http.address` | string | `Listen specification <host>:<port> for http client connection.` | `"0.0.0.0:9101"` |
| `apis.http.readTimeout` | string | `The maximum duration for reading the entire request, including the body by the server. A zero or negative value means there will be no timeout.` | `8s` |
| `apis.http.readHeaderTimeout` | string | `The amount of time allowed to read request headers by the server. If readHeaderTimeout is zero, the value of readTimeout is used. If both are zero, there is no timeout.` | `4s` |
| `apis.http.writeTimeout` | string | `The maximum duration before the server times out writing of the response. A zero or negative value means there will be no timeout.` | `16s` |
| `apis.http.idleTimeout` | string | `The maximum amount of time the server waits for the next request when keep-alives are enabled. If idleTimeout is zero, the value of readTimeout is used. If both are zero, there is no timeout.` | `30s` |

### Event Bus

plgd hub uses NATS messaging system as a event bus to be notified about device changes to trigger conditions evaluation.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.eventBus.nats.url` | string | `URL to nats messaging system.` | `"nats://localhost:4222"` |
| `clients.eventBus.nats.pendingLimits.msgLimit` | int | `Limit number of messages in queue. -1 means unlimited` | `524288` |
| `clients.eventBus.nats.pendingLimits.bytesLimit` | int | `Limit buffer size of queue. -1 means unlimited` | `67108864` |
| `clients.eventBus.nats.tls.caPool` | []string | `File paths to the root certificates in PEM format. The file may contain multiple certificates.` |  `[]` |
| `clients.eventBus.nats.tls.keyFile` | string | `File name of private key in PEM format.` | `""` |
| `clients.eventBus.nats.tls.certFile` | string | `File name of certificate in PEM format.` | `""` |
| `clients.eventBus.nats.tls.useSystemCAPool` | bool | `If true, use system certification pool.` | `false` |

### Storage

plgd hub uses MongoDB database as a storage for conditions, configurations and applied configurations.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.storage.cleanUpExpiredUpdates` | string | `Cron expression to clean up expired updates.` | `"0 * * * *"` |
| `clients.storage.mongoDB.uri` | string | `URI to mongo database.` | `"mongodb://localhost:27017"` |
| `clients.storage.mongoDB.database` | string | `Name of database` | `"snippetService"` |
| `clients.storage.mongoDB.maxPoolSize` | int | `Limits number of connections.` | `16` |
| `clients.storage.mongoDB.maxConnIdleTime` | string | `Close connection when idle time reach the value.` | `4m` |
| `clients.storage.mongoDB.tls.caPool` | []string | `File paths to the root certificates in PEM format. The file may contain multiple certificates.` |  `[]` |
| `clients.storage.mongoDB.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `clients.storage.mongoDB.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `clients.storage.mongoDB.tls.useSystemCAPool` | bool | `If true, use system certification pool.` | `false` |

### Resource Aggregate Client

Resource Aggregate client is responsible for updating resources in the device when conditions are satisfied or on-demand by the user.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.resourceAggregate.grpc.address` | string | `Address of the Resource Aggregate.` | `""` |
| `clients.resourceAggregate.grpc.sendMsgSize` | int32 | `Set the max message size in bytes the server can send. 0 means 2147483647` | `4194304` |
| `clients.resourceAggregate.grpc.recvMsgSize` | int32 | `Set the max message size in bytes the server can receive. 0 means 4194304` | `4194304` |
| `clients.resourceAggregate.grpc.enforcementPolicy.minTime` | string | `The minimum amount of time a client should wait before sending a keepalive ping. Otherwise the server close connection.` | `5s`|
| `clients.resourceAggregate.grpc.enforcementPolicy.permitWithoutStream` | bool |  `If true, server allows keepalive pings even when there are no active streams(RPCs). Otherwise the server close connection.`  | `true` |
| `clients.resourceAggregate.grpc.keepAlive.maxConnectionIdle` | string | `A duration for the amount of time after which an idle connection would be closed by sending a GoAway. 0s means infinity.` | `0s` |
| `clients.resourceAggregate.grpc.keepAlive.maxConnectionAge` | string | `A duration for the maximum amount of time a connection may exist before it will be closed by sending a GoAway. 0s means infinity.` | `0s` |
| `clients.resourceAggregate.grpc.keepAlive.maxConnectionAgeGrace` | string | `An additive period after MaxConnectionAge after which the connection will be forcibly closed. 0s means infinity.` | `0s` |
| `clients.resourceAggregate.grpc.keepAlive.time` | string | `After a duration of this time if the server doesn't see any activity it pings the client to see if the transport is still alive.` | `2h` |
| `clients.resourceAggregate.grpc.keepAlive.timeout` | string | `After having pinged for keepalive check, the client waits for a duration of Timeout and if no activity is seen even after that the connection is closed.` | `20s` |
| `clients.resourceAggregate.grpc.tls.caPool` | []string | `File paths to the root certificates in PEM format. The file may contain multiple certificates.` |  `[]` |
| `clients.resourceAggregate.grpc.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `clients.resourceAggregate.grpc.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `clients.resourceAggregate.grpc.tls.useSystemCAPool` | bool | `If true, use system certification pool.` | `false` |

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

{{< note >}}

Note that the string type related to time (i.e. timeout, idleConnTimeout, expirationTime) is decimal numbers, each with optional fraction and a unit suffix, such as "300ms", "1.5h" or "2h45m". Valid time units are "ns", "us", "ms", "s", "m", "h".

{{< /note >}}
