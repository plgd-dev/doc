---
title: 'Deploy your own plgd cloud'
description: 'Deploy plgd cloud on your own development machine'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [quickstart]
keywords: [quickstart, bundle, deployment, d2c]
menu:
  docs:
    parent: quickstart
    weight: 40
toc: true
---

There are multiple ways how to start using / testing the plgd Cloud on your own infrastructure. If you're just trying to get in touch with this IoT framework, go back to [Control device remotely](./device-to-cloud.md) tutorial and use our free [try.plgd.cloud](https://tryplgd.cloud) instance. In case you want to **get in touch** with the system localy and you have the [Docker installed](https://docs.docker.com/get-docker/), use our [plgd cloud #Bundle](#plgd-bundle).

{{% note %}}
Deployment using helm chart is in progress.
{{% /note %}}

# plgd Bundle
Bundle deployment hosts core plgd Cloud Services with mocked OAuth Server in a single Docker image. All services which hosts the gRPC or HTTP API are proxied through the NGINX with configurable `NGINX_PORT` and `FQDN`. Mobile application used in the [Control device remotely](../device-to-cloud) works also with the bundle.

{{% warning %}}
Bundle version of plgd services should be used only for simple testing and development purposes. Performance evaluations, production environment or other sensitive deployments should deploy plgd services using the plgd HELM chart.
{{% /warning %}}

## Run on localhost
To deploy and access plgd cloud on your local PC using bundle, run single command:
```bash
docker run -d --name plgd -p 443:443 -p 5683:5683 -p 5684:5684 plgd/bundle:v2next
```
After couple of seconds your plgd cloud will become available. The plgd dashboard can be opened in your browser at [https://localhost/](https://localhost/).
>Note that bundle issues it's own **self-signed certificate** which needs to be accepted in the browser.

## Authorization
The plgd cloud doesn't work without OAuth Server. To not require developers not interested in sharing bundle instance with other users, simple mocked OAuth Server is included in the bundle. Authentication to the plgd is therefore not required and test user is automatically logged in. Same applies also to device connections; in case you're using the bundle, devices connecting to the CoAP Gateway can use random/static onboarding code as it's not verified. Onboarding of devices is therefore much simpler.

{{% warning %}}
Authorization Service which is part of the plgd is only for testing and development purposes. For the production, integration of the plgd device identity management API is required.
{{% /warning %}}

Even for the development and testing, more complex scenarios are supported by the built-in authorization service. Read more in the [Tutorials](../../tutorials/).

## Troubleshooting
- By default the plgd cloud bundle hosts the NGINX proxy on port `443`. This port might be already occupied by other process, e.g. Skype. Default port can be changed by environment variable `-e NGINX_PORT=8443`. Please be aware that the port needs to be exposed from the container -> `-p 443:443` needs to be changed to match a new port, e.g. `-p 8443:8443`.
- Logs and data are by default stored at `/data` path. Run the container with `-v $PWD/vol/plgd/data:/data` to be able to analyze the logs in case of an issue.
- In case you need support, we are happy to support you on [Gitter](http://gitter.im/ocfcloud/Lobby)
- OCF UCI (Cloud2Cloud Gateway) is not part of the bundle

# Kubernetes
coming soon...