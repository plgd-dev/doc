---

title: 'MongoDB Standby Tool'
description: 'MongoDB Standby Tool overview'
date: '2024-06-26'
categories: [configuration, deployment, disaster recovery]
keywords: [configuration, mongodb, disaster recovery]
weight: 11
---

MongoDB Standby Tool is used to reconfigure some of the MongoDB replica set members to be in a hidden and secondary state. For example, you can have a replica set where members are running in multiple regions for disaster recovery purposes.

## Docker Image

```bash
docker pull ghcr.io/plgd-dev/hub/mongodb-standby-tool:latest
```

## YAML Configuration

A configuration template is available at [tools/mongodb/standby-tool/config.yaml](https://github.com/plgd-dev/hub/blob/main/tools/mongodb/standby-tool/config.yaml).

### Logging

| Property | Type | Description | Default |
|----------|------|-------------|---------|
| `log.level` | string | `Logging enabled from this level.` | `"info"` |
| `log.encoding` | string | `Logging format. Supported values are: "json", "console".` | `"json"` |
| `log.stacktrace.enabled` | bool | `Log stacktrace.` | `false` |
| `log.stacktrace.level` | string | `Stacktrace from this level.` | `"warn"` |
| `log.encoderConfig.timeEncoder` | string | `Time format for logs. Supported values are: "rfc3339nano", "rfc3339".` | `"rfc3339nano"` |

## Mode

When the tool is started, it is in standby mode by default. In this mode, all members in the replica set are available, and members in the standby list are set as hidden, without votes, priority, and with a delay. All other members are reconfigured as secondary members. You can change the mode to active by setting the `mode` property to `active`. In this mode, standby members are set as secondary members with votes, priority, and without delay, and other members are set as hidden, without votes, priority, and with a delay.

| Property | Type | Description | Default |
|----------|------|-------------|---------|
| `mode` | string | `Set the running mode of the tool. Supported modes are "standby" and "active".` | `"standby"` |

### Replica Set

| Property | Type | Description | Default |
|----------|------|-------------|---------|
| `replicaSet.forceUpdate` | bool | `Update the replica set configuration with the force flag. More info [here](https://www.mongodb.com/docs/manual/reference/method/rs.reconfig/#std-label-rs-reconfig-method-force).` | `false` |
| `replicaSet.maxReadyWaits` | int | `Set the maximum number of retries for members to be ready.` | `10` |
| `replicaSet.standby.members` | []string | `List of the MongoDB members in the replica set which are used as hidden (mode == "standby") or secondary (mode == "active") members. All other members are reconfigured to the opposite.` | `[]` |
| `replicaSet.standby.delays` | string | `Set the delay for syncing the hidden members with the secondary/primary members.` | `6m` |
| `replicaSet.secondary.priority` | int | `Used to configure the secondary members' priority.` | `10` |
| `replicaSet.secondary.votes` | int | `Set the number of votes for the secondary members.` | `1` |

### MongoDB

The tool uses the first member in the standby list (`replicaSet.standby.members`) to connect to MongoDB.

| Property | Type | Description | Default |
|----------|------|-------------|---------|
| `clients.storage.mongoDB.timeout` | string | `The maximum duration for the entire request operation.` | `"20s"` |
| `clients.storage.mongoDB.tls.enabled` | bool | `If true, use TLS for the connection.` | `false` |
| `clients.storage.mongoDB.tls.caPool` | []string | `File paths to the root certificates in PEM format. The file may contain multiple certificates.` | `[]` |
| `clients.storage.mongoDB.tls.keyFile` | string | `File path to the private key in PEM format.` | `""` |
| `clients.storage.mongoDB.tls.certFile` | string | `File path to the certificate in PEM format.` | `""` |
| `clients.storage.mongoDB.tls.useSystemCAPool` | bool | `If true, use the system certification pool.` | `false` |

{{< note >}}

Note that string types related to time (i.e. timeout, idleConnTimeout, expirationTime) are decimal numbers, each with an optional fraction and a unit suffix, such as "300ms", "1.5h", or "2h45m". Valid time units are "ns", "us", "ms", "s", "m", "h".

{{< /note >}}
