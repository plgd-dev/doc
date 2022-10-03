---
title: "plgd bundle available"
date: 2021-01-21
description: "New deployment option for developers available. Start the plgd hub by running a single docker container!"
---

As you've might already experienced, the plgd hub consist of a few scalable services which are in the production environment deployed and scaled independently. This deployment complexity is very often a pain point for device developers who are using the plgd hub just to verify their device API implementation and its behavior.

To make the experience with the usage of the plgd hub perfect to everyone, we have created a new package - plgd bundle docker container which bundles all plgd core services and more:

- plgd Dashboard
- OAuth2.0 Mock Server
- NGINX proxy
- both secure and unsecure CoAP Gateway (device endpoint)
- CA service issuing identity certificates for your devices

Yes, you can use both secure and unsecure devices, utilize the OAuth2.0 authorization and play around with the plgd hub by running a single command:
```
docker run -d --name plgd -P plgd/bundle:vnext
```

> The plgd mobile application also works with the bundle. Just point it to the reachable IP of your plgd bundle deployment.
