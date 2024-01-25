---
title: 'Identity Store'
description: 'Service configuration overview'
date: '2021-10-04'
categories: [configuration, deployment]
keywords: [configuration]
weight: 6
---

Identity Store manages devices and their owners.

## Docker Image

```bash
docker pull ghcr.io/plgd-dev/hub/identity-store:latest
```

## YAML Configuration

A configuration template is available on [identity-store/config.yaml](https://github.com/plgd-dev/hub/blob/main/identity-store/config.yaml).

### Logging

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `log.level` | string | Logging enabled from level. | `"info"` |
| `log.encoding` | string | Logging format. The supported values are: "json", "console" | `"json"` |
| `log.stacktrace.enabled` | bool | Log stacktrace. | `false` |
| `log.stacktrace.level` | string | Stacktrace from level. | `"warn"` |
| `log.encoderConfig.timeEncoder` | string | Time format for logs. The supported values are: "rfc3339nano", "rfc3339". | `"rfc3339nano"` |

### gRPC API

gRPC API of the Identity Store service as defined [here](https://github.com/plgd-dev/hub/blob/main/identity-store/pb/service_grpc.pb.go#L19).

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `apis.grpc.address` | string | Listen specification <host>:<port> for grpc client connection. | `"0.0.0.0:9100"` |
| `apis.grpc.enforcementPolicy.minTime` | string | The minimum amount of time a client should wait before sending a keepalive ping. Otherwise the server close connection. | `5s` |
| `apis.grpc.enforcementPolicy.permitWithoutStream` | bool | If true, server allows keepalive pings even when there are no active streams(RPCs). Otherwise the server close connection. | `true` |
| `apis.grpc.keepAlive.maxConnectionIdle` | string | A duration for the amount of time after which an idle connection would be closed by sending a GoAway. 0s means infinity. | `0s` |
| `apis.grpc.keepAlive.maxConnectionAge` | string | A duration for the maximum amount of time a connection may exist before it will be closed by sending a GoAway. 0s means infinity. | `0s` |
| `apis.grpc.keepAlive.maxConnectionAgeGrace` | string | An additive period after MaxConnectionAge after which the connection will be forcibly closed. 0s means infinity. | `0s` |
| `apis.grpc.keepAlive.time` | string | After a duration of this time if the server doesn't see any activity it pings the client to see if the transport is still alive. | `2h` |
| `apis.grpc.keepAlive.timeout` | string | After having pinged for keepalive check, the client waits for a duration of Timeout and if no activity is seen even after that the connection is closed. | `20s` |
| `apis.grpc.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `apis.grpc.tls.keyFile` | string | File path to the private key in PEM format. | `""` |
| `apis.grpc.tls.certFile` | string | File path to the certificate in PEM format. | `""` |
| `apis.grpc.tls.clientCertificateRequired` | bool | If true, require client certificate. | `true` |
| `apis.grpc.authorization.ownerClaim` | string | Claim used to identify owner of the device. | `"sub"` |
| `apis.grpc.authorization.authority` | string | Authority is the address of the token-issuing authentication server. Services will use this URI to find and retrieve the public key that can be used to validate the token's signature. | `""` |
| `apis.grpc.authorization.audience` | string | Identifier of the API configured in your OAuth provider. | `""` |
| `apis.grpc.authorization.http.maxIdleConns` | int | It controls the maximum number of idle (keep-alive) connections across all hosts. Zero means no limit. | `16` |
| `apis.grpc.authorization.http.maxConnsPerHost` | int | It optionally limits the total number of connections per host, including connections in the dialing, active, and idle states. On limit violation, dials will block. Zero means no limit. | `32` |
| `apis.grpc.authorization.http.maxIdleConnsPerHost` | int | If non-zero, controls the maximum idle (keep-alive) connections to keep per-host. If zero, DefaultMaxIdleConnsPerHost is used. | `16` |
| `apis.grpc.authorization.http.idleConnTimeout` | string | The maximum amount of time an idle (keep-alive) connection will remain idle before closing itself. Zero means no limit. | `30s` |
| `apis.grpc.authorization.http.timeout` | string | A time limit for requests made by this Client. A Timeout of zero means no timeout. | `10s` |
| `apis.grpc.authorization.http.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `apis.grpc.authorization.http.tls.keyFile` | string | File path to the private key in PEM format. | `""` |
| `apis.grpc.authorization.http.tls.certFile` | string | File path to the certificate in PEM format. | `""` |
| `apis.grpc.authorization.http.tls.useSystemCAPool` | bool | If true, use the system certification pool. | `false` |

### Event Bus

plgd hub uses NATS messaging system as a event bus.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.eventBus.nats.url` | string | URL to nats messaging system. | `"nats://localhost:4222"` |
| `clients.eventBus.nats.flusherTimeout` | string | Define flush timeout for publishing message. | `"30s"` |
| `clients.eventBus.nats.jetstream`| bool | If true, events will be published to jetstream. | `false` |
| `clients.eventBus.nats.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `clients.eventBus.nats.tls.keyFile` | string | File name of private key in PEM format. | `""` |
| `clients.eventBus.nats.tls.certFile` | string | File name of certificate in PEM format. | `""` |
| `clients.eventBus.nats.tls.useSystemCAPool` | bool | If true, use the system certification pool. | `false` |

### Storage Configuration

To configure the Storage, modify the properties under `clients.storage` in your configuration file. You can choose between two databases: `MongoDB` and `CqlDB``, and customize their settings accordingly.

#### General Storage Settings

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.storage.use` | string | Database to store events. The supported values are: "mongoDB", "cqlDB". | `"mongoDB"` |

#### MongoDB Configuration

Configure MongoDB settings under `clients.storage.mongoDB`.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.storage.mongoDB.uri` | string | URI to the MongoDB database. | `"mongodb://localhost:27017"` |
| `clients.storage.mongoDB.database` | string | Name of the MongoDB database. | `"eventStore"` |
| `clients.storage.mongoDB.maxPoolSize` | int | Limits the number of connections. | `16` |
| `clients.storage.mongoDB.maxConnIdleTime` | string | Closes connection when idle time reaches the specified value. | `4m` |
| `clients.storage.mongoDB.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `clients.storage.mongoDB.tls.keyFile` | string | File path to the private key in PEM format. | `""` |
| `clients.storage.mongoDB.tls.certFile` | string | File path to the certificate in PEM format. | `""` |
| `clients.storage.mongoDB.tls.useSystemCAPool` | bool | If true, use the system certification pool. | `false` |

#### CqlDB Configuration

Configure CqlDB settings under `clients.storage.cqlDB`.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.storage.cqlDB.table` | string | Name of the CqlDB table. | `"events"` |
| `clients.storage.cqlDB.keyspace.name` | string | Name of the CqlDB keyspace. | `"plgdhub"` |
| `clients.storage.cqlDB.keyspace.create` | bool | If true, attempt to create the keyspace if it does not exist. | `true` |
| `clients.storage.cqlDB.keyspace.replication` | object | [Replication map](https://docs.datastax.com/en/cql-oss/3.3/cql/cql_reference/cqlCreateKeyspace.html) determining the data copies in a data center. | `{ "class": "SimpleStrategy", "replication_factor": 1 }` |
| `clients.storage.cqlDB.hosts` | []string | List of hosts to connect to without scheme and port. | `[]` |
| `clients.storage.cqlDB.port` | int | Port to connect to. | `9042` |
| `clients.storage.cqlDB.numConnections` | int | The number of connections to the DB. | `16` |
| `clients.storage.cqlDB.connectTimeout` | string | Time to wait until a successful connection is established. | `10s` |
| `clients.storage.cqlDB.useHostnameResolution` | bool | If true, attempt to resolve IP for FQDN and use it for connection. | `true` |
| `clients.storage.cqlDB.reconnectionPolicy.constant.interval` | string | Time to sleep between connection attempts after a failure. | `3s` |
| `clients.storage.cqlDB.reconnectionPolicy.constant.maxRetries` | int | Number of times to attempt reconnection. | `3` |
| `clients.storage.cqlDB.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `clients.storage.cqlDB.tls.keyFile` | string | File path to the private key in PEM format. | `""` |
| `clients.storage.cqlDB.tls.certFile` | string | File path to the certificate in PEM format. | `""` |
| `clients.storage.cqlDB.tls.useSystemCAPool` | bool | If true, use the system certification pool. | `false` |

{{< note >}}

Note that the string type related to time (i.e. timeout, idleConnTimeout, expirationTime) is decimal numbers, each with optional fraction and a unit suffix, such as "300ms", "1.5h" or "2h45m". Valid time units are "ns", "us", "ms", "s", "m", "h".

{{< /note >}}
