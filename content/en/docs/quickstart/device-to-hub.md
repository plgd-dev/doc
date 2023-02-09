---
title: 'Remote Access'
description: 'Ensure secure remote communication with your device. This guide walks you through the onboarding process to the plgd hub.'
docsTitleDisplay: true
docsTitleIndex: 4
date: '2023-02-03'
categories: ['quickstart']
keywords: ['quickstart', 'onboarding', 'dashboard', 'd2c']
weight: 4
---

## try.plgd.cloud

You can try out the plgd hub without downloading a single docker container by going to [try.plgd.cloud](https://try.plgd.cloud). Additionally, there is a [client application](https://github.com/plgd-dev/client-application/releases) which can be used to onboard any IoTivitiy-Lite or OCF standardized devices to the plgd hub. In order to use the client application, you need to use both a [config.yaml](#configyaml-file) file that contains configuration for the plgd hub and a [ca.crt](#cacrt-file) file that contains the certificate of the plgd hub, both of which need to be in the same directory as the client-application binary.

The client application can then be run with the following command:

```bash
./client-application --config ./config.yaml
```

Once the application is running, you can open [http://localhost:3000](http://localhost:3000) in your browser. Onboarding your device requires that you Discover devices, go to specific device, own it, and finally onboard it.

### config.yaml file

```yml
log:
    level: info
    encoding: console
    stacktrace:
        enabled: false
        level: warn
    encoderConfig:
        timeEncoder: rfc3339nano
apis:
    http:
        enabled: true
        address: :3000
        tls:
            enabled: false
            caPool: null
            keyFile: ""
            certFile: ""
            clientCertificateRequired: false
        readTimeout: 8s
        readHeaderTimeout: 4s
        writeTimeout: 16s
        idleTimeout: 30s
        cors:
            allowedOrigins:
                - '*'
            allowedHeaders:
                - Accept
                - Accept-Language
                - Accept-Encoding
                - Content-Type
                - Content-Language
                - Content-Length
                - Origin
                - X-CSRF-Token
                - Authorization
            allowedMethods:
                - GET
                - PATCH
                - HEAD
                - POST
                - PUT
                - OPTIONS
                - DELETE
            allowCredentials: false
        ui:
            enabled: true
            directory: ./www
    grpc:
        enabled: false
        address: :8081
        enforcementPolicy:
            minTime: 5m0s
            permitWithoutStream: false
        keepAlive:
            maxConnectionIdle: 0s
            maxConnectionAge: 0s
            maxConnectionAgeGrace: 0s
            time: 0s
            timeout: 0s
        tls:
            enabled: false
            caPool: null
            keyFile: ""
            certFile: ""
            clientCertificateRequired: false
clients:
    device:
        coap:
            maxMessageSize: 262144
            inactivityMonitor:
                timeout: 10s
            blockwiseTransfer:
                enabled: true
                blockSize: "1024"
            ownershipTransfer:
                methods:
                    - justWorks
                manufacturerCertificate:
                    tls:
                        caPool: null
                        keyFile: ""
                        certFile: ""
            tls:
                authentication: x509
remoteProvisioning:
    ownerClaim: owner-id
    hubID: 1c10a3b6-287c-11ec-ac2d-13054959c274
    coapGateway: coaps+tcp://try.plgd.cloud:5684
    authority: https://auth.plgd.cloud/realms/shared
    certificateAuthority: https://api.try.plgd.cloud
    webOAuthClient:
        clientID: LXZ9OhKWWRYqf12W0B5OXduqt02q0zjS
        audience: https://try.plgd.cloud
        scopes:
            - openid
    deviceOAuthClient:
        clientID: cYN3p6lwNcNlOvvUhz55KvDZLQbJeDr5
        audience: ""
        scopes:
            - offline_access
        providerName: plgd.web.d2d.localhost.3000
    mode: "userAgent"
    userAgent:
        csrChallengeStateExpiration: 1m0s
    caPool:
        - ./ca.crt
```

### ca.crt file

```file
-----BEGIN CERTIFICATE-----
MIIBZjCCAQugAwIBAgIRAKtzI+VSfFl46j6bSAcm/NYwCgYIKoZIzj0EAwIwEjEQ
MA4GA1UEAxMHcGxnZC1jYTAeFw0yMjEwMTcyMTEyMTNaFw0yMzEwMTcyMTEyMTNa
MBIxEDAOBgNVBAMTB3BsZ2QtY2EwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAAR4
igB2lVcxggiGPBt4t6Qv2TcMf+1YGhWNpZ8+tCGHcIIhAVvxs2MafErth24fNbiQ
Mj/XQM7aq5xRpOsHyKQ5o0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUw
AwEB/zAdBgNVHQ4EFgQUp5ONBSJGMxEecSYMSQKr4i96PpwwCgYIKoZIzj0EAwID
SQAwRgIhAN0xA9BXNkEkfLqAgdqmPKhVdt+rgZPVLTu4bRtx2/PDAiEAhQWL4NqT
aFPz/IQs1+0I/7WaTP7l4fUMa/G3a3oYTsQ=
-----END CERTIFICATE-----
```
