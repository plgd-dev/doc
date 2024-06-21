---
title: 'Disaster Recovery via Replica Set'
description: 'How to perform disaster recovery with a Replica Set?'
date: '2024-06-17'
categories: [architecture, d2c, provisioning, disaster-recovery]
keywords: [architecture, d2c, provisioning, disaster-recovery]
weight: 15
---

The plgd-hub Helm charts support disaster recovery via a MongoDB replica set because the source of truth is stored in the MongoDB database. It is required that devices have configured **device provisioning endpoints** for both clusters' device provisioning services. In this tutorial, we have two MicroK8s clusters: primary and standby. Each of them uses three root CA certificates:

- `external CA certificate pair`: Used for public APIs (CoAP, HTTPS, gRPC) and is the same for both clusters.
- `internal CA certificate pair`: Used for plgd services to communicate with each other, MongoDB, and NATs. Each cluster has its own internal CA certificate.
- `storage CA certificate pair`: Used for MongoDB. Each cluster has its own storage CA certificate.

We also use an `authorization CA certificate` to communicate with the OAuth2 authorization server. In this tutorial, `mock-oauth-server` and its certificate are signed by the `external CA certificate pair`. Thus, we have only one `authorization CA certificate` for both clusters, which is the `external CA certificate`.

The goal is to ensure that only MongoDBs from the primary and standby clusters can communicate with each other, while plgd services can only connect to the MongoDB in their respective clusters. All APIs will be available on the root domain `primary.plgd.cloud` for the primary cluster and `standby.plgd.cloud` for the standby cluster. Additionally, MongoDB members are exposed via the LoadBalancer service type, and each member needs its own DNS name.

| For the primary cluster we have | For the standby cluster we have |
| --- | --- |
| `primary.plgd.cloud` | `standby.plgd.cloud` |
| `mongodb-0.primary.plgd.cloud` | `mongodb-0.standby.plgd.cloud` |
| `mongodb-1.primary.plgd.cloud` | `mongodb-1.standby.plgd.cloud` |
| `mongodb-2.primary.plgd.cloud` | `mongodb-2.standby.plgd.cloud` |
| `mongodb.primary.plgd.cloud` | `----` |

The `mongodb.primary.plgd.cloud` is used for external access to the MongoDB replica set for the standby cluster. This DNS record is an alias for all members of the primary cluster.

This DNS needs to be resolved to the external IP address of the LoadBalancer. The external IP address of the LoadBalancer is used to connect to the MongoDB replica set from the other cluster. For clouds, you can use the [external-dns](https://github.com/kubernetes-sigs/external-dns/) tool to create DNS records in AWS Route53 / Google Cloud DNS / Azure DNS.
In this tutorial, we show how to get the IPs of MongoDB services, and we will set them manually in /etc/hosts, then restart the dnsmasq daemon to load these changes on the computer with IP 192.168.1.1.

{{< warning >}}
It is also recommended to set up a firewall between clusters with source IP address filtering to mitigate DDOS attacks on MongoDB. The default port for MongoDB is 27017. Alternatively, use a VPN to interconnect clusters.
{{< /warning >}}

## Installation

### MicroK8s Prerequisites

The following addons are expected to be enabled on both clusters, with **Kubernetes v1.24+** installed.

```yaml
addons:
  enabled:
    cert-manager         # (core) Cloud-native certificate management
    dns                  # (core) CoreDNS
    ha-cluster           # (core) Configure high availability on the current node
    helm                 # (core) Helm - the package manager for Kubernetes
    helm3                # (core) Helm 3 - the package manager for Kubernetes
    hostpath-storage     # (core) Storage class; allocates storage from host directory
    ingress              # (core) Ingress controller for external access
    metallb              # (core) Loadbalancer for your Kubernetes cluster
```

The [dns](https://microk8s.io/docs/addon-dns) addon is configured to use a DNS server that hosts all records for `primary.plgd.cloud` and `standby.plgd.cloud` domains. To configure DNS in MicroK8s, you can use the following command:

```bash
microk8s disable dns
microk8s enable dns:192.168.1.1
```

For [metallb](https://microk8s.io/docs/addon-metallb), we need to set up the IP address pool for the LoadBalancer service type. The IP address pool needs to be accessible from the network where the MicroK8s is running. It is important that the IP address is not used by any other device in the network and that the DHCP server is not assigning this IP address to any device.

Example for the primary cluster:

```bash
microk8s disable metallb
microk8s enable metallb:192.168.1.200-192.168.1.219
```

Example for the standby cluster:

```bash
microk8s disable metallb
microk8s enable metallb:192.168.1.220-192.168.1.239
```

### Creating Certificates

To create certificates, you can use the cert-tool Docker image to generate root CA certificates for the services.

1. Create the external CA certificate pair (same for both clusters):

   ```bash
   mkdir -p .tmp/certs/external
   docker run \
        --rm -v $(pwd)/.tmp/certs/external:/certs \
        --user $(id -u):$(id -g) \
        ghcr.io/plgd-dev/hub/cert-tool:vnext \
        --cmd.generateRootCA --outCert=/certs/tls.crt --outKey=/certs/tls.key \
        --cert.subject.cn=external.root.ca --cert.validFor=876000h
   ```

2. Create the internal CA certificate pair for the primary cluster:

   ```bash
   mkdir -p .tmp/primary/certs/internal
   docker run \
        --rm -v $(pwd)/.tmp/primary/certs/internal:/certs \
        --user $(id -u):$(id -g) \
        ghcr.io/plgd-dev/hub/cert-tool:vnext \
        --cmd.generateRootCA --outCert=/certs/tls.crt --outKey=/certs/tls.key \
        --cert.subject.cn=primary.internal.root.ca --cert.validFor=876000h
   ```

3. Create the storage CA certificate pair for the primary cluster:

   ```bash
   mkdir -p .tmp/primary/certs/storage
   docker run \
        --rm -v $(pwd)/.tmp/primary/certs/storage:/certs \
        --user $(id -u):$(id -g) \
        ghcr.io/plgd-dev/hub/cert-tool:vnext \
        --cmd.generateRootCA --outCert=/certs/tls.crt --outKey=/certs/tls.key \
        --cert.subject.cn=primary.storage.root.ca --cert.validFor=876000h
   ```

4. Create the internal CA certificate pair for the standby cluster:

   ```bash
   mkdir -p .tmp/standby/certs/internal
   docker run \
        --rm -v $(pwd)/.tmp/standby/certs/internal:/certs \
        --user $(id -u):$(id -g) \
        ghcr.io/plgd-dev/hub/cert-tool:vnext \
        --cmd.generateRootCA --outCert=/certs/tls.crt --outKey=/certs/tls.key \
        --cert.subject.cn=standby.internal.root.ca --cert.validFor=876000h
   ```

5. Create the storage CA certificate pair for the standby cluster:

   ```bash
   mkdir -p .tmp/standby/certs/storage
   docker run \
        --rm -v $(pwd)/.tmp/standby/certs/storage:/certs \
        --user $(id -u):$(id -g) \
        ghcr.io/plgd-dev/hub/cert-tool:vnext \
        --cmd.generateRootCA --outCert=/certs/tls.crt --outKey=/certs/tls.key \
        --cert.subject.cn=standby.storage.root.ca --cert.validFor=876000h
   ```

### Preparing Device Provisioning Service dependencies

The Device Provisioning Service (DPS) requires a certificate for the manufacturer. The certificate is used to authenticate the manufacturer when enrolling devices which need to stored in the file `.tmp/certs/manufacturer/tls.crt`.

To download proprietary device provisioning service docker image, you need to have a token for the GitHub Container Registry. The token need to stored in the file `.tmp/tokens/plgd-docker-auth-token.txt`.

### Setting up cert-manager on the Primary Cluster

Ensure that you have cert-manager installed.

1. Create an external TLS secret for issuing certificates:

   ```bash
   kubectl -n cert-manager create secret tls external-plgd-ca-tls \
        --cert=.tmp/certs/external/tls.crt \
        --key=.tmp/certs/external/tls.key
   ```

2. Create a ClusterIssuer that points to `external-plgd-ca-tls`:

   ```bash
   cat <<EOF | kubectl apply -f -
   apiVersion: cert-manager.io/v1
   kind: ClusterIssuer
   metadata:
     name: external-plgd-ca-issuer
   spec:
     ca:
       secretName: external-plgd-ca-tls
   EOF
   ```

3. Create an internal TLS secret for issuing certificates:

   ```bash
   kubectl -n cert-manager create secret tls internal-plgd-ca-tls \
        --cert=.tmp/primary/certs/internal/tls.crt \
        --key=.tmp/primary/certs/internal/tls.key
   ```

4. Create a ClusterIssuer that points to `internal-plgd-ca-tls`:

   ```bash
   cat <<EOF | kubectl apply -f -
   apiVersion: cert-manager.io/v1
   kind: ClusterIssuer
   metadata:
     name: internal-plgd-ca-issuer
   spec:
     ca:
       secretName: internal-plgd-ca-tls
   EOF
   ```

5. Create a storage TLS secret for issuing certificates:

   ```bash
   kubectl -n cert-manager create secret tls storage-plgd-ca-tls \
        --cert=.tmp/primary/certs/storage/tls.crt \
        --key=.tmp/primary/certs/storage/tls.key
   ```

6. Create a ClusterIssuer that points to `storage-plgd-ca-tls`:

   ```bash
   cat <<EOF | kubectl apply -f -
   apiVersion: cert-manager.io/v1
   kind: ClusterIssuer
   metadata:
     name: storage-plgd-ca-issuer
   spec:
     ca:
       secretName: storage-plgd-ca-tls
   EOF
   ```

### Setting up cert-manager on the Standby Cluster

Ensure that you have cert-manager installed on the standby cluster as well.

1. Create an external TLS secret for issuing certificates:

   ```bash
   kubectl -n cert-manager create secret tls external-plgd-ca-tls \
        --cert=.tmp/certs/external/tls.crt \
        --key=.tmp/certs/external/tls.key
   ```

2. Create a ClusterIssuer that points to `external-plgd-ca-tls`:

   ```bash
   cat <<EOF | kubectl apply -f -
   apiVersion: cert-manager.io/v1
   kind: ClusterIssuer
   metadata:
     name: external-plgd-ca-issuer
   spec:
     ca:
       secretName: external-plgd-ca-tls
   EOF
   ```

3. Create an internal TLS secret for issuing certificates:

   ```bash
   kubectl -n cert-manager create secret tls internal-plgd-ca-tls \
        --cert=.tmp/standby/certs/internal/tls.crt \
        --key=.tmp/standby/certs/internal/tls.key
   ```

4. Create a ClusterIssuer that points to `internal-plgd-ca-tls`:

   ```bash
   cat <<EOF | kubectl apply -f -
   apiVersion: cert-manager.io/v1
   kind: ClusterIssuer
   metadata:
     name: internal-plgd-ca-issuer
   spec:
     ca:
       secretName: internal-plgd-ca-tls
   EOF
   ```

5. Create a storage TLS secret for issuing certificates:

   ```bash
   kubectl -n cert-manager create secret tls storage-plgd-ca-tls \
        --cert=.tmp/standby/certs/storage/tls.crt \
        --key=.tmp/standby/certs/storage/tls.key
   ```

6. Create a ClusterIssuer that points to `storage-plgd-ca-tls`:

   ```bash
   cat <<EOF | kubectl apply -f -
   apiVersion: cert-manager.io/v1
   kind: ClusterIssuer
   metadata:
     name: storage-plgd-ca-issuer
   spec:
     ca:
       secretName: storage-plgd-ca-tls
   EOF
   ```

### Deploy plgd on Primary Cluster

The primary cluster will deploy the Hub with all APIs exposed on the `primary.plgd.cloud` domain. The CoAP gateway listens on NodePort `15684`, and the device provisioning service listens on NodePort `5684`. The MongoDB replica set is exposed via a LoadBalancer service type, requiring a client certificate (mTLS) to connect to MongoDB.

To deploy the plgd-hub and plgd-dps Helm charts on the primary cluster, use the following Helm command:

```bash
# Set variables
DOMAIN="primary.plgd.cloud"
HUB_ID="d03a1bb4-0a77-428c-b78c-1c46efe6a38e"
OWNER_CLAIM="https://plgd.dev/owner"
STANDBY=false

# Read certificate files
AUTHORIZATION_CA_IN_PEM=.tmp/certs/external/tls.crt
INTERNAL_CA_IN_PEM=.tmp/primary/certs/internal/tls.crt
EXTERNAL_CA_IN_PEM=.tmp/certs/external/tls.crt
STORAGE_PRIMARY_CA_IN_PEM=.tmp/primary/certs/storage/tls.crt
STORAGE_STANDBY_CA_IN_PEM=.tmp/standby/certs/storage/tls.crt
MANUFACTURER_CERTIFICATE_CA=.tmp/certs/manufacturer/tls.crt
DOCKER_AUTH_TOKEN=.tmp/tokens/plgd-docker-auth-token.txt

# Create values.yaml file
cat <<EOF > values.yaml
global:
  domain: "$DOMAIN"
  hubId: "$HUB_ID"
  ownerClaim: "$OWNER_CLAIM"
  standby: $STANDBY
  extraCAPool:
    authorization: |
$(sed 's/^/      /' $AUTHORIZATION_CA_IN_PEM)
    internal: |
$(sed 's/^/      /' $INTERNAL_CA_IN_PEM)
$(sed 's/^/      /' $STORAGE_PRIMARY_CA_IN_PEM)
$(sed 's/^/      /' $EXTERNAL_CA_IN_PEM)
    storage: |
$(sed 's/^/      /' $STORAGE_PRIMARY_CA_IN_PEM)
$(sed 's/^/      /' $STORAGE_STANDBY_CA_IN_PEM)
$(sed 's/^/      /' $INTERNAL_CA_IN_PEM)
mockoauthserver:
  enabled: true
  oauth:
    - name: "plgd.dps"
      clientID: "test"
      clientSecret: "test"
      grantType: "clientCredentials"
      redirectURL: "https://$DOMAIN/things"
      scopes: ['openid']
    - name: "plgd.web"
      clientID: "test"
      clientSecret: "test"
      redirectURL: "https://$DOMAIN/things"
      scopes: ['openid']
      useInUi: true
mongodb:
  tls:
    extraDnsNames:
      - "mongodb.$DOMAIN"
  externalAccess:
    enabled: true
    service:
      type: LoadBalancer
      publicNames:
        - "mongodb-0.$DOMAIN"
        - "mongodb-1.$DOMAIN"
        - "mongodb-2.$DOMAIN"
      annotationsList:
        - external-dns.alpha.kubernetes.io/hostname: "mongodb-0.$DOMAIN"
        - external-dns.alpha.kubernetes.io/hostname: "mongodb-1.$DOMAIN"
        - external-dns.alpha.kubernetes.io/hostname: "mongodb-2.$DOMAIN"
certmanager:
  storage:
    issuer:
      kind: ClusterIssuer
      name: storage-plgd-ca-issuer
  internal:
    issuer:
      kind: ClusterIssuer
      name: internal-plgd-ca-issuer
  default:
    ca:
      issuerRef:
        kind: ClusterIssuer
        name: external-plgd-ca-issuer
httpgateway:
  apiDomain: "$DOMAIN"
grpcgateway:
  domain: "$DOMAIN"
certificateauthority:
  domain: "$DOMAIN"
coapgateway:
  service:
    type: NodePort
    nodePort: 15684
resourcedirectory:
  publicConfiguration:
    coapGateway: "coaps+tcp://$DOMAIN:15684"
deviceProvisioningService:
  apiDomain: "$DOMAIN"
  service:
    type: NodePort
  image:
    dockerConfigSecret: |
      {
        "auths": {
          "ghcr.io": {
              "auth": "$(cat $DOCKER_AUTH_TOKEN)"
          }
        }
      }
  enrollmentGroups:
    - id: "5db6ccde-05e1-480b-a522-c1591ad7dfd2"
      owner: "1"
      attestationMechanism:
        x509:
          certificateChain: |-
$(sed 's/^/            /' $MANUFACTURER_CERTIFICATE_CA)
      hub:
        coapGateway: "$DOMAIN:15684"
        certificateAuthority:
          grpc:
            address: "$DOMAIN:443"
        authorization:
          provider:
            name: "plgd.dps"
            clientId: "test"
            clientSecret: "test"
            audience: "https://$DOMAIN"
EOF
helm upgrade -i -n plgd --create-namespace -f values.yaml hub plgd/plgd-hub
helm upgrade -i -n plgd --create-namespace -f values.yaml dps plgd/plgd-dps
```

Now we need to get the IP addresses of the MongoDB members and set them to the DNS. The external IP address of the LoadBalancer is used to connect to the MongoDB replica set from the other cluster.

```bash
kubectl -n plgd get services | grep mongodb | grep LoadBalancer | awk '{print $1 ":" $4}'
mongodb-0-external:192.168.1.202
mongodb-1-external:192.168.1.200
mongodb-2-external:192.168.1.201
```

Next, we need to set the DNS records for the primary cluster to the DNS server running on `192.168.1.1`. The `mongodb.primary.plgd.cloud` is an alias to all members of the primary cluster.

```bash
echo "
192.168.1.202 mongodb-0.primary.plgd.cloud mongodb.primary.plgd.cloud
192.168.1.200 mongodb-1.primary.plgd.cloud mongodb.primary.plgd.cloud
192.168.1.201 mongodb-2.primary.plgd.cloud mongodb.primary.plgd.cloud
" | sudo tee -a /etc/hosts
sudo systemctl restart dnsmasq
```

After some time for the pods to start, you can access the Hub at `https://primary.plgd.cloud`.

### Deploy plgd on Standby Cluster

Deploying plgd to the standby cluster is similar to deploying it to the primary cluster. The differences are that the domain is `standby.plgd.cloud`, different internal and storage certificates are used, the standby flag is set to `true`, NATs is disabled and MongoDB is configured to use the master DB at `mongodb.primary.plgd.cloud`, and the `mongodb-standby-tool` job is enabled to configure the MongoDB replica set.

```bash
# Set variables
DOMAIN="standby.plgd.cloud"
PRIMARY_MONGO_DB="mongodb.primary.plgd.cloud"
HUB_ID="d03a1bb4-0a77-428c-b78c-1c46efe6a38e"
OWNER_CLAIM="https://plgd.dev/owner"
STANDBY=true

# Read certificate files
AUTHORIZATION_CA_IN_PEM=.tmp/certs/external/tls.crt
INTERNAL_CA_IN_PEM=.tmp/primary/certs/internal/tls.crt
EXTERNAL_CA_IN_PEM=.tmp/certs/external/tls.crt
STORAGE_PRIMARY_CA_IN_PEM=.tmp/primary/certs/storage/tls.crt
STORAGE_STANDBY_CA_IN_PEM=.tmp/standby/certs/storage/tls.crt
MANUFACTURER_CERTIFICATE_CA=.tmp/certs/manufacturer/tls.crt
DOCKER_AUTH_TOKEN=.tmp/tokens/plgd-docker-auth-token.txt

# Create values.yaml file
cat <<EOF > values.yaml
global:
  domain: "$DOMAIN"
  hubId: "$HUB_ID"
  ownerClaim: "$OWNER_CLAIM"
  standby: $STANDBY
  extraCAPool:
    authorization: |
$(sed 's/^/      /' $AUTHORIZATION_CA_IN_PEM)
    internal: |
$(sed 's/^/      /' $INTERNAL_CA_IN_PEM)
$(sed 's/^/      /' $STORAGE_STANDBY_CA_IN_PEM)
$(sed 's/^/      /' $EXTERNAL_CA_IN_PEM)
    storage: |
$(sed 's/^/      /' $STORAGE_PRIMARY_CA_IN_PEM)
$(sed 's/^/      /' $STORAGE_STANDBY_CA_IN_PEM)
$(sed 's/^/      /' $INTERNAL_CA_IN_PEM)
mockoauthserver:
  enabled: true
  oauth:
    - name: "plgd.dps"
      clientID: "test"
      clientSecret: "test"
      grantType: "clientCredentials"
      redirectURL: "https://$DOMAIN/things"
      scopes: ['openid']
    - name: "plgd.web"
      clientID: "test"
      clientSecret: "test"
      redirectURL: "https://$DOMAIN/things"
      scopes: ['openid']
      useInUi: true
mongodb:
  standbyTool:
    enabled: true
    replicaSet:
      standby:
        members:
        - "mongodb-0.$DOMAIN:27017"
        - "mongodb-1.$DOMAIN:27017"
        - "mongodb-2.$DOMAIN:27017"
  externalAccess:
    enabled: true
    externalMaster:
      enabled: true
      host: "$PRIMARY_MONGO_DB"
    service:
      type: LoadBalancer
      publicNames:
        - "mongodb-0.$DOMAIN"
        - "mongodb-1.$DOMAIN"
        - "mongodb-2.$DOMAIN"
      annotationsList:
        - external-dns.alpha.kubernetes.io/hostname: "mongodb-0.$DOMAIN"
        - external-dns.alpha.kubernetes.io/hostname: "mongodb-1.$DOMAIN"
        - external-dns.alpha.kubernetes.io/hostname: "mongodb-2.$DOMAIN"
nats:
  enabled: false
certmanager:
  storage:
    issuer:
      kind: ClusterIssuer
      name: storage-plgd-ca-issuer
  internal:
    issuer:
      kind: ClusterIssuer
      name: internal-plgd-ca-issuer
  default:
    ca:
      issuerRef:
        kind: ClusterIssuer
        name: external-plgd-ca-issuer
httpgateway:
  apiDomain: "$DOMAIN"
grpcgateway:
  domain: "$DOMAIN"
certificateauthority:
  domain: "$DOMAIN"
coapgateway:
  service:
    type: NodePort
    nodePort: 15684
resourcedirectory:
  publicConfiguration:
    coapGateway: "coaps+tcp://$DOMAIN:15684"
deviceProvisioningService:
  apiDomain: "$DOMAIN"
  service:
    type: NodePort
  image:
    dockerConfigSecret: |
      {
        "auths": {
          "ghcr.io": {
              "auth": "$(cat $DOCKER_AUTH_TOKEN)"
          }
        }
      }
  enrollmentGroups:
    - id: "5db6ccde-05e1-480b-a522-c1591ad7dfd2"
      owner: "1"
      attestationMechanism:
        x509:
          certificateChain: |-
$(sed 's/^/            /' $MANUFACTURER_CERTIFICATE_CA)
      hub:
        coapGateway: "$DOMAIN:15684"
        certificateAuthority:
          grpc:
            address: "$DOMAIN:443"
        authorization:
          provider:
            name: "plgd.dps"
            clientId: "test"
            clientSecret: "test"
            audience: "https://$DOMAIN"
EOF
helm upgrade -i -n plgd --create-namespace -f values.yaml hub plgd/plgd-hub
helm upgrade -i -n plgd --create-namespace -f values.yaml dps plgd/plgd-dps
```

Next, we need to get the IP addresses of the MongoDB members and set them to the DNS server running on `192.168.1.1`, similar to the primary cluster.

```bash
kubectl -n plgd get services | grep mongodb | grep LoadBalancer | awk '{print $1 ":" $4}'
echo "
192.168.1.222 mongodb-0.standby.plgd.cloud
192.168.1.220 mongodb-1.standby.plgd.cloud
192.168.1.221 mongodb-2.standby.plgd.cloud
" | sudo tee -a /etc/hosts
sudo systemctl restart dnsmasq
```

{{< note >}}
It is important that the `global.standby` flag is set to `true`, which means that plgd pods are not running on the standby cluster.
{{< /note >}}

Once the MongoDB pods are running, we need to run the `mongodb-standby-tool` job to configure the MongoDB replica set. This configuration demotes the secondary members to hidden members.

```bash
kubectl -n plgd patch job/$(kubectl -n plgd get jobs | grep mongodb-standby-tool | awk '{print $1}') --type=strategic --patch '{"spec":{"suspend":false}}'
```

Now the job will create the pod and configure the MongoDB replica set.

## Disaster Recovery

{{< note >}}
These steps could be used in case of planned maintenance.
{{< /note >}}

### How to Switch to the Standby Cluster

When the primary cluster is down, you need to switch to the standby cluster.

#### Promote the Standby Cluster

First, promote the hidden members to secondary members. To do this, upgrade the Helm chart with the `mongodb.standbyTool.mode` set to `active`. The active mode reconfigures the MongoDB replica set, promoting hidden members to secondary members and demoting the previous members to hidden. To do that we need to delete the `mongodb-standby-tool` job and upgrade the Helm chart which will create a new job.

```bash
kubectl -n plgd delete job/$(kubectl -n plgd get jobs | grep mongodb-standby-tool | awk '{print $1}')
helm upgrade -i -n plgd --create-namespace -f values.yaml --set mongodb.standbyTool.mode=active hub plgd/plgd-hub
```

Next, resume the job to configure the MongoDB replica set.

```bash
kubectl -n plgd patch job/$(kubectl -n plgd get jobs | grep mongodb-standby-tool | awk '{print $1}') --type=strategic --patch '{"spec":{"suspend":false}}'
```

The final step is to run plgd pods on the standby cluster. Set the `global.standby` flag to `false`, enable NATs via `nats.enabled=true` and upgrade the Helm chart.

```bash
helm upgrade -i -n plgd --create-namespace -f values.yaml --set mongodb.standbyTool.mode=active --set global.standby=false --set nats.enabled=true hub plgd/plgd-hub
helm upgrade -i -n plgd --create-namespace -f values.yaml --set mongodb.standbyTool.mode=active --set global.standby=false --set nats.enabled=true dps plgd/plgd-dps
```

After rotating the device provisioning endpoints, the devices will connect to the standby cluster.

#### Turn Off plgd Pods on the Primary Cluster

When the primary cluster is back up, set the `global.standby` flag to `true`, disable NATs via `nats.enabled=false` and upgrade the Helm chart.

```bash
helm upgrade -i -n plgd --create-namespace -f values.yaml --set global.standby=true --set nats.enabled=false hub plgd/plgd-hub
helm upgrade -i -n plgd --create-namespace -f values.yaml --set global.standby=true --set nats.enabled=false dps plgd/plgd-dps
```

### How to Switch Back to the Primary Cluster

When the primary cluster is ready for devices, switch back to the primary cluster.

#### Demote the Standby Cluster

First, promote the primary cluster's MongoDB hidden members to secondary members and demote the standby cluster's MongoDB secondary members to hidden. Upgrade the Helm chart with the `mongodb.standbyTool.mode` set to `standby`. To do that we need to delete the `mongodb-standby-tool` job and upgrade the Helm chart which will create a new job.

```bash
kubectl -n plgd delete job/$(kubectl -n plgd get jobs | grep mongodb-standby-tool | awk '{print $1}')
helm upgrade -i -n plgd --create-namespace -f values.yaml --set mongodb.standbyTool.mode=standby hub plgd/plgd-hub
```

Next, delete the `mongodb-standby-tool` job and resume it to configure the MongoDB replica set.

```bash
kubectl -n plgd patch job/$(kubectl -n plgd get jobs | grep mongodb-standby-tool | awk '{print $1}') --type=strategic --patch '{"spec":{"suspend":false}}'
```

The final step is to run plgd pods on the standby cluster. Set the `global.standby` flag to `true`, disable NATs via `nats.enabled=false`  and upgrade the Helm chart.

```bash
helm upgrade -i -n plgd --create-namespace -f values.yaml --set mongodb.standbyTool.mode=standby --set global.standby=true --set nats.enabled=false hub plgd/plgd-hub
helm upgrade -i -n plgd --create-namespace -f values.yaml --set mongodb.standbyTool.mode=standby --set global.standby=true --set nats.enabled=false dps plgd/plgd-dps
```

#### Turn On plgd Pods on the Primary Cluster

When the standby cluster is ready for devices, switch back to the primary cluster. Set the `global.standby` flag to `false`, enable NATs via `nats.enabled=true` and upgrade the Helm chart.

```bash
helm upgrade -i -n plgd --create-namespace -f values.yaml --set global.standby=false --set nats.enabled=true hub plgd/plgd-hub
helm upgrade -i -n plgd --create-namespace -f values.yaml --set global.standby=false --set nats.enabled=true dps plgd/plgd-dps
```

After rotating the device provisioning endpoints, the devices will connect to the primary cluster.
