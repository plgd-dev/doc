---
title: 'External OAuth Server with bundle'
description: 'Use your custom external OAuth Server with plgd bundle'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [tutorials, authorization]
keywords: [bundle, oauth, auth0]
menu:
  docs:
    parent: tutorials
    weight: 10
toc: true
---

Even though the bundle start core plgd services as processes in a single container, a user has still a possibility to configure most of the services parameters. **For testing purposes**, the external OAuth Server (e.g. [Auth0](https://auth0.com)) can be set up.
To skip internal mocked OAuth Server and switch to your external one, configure following environment variables:
```yaml
    OAUTH_AUDIENCE: https://api.example.com
    OAUTH_ENDPOINT_AUTH_URL: https://auth.example.com/authorize
    OAUTH_ENDPOINT_TOKEN_URL: https://auth.example.com/oauth/token
    OAUTH_ENDPOINT: auth.example.com
    JWKS_URL: https://auth.example.com/.well-known/jwks.json
    OAUTH_CLIENT_ID: ij12OJj2J23K8KJs
    OAUTH_CLIENT_SECRET: 654hkja12asd123d
    SERVICE_OAUTH_CLIENT_ID: 412dsFf53Sj6$
    SERVICE_OAUTH_CLIENT_SECRET: 235Jgdf65jsd4Shls
    OWNER_CLAIM: sub
```

# How to configure Auth0
Assuming you have an account in the Auth0 OAuth as a service, you need to create 2 Applications and one API. Follow these steps to successfully configure bundle to run against your Auth0 instance.
1. Create new **API** in the APIs section
    1. Use name of your choice
    1. Set a unique API identifier (e.g. `https://api.example.com`)
    1. After saving open details of newly created api and **Enable Offline Access**
    1. Store the internal Auth0 API Id required for the step 2c
    1. Switch to **Permissions** tab and add `openid` scope to the list
1. Create new **Regular Web Application** in the Application section
    1. Make sure **Token Endpoint Authentication Method** is set to `None`
    1. Add `https://{FQDN}:{NGINX_PORT}` and `https://{FQDN}:{NGINX_PORT}/api/authz/callback` to **Allowed Callback URLs**
    1. Add `https://{FQDN}:{NGINX_PORT}` to **Allowed Logout URLs**
    1. Add `https://{FQDN}:{NGINX_PORT}` to **Allowed Web Origins**
    1. Open **Advanced Settings**, switch to **OAuth** tab and paste here the API Id from the step 1d
    1. Switch to **Grant Types** and make sure **only** `Implicit`, `Authorization Code` and `Refresh Token` grants are enabled
1. Create new **Machine to Machine Application** in the Application section
    1. Set **Token Endpoint Authentication Method** to `Post`
    1. Add `https://{FQDN}:{NGINX_PORT}` to **Allowed Callback URLs**
    1. Add `https://{FQDN}:{NGINX_PORT}` to **Allowed Web Origins**
    1. Open **Advanced Settings**, switch to **OAuth** tab and paste here the API Id from the step 1d
    1. Switch to **Grant Types** and make sure **only** `Client Credentials` grant is enabled
