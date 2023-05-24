---
title: 'Overview'
description: 'Overview of plgd deployments'
docsOthersDisplay: true
date: '2023-05-24'
categories: [deployment, kubernetes]
keywords: [deployment, kubernetes, k8s, helm, chart]
weight: 1
---

In this section, you can find several examples of how to deploy plgd hub with external OAuth2.0 Server or with mocked one for development purposes.

## Getting started

In our GitHub repository you can find `charts/` folder with plgd hub [Helm chart](https://helm.sh/docs/topics/charts/). This chart is automatically published to the Helm registry `https://charts.plgd.dev` during the release. Version of the chart is in sync with the appversion, matching the release tag _(e.g. 2.1.1)_. All configuration values are documented [here](https://github.com/plgd-dev/hub/tree/main/charts/plgd-hub#values).

### Install Certificate Manager

A communication between devices, clients and plgd hub is secured as well as between plgd hub services. To simplify the deployment and certificate management, [Certificate Manager](https://cert-manager.io/docs/), required dependency, have to be deployed in your Kubernetes cluster.

```sh
kubectl apply -f https://github.com/jetstack/cert-manager/releases/download/v1.6.0/cert-manager.yaml
```

### Register plgd Helm Chart Registry

```sh
// helm repo add plgd https://charts.plgd.dev
// helm repo update
Hang tight while we grab the latest from your chart repositories...
...Successfully got an update from the "plgd" chart repository

// helm repo list
NAME    URL
plgd    https://charts.plgd.dev

// helm search repo plgd
NAME            CHART VERSION   APP VERSION     DESCRIPTION
plgd/plgd-dps   0.9.0           0.9.0           A Helm chart for plgd device provisioning service
plgd/plgd-hub   2.7.15          2.7.15          A Helm chart for plgd-hub

```

### [Hub deployment](../hub/)

Deploys the plgd hub that allows you to manage your devices via HTTP, gRPC, Cloud2Cloud API.

### [Device Provisioning Service deployment](../device-provisioning-service/)

Extends the hub deployment with the Device Provisioning Service, that allows you to onboard devices to the hub without manual intervention.
