---
title: 'Cloud2Cloud Connector'
description: 'Service configuration overview'
date: '2021-10-01'
categories: [configuration, deployment]
keywords: [configuration]
weight: 9
---

C2C Connector connects to other hub implementing the [Open Connectivity Foundation's Cloud API for Cloud Services](https://openconnectivity.org/developer/specifications/) and mirror devices from this linked instance to yours.

## Docker Image

```bash
docker pull ghcr.io/plgd-dev/hub/cloud2cloud-connector:latest
```

## YAML Configuration

A configuration template is available in [cloud2cloud-connector/config.yaml](https://github.com/plgd-dev/hub/blob/main/cloud2cloud-connector/config.yaml)

### Logging

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `log.level` | string | Logging enabled from level. | `"info"` |
| `log.encoding` | string | Logging format. The supported values are: "json", "console" | `"json"` |
| `log.stacktrace.enabled` | bool | Log stacktrace. | `false` |
| `log.stacktrace.level` | string | Stacktrace from level. | `"warn"` |
| `log.encoderConfig.timeEncoder` | string | Time format for logs. The supported values are: "rfc3339nano", "rfc3339". | `"rfc3339nano"` |

### HTTP API

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
| `apis.http.eventsURL` | string | External url where the events from another hub will be send. | `""` |
| `apis.http.pullDevices.disabled` | bool | Disable get devices via pull for all clouds. | `true` |
| `apis.http.pullDevices.interval` | string | "Time interval between pulls. | `"5s"` |
| `apis.http.clientID` | string | Client ID to exchange an authorization code for an access token. | `""` |
| `apis.http.clientSecretFile` | string | File path to client secret required to exchange an authorization code for an access token. | `""` |
| `apis.http.scopes` | string array | List of required scopes. | `""` |
| `apis.http.authorization.authority` | string | Authority is the address of the token-issuing authentication server. Services will use this URI to find and retrieve the public key that can be used to validate the token's signature. | `""` |
| `apis.http.authorization.audience` | string | Identifier of the API configured in your OAuth provider. | `""` |
| `apis.http.authorization.ownerClaim` | string | | Claim used to identify owner of the device. | `"sub"` |
| `apis.http.authorization.redirectURL` | string | External redirect url to acquire authorization code. | `""` |
| `apis.http.authorization.http.maxIdleConns` | int | It controls the maximum number of idle (keep-alive) connections across all hosts. Zero means no limit. | `16` |
| `apis.http.authorization.http.maxConnsPerHost` | int | It optionally limits the total number of connections per host, including connections in the dialing, active, and idle states. On limit violation, dials will block. Zero means no limit. | `32` |
| `apis.http.authorization.http.maxIdleConnsPerHost` | int | If non-zero, controls the maximum idle (keep-alive) connections to keep per-host. If zero, DefaultMaxIdleConnsPerHost is used. | `16` |
| `apis.http.authorization.http.idleConnTimeout` | string | The maximum amount of time an idle (keep-alive) connection will remain idle before closing itself. Zero means no limit. | `30s` |
| `apis.http.authorization.http.timeout` | string | A time limit for requests made by this Client. A Timeout of zero means no timeout. | `10s` |
| `apis.http.authorization.http.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `apis.http.authorization.http.tls.keyFile` | string | File path to the private key in PEM format. | `""` |
| `apis.http.authorization.http.tls.certFile` | string | File path to the certificate in PEM format. | `""` |
| `apis.http.authorization.http.tls.useSystemCAPool` | bool | If true, use the system certification pool. | `false` |

{{< tip >}}

You might have one client, but multiple APIs in the OAuth system. What you want to prevent is to be able to contact all the APIs of your system with one token. This audience allows you to request the token for a specific API. If you configure it to myplgdc2c.api in the Auth0, you have to set it here if you want to also validate it.

{{< /tip >}}

### Identity Store Client

Client configurations to internally connect to Identity Store service.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.identityStore.grpc.address` | string | Identity Store service address. | `"127.0.0.1:9100"` |
| `clients.identityStore.grpc.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `clients.identityStore.grpc.tls.keyFile` | string | File path to the private key in PEM format. | `""` |
| `clients.identityStore.grpc.tls.certFile` | string | File path to the certificate in PEM format. | `""` |
| `clients.identityStore.grpc.tls.useSystemCAPool` | bool | If true, use the system certification pool. | `false` |
| `clients.identityStore.grpc.keepAlive.time` | string | After a duration of this time if the client doesn't see any activity it pings the server to see if the transport is still alive. | `10s` |
| `clients.identityStore.grpc.keepAlive.timeout` | string | After having pinged for keepalive check, the client waits for a duration of Timeout and if no activity is seen even after that the connection is closed. | `20s` |
| `clients.identityStore.grpc.keepAlive.permitWithoutStream` | bool | If true, client sends keepalive pings even with no active RPCs. If false, when there are no active RPCs, Time and Timeout will be ignored and no keepalive pings will be sent. | `false` |

### Event Bus

plgd hub uses NATS messaging system as an event bus.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.eventBus.nats.url` | string | URL to nats messaging system. | `"nats://localhost:4222"` |
| `clients.eventBus.nats.pendingLimits.msgLimit` | int | Limit number of messages in queue. -1 means unlimited | `524288` |
| `clients.eventBus.nats.pendingLimits.bytesLimit` | int | Limit buffer size of queue. -1 means unlimited | `67108864` |
| `clients.eventBus.nats.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `clients.eventBus.nats.tls.keyFile` | string | File name of private key in PEM format. | `""` |
| `clients.eventBus.nats.tls.certFile` | string | File name of certificate in PEM format. | `""` |
| `clients.eventBus.nats.tls.useSystemCAPool` | bool | If true, use the system certification pool. | `false` |

### GRPC Gateway Client

Client configurations to internally connect to GRPC Gateway service.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.grpcGateway.grpc.address` | string | GRPC Gateway service address. | `"127.0.0.1:9100"` |
| `clients.grpcGateway.grpc.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `clients.grpcGateway.grpc.tls.keyFile` | string | File path to the private key in PEM format. | `""` |
| `clients.grpcGateway.grpc.tls.certFile` | string | File path to the certificate in PEM format. | `""` |
| `clients.grpcGateway.grpc.tls.useSystemCAPool` | bool | If true, use the system certification pool. | `false` |
| `clients.grpcGateway.grpc.keepAlive.time` | string | After a duration of this time if the client doesn't see any activity it pings the server to see if the transport is still alive. | `10s` |
| `clients.grpcGateway.grpc.keepAlive.timeout` | string | After having pinged for keepalive check, the client waits for a duration of Timeout and if no activity is seen even after that the connection is closed. | `20s` |
| `clients.grpcGateway.grpc.keepAlive.permitWithoutStream` | bool | If true, client sends keepalive pings even with no active RPCs. If false, when there are no active RPCs, Time and Timeout will be ignored and no keepalive pings will be sent. | `false` |

### Resource Aggregate Client

Client configurations to internally connect to the Resource Aggregate service.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.resourceAggregate.grpc.address` | string | Resource aggregate service address. | `"127.0.0.1:9100"` |
| `clients.resourceAggregate.grpc.keepAlive.time` | string | After a duration of this time if the client doesn't see any activity it pings the server to see if the transport is still alive. | `10s` |
| `clients.resourceAggregate.grpc.keepAlive.timeout` | string | After having pinged for keepalive check, the client waits for a duration of Timeout and if no activity is seen even after that the connection is closed. | `20s` |
| `clients.resourceAggregate.grpc.keepAlive.permitWithoutStream` | bool | If true, client sends keepalive pings even with no active RPCs. If false, when there are no active RPCs, Time and Timeout will be ignored and no keepalive pings will be sent. | `false` |
| `clients.resourceAggregate.grpc.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `clients.resourceAggregate.grpc.tls.keyFile` | string | File path to the private key in PEM format. | `""` |
| `clients.resourceAggregate.grpc.tls.certFile` | string | File path to the certificate in PEM format. | `""` |
| `clients.resourceAggregate.grpc.tls.useSystemCAPool` | bool | If true, use the system certification pool. | `false` |

### Storage

plgd hub uses MongoDB database as the owner's device store.

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.storage.mongoDB.uri` | string | URI to mongo database. | `"mongodb://localhost:27017"` |
| `clients.storage.mongoDB.database` | string | Name of database. | `"cloud2cloudConnector"` |
| `clients.storage.mongoDB.maxPoolSize` | int | Limits number of connections. | `16` |
| `clients.storage.mongoDB.maxConnIdleTime` | string | Close connection when idle time reach the value. | `4m` |
| `clients.storage.mongoDB.tls.caPool` | []string | File paths to root certificates in PEM format. | `[]` |
| `clients.storage.mongoDB.tls.keyFile` | string | File path to the private key in PEM format. | `""` |
| `clients.storage.mongoDB.tls.certFile` | string | File path to the certificate in PEM format. | `""` |
| `clients.storage.mongoDB.tls.useSystemCAPool` | bool | If true, use the system certification pool. | `false` |

### Subscription

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `clients.subscription.http.reconnectInterval` | string | The interval after which to try to reconnect to grpc-gateway client when connection was closed. | `"10s"` |
| `clients.subscription.http.resubscribeInterval` | string | The interval after which to try to resubscribe to grpc-gateway client when subscription does not exist. | `"10s"` |

### Task Queue

| Property | Type | Description | Default |
| ---------- | -------- | -------------- | ------- |
| `taskProcessor.cacheSize` | int | Size of processor task queue. | `2048` |
| `taskProcessor.timeout` | string | Timeout for one running task. | `"5s"` |
| `taskProcessor.maxParallel` | int | Maximal number of running tasks at the same time. | `128` |
| `taskProcessor.delay` | string | Task delay before start. | `0s` |

## API

Follow [OCF Cloud API For Cloud Services Specification](https://openconnectivity.org/specs/OCF_Cloud_API_For_Cloud_Services_Specification_v2.2.0.pdf)

### Commands

- maintenance of linked clouds
- maintenance of linked accounts
- [swagger](https://petstore.swagger.io/?url=https://raw.githubusercontent.com/plgd-dev/hub/main/cloud2cloud-connector/swagger.yaml)
