---
title: 'Resource Aggregate'
description: 'Service configuration overview'
date: '2021-10-01'
lastmod: '2021-10-01'
categories: [configuration, deployment]
keywords: [configuration]
menu:
  docs:
    parent: configuration
    weight: 40
toc: true
---

Resource Aggregate translates commands to events, stores them to the database and publishes them to the messaging system.

## Docker Image

```bash
docker pull plgd/resource-aggregate:latest
```

## YAML Configuration

A configuration template is available on [resource-aggregate/config.yaml](https://github.com/plgd-dev/hub/blob/main/resource-aggregate/config.yaml).

### Logging

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `log.debug` | bool | `Set to true if you would like to see extra information on logs.` | `false` |

### gRPC API

gRPC API of the Resource Aggregate service as defined [here](https://github.com/plgd-dev/hub/blob/main/resource-aggregate/service/service_grpc.pb.go#L20).

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `api.grpc.ownerCacheExpiration` | string | `Time limit of how long to keep subscribed to device updates after last use of the given cache item.` | `1m` |
| `api.grpc.address` | string | `Listen specification <host>:<port> for grpc client connection.` | `"0.0.0.0:9100"` |
| `api.grpc.enforcementPolicy.minTime` | string | `The minimum amount of time a client should wait before sending a keepalive ping. Otherwise the server close connection.` | `5s`|
| `api.grpc.enforcementPolicy.permitWithoutStream` | bool |  `If true, server allows keepalive pings even when there are no active streams(RPCs). Otherwise the server close connection.`  | `true` |
| `api.grpc.keepAlive.maxConnectionIdle` | string | `A duration for the amount of time after which an idle connection would be closed by sending a GoAway. 0s means infinity.` | `0s` |
| `api.grpc.keepAlive.maxConnectionAge` | string | `A duration for the maximum amount of time a connection may exist before it will be closed by sending a GoAway. 0s means infinity.` | `0s` |
| `api.grpc.keepAlive.maxConnectionAgeGrace` | string | `An additive period after MaxConnectionAge after which the connection will be forcibly closed. 0s means infinity.` | `0s` |
| `api.grpc.keepAlive.time` | string | `After a duration of this time if the server doesn't see any activity it pings the client to see if the transport is still alive.` | `2h` |
| `api.grpc.keepAlive.timeout` | string | `After having pinged for keepalive check, the client waits for a duration of Timeout and if no activity is seen even after that the connection is closed.` | `20s` |
| `api.grpc.tls.caPool` | string | `File path to the root certificate in PEM format which might contain multiple certificates in a single file.` |  `""` |
| `api.grpc.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `api.grpc.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `api.grpc.tls.clientCertificateRequired` | bool | `If true, require client certificate.` | `true` |
| `api.grpc.authorization.authority` | string | `Authority is the address of the token-issuing authentication server. Services will use this URI to find and retrieve the public key that can be used to validate the token’s signature.` | `""` |
| `api.grpc.authorization.audience` | string | `Identifier of the API configured in your OAuth provider.` | `""` |
| `api.grpc.authorization.http.maxIdleConns` | int | `It controls the maximum number of idle (keep-alive) connections across all hosts. Zero means no limit.` | `16` |
| `api.grpc.authorization.http.maxConnsPerHost` | int | `It optionally limits the total number of connections per host, including connections in the dialing, active, and idle states. On limit violation, dials will block. Zero means no limit.` | `32` |
| `api.grpc.authorization.http.maxIdleConnsPerHost` | int | `If non-zero, controls the maximum idle (keep-alive) connections to keep per-host. If zero, DefaultMaxIdleConnsPerHost is used.` | `16` |
| `api.grpc.authorization.http.idleConnTimeout` | string | `The maximum amount of time an idle (keep-alive) connection will remain idle before closing itself. Zero means no limit.` | `30s` |
| `api.grpc.authorization.http.timeout` | string | `A time limit for requests made by this Client. A Timeout of zero means no timeout.` | `10s` |
| `api.grpc.authorization.http.tls.caPool` | string | `File path to the root certificate in PEM format which might contain multiple certificates in a single file.` |  `""` |
| `api.grpc.authorization.http.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `api.grpc.authorization.http.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `api.grpc.authorization.http.tls.useSystemCAPool` | bool | `If true, use system certification pool.` | `false` |

### Event Bus

plgd hub uses NATS messaging system as a event bus.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.eventBus.nats.url` | string | `URL to nats messaging system.` | `"nats://localhost:4222"` |
| `clients.eventBus.nats.flusherTimeout` | string | `Define flush timeout for publishing message.` | `"30s"` |
| `clients.eventBus.nats.jetstream`| bool | `If true, events will be published to jetstream.` | `false` |
| `clients.eventBus.nats.tls.caPool` | string | `root certificate the root certificate in PEM format.` |  `""` |
| `clients.eventBus.nats.tls.keyFile` | string | `File name of private key in PEM format.` | `""` |
| `clients.eventBus.nats.tls.certFile` | string | `File name of certificate in PEM format.` | `""` |
| `clients.eventBus.nats.tls.useSystemCAPool` | bool | `If true, use system certification pool.` | `false` |

### Event Store

plgd hub uses MongoDB database as a event store.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.eventStore.defaultCommandTimeToLive` | string | `Replaces time to live in CreateResource, RetrieveResource, UpdateResource, DeleteResource and UpdateDeviceMetadata commands when it is zero value. 0s - means forever.` | `"0s"` |
| `clients.eventStore.snapshotThreshold` | int | `Tries to create the snapshot event after n events.` | `16` |
| `clients.eventStore.occMaxRetry` | int | `Limits number of try to store event.` | `8` |
| `clients.eventStore.mongoDB.uri` | string | `URI to mongo database.` | `"mongodb://localhost:27017"` |
| `clients.eventStore.mongoDB.database` | string | `Name of database` | `"eventStore"` |
| `clients.eventStore.mongoDB.maxPoolSize` | int | `Limits number of connections.` | `16` |
| `clients.eventStore.mongoDB.maxConnIdleTime` | string | `Close connection when idle time reach the value.` | `4m` |
| `clients.eventStore.mongoDB.tls.caPool` | string | `File path to the root certificate in PEM format which might contain multiple certificates in a single file.` |  `""` |
| `clients.eventStore.mongoDB.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `clients.eventStore.mongoDB.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `clients.eventStore.mongoDB.tls.useSystemCAPool` | bool | `If true, use system certification pool.` | `false` |

### Identity Store Client

Client configurations to internally connect to Identity Store service.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.identityStore.grpc.address` | string | `Identity Store service address.` | `"127.0.0.1:9100"` |
| `clients.identityStore.grpc.tls.caPool` | string | `File path to the root certificate in PEM format which might contain multiple certificates in a single file.` |  `""` |
| `clients.identityStore.grpc.tls.keyFile` | string | `File path to private key in PEM format.` | `""` |
| `clients.identityStore.grpc.tls.certFile` | string | `File path to certificate in PEM format.` | `""` |
| `clients.identityStore.grpc.tls.useSystemCAPool` | bool | `If true, use system certification pool.` | `false` |
| `clients.identityStore.grpc.keepAlive.time` | string | `After a duration of this time if the client doesn't see any activity it pings the server to see if the transport is still alive.` | `10s` |
| `clients.identityStore.grpc.keepAlive.timeout` | string | `After having pinged for keepalive check, the client waits for a duration of Timeout and if no activity is seen even after that the connection is closed.` | `20s` |
| `clients.identityStore.grpc.keepAlive.permitWithoutStream` | bool | `If true, client sends keepalive pings even with no active RPCs. If false, when there are no active RPCs, Time and Timeout will be ignored and no keepalive pings will be sent.` | `false` |

> Note that the string type related to time (i.e. time, timeout) is decimal numbers, each with optional fraction and a unit suffix, such as "300ms", "1.5h" or "2h45m". Valid time units are "ns", "us", "ms", "s", "m", "h".
