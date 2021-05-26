---
title: 'Create device'
description: 'Start your own device and host resources'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [quickstart]
keywords: [quickstart, device, iotivity]
menu:
  docs:
    parent: quickstart
    weight: 10
toc: true
---
This guide will cover how to get the first steps while using the plgd cloud in the Raspberry Pi. This includes how to create cloud server (OCF Device) available from IoTivity-Lite open source project in the Github repository.

## Requirements
You will need the following: 
- Raspberry Pi board with a microSD card with Raspberry Pi OS(See OS)
- Optional: HDMI monitor and cable, USB keyboard and mouse for the Pi
- Ethernet or Wi-Fi connection between a DHCP-enabled network and the Pi
- Personal computer(Linux/MacOS recommended) on the same network as the Pi

## Setting up the Raspberry Pi
1. Set up your SD card as described [here](https://projects.raspberrypi.org/en/projects/raspberry-pi-setting-up/2)
2. Connect your Raspberry Pi as described [here](https://projects.raspberrypi.org/en/projects/raspberry-pi-setting-up/3)
3. Start up your Raspberry Pi as described [here](https://projects.raspberrypi.org/en/projects/raspberry-pi-setting-up/4)
4. Configuring your Raspberry Pi described [here](https://projects.raspberrypi.org/en/projects/raspberry-pi-using/9)
    
    #### Configure SSH(Remote Access)
    You make sure to enable SSH to allow remote access to your Raspberry Pi from another computer using SSH. 
    
    Simple way to enable SSH as described [here](https://www.raspberrypi.org/documentation/remote-access/ssh/README.md) is to select `Enable` next to `SSH` in the interface tap of `Raspberry Pi configuration` from the `Preferences` menu.
    
    ![Interfaces] (https://projects-static.raspberrypi.org/projects/raspberry-pi-using/e3d1233ee773b4f80821995131420119ddb7e40c/en/images/pi-configuration-interfaces.png)
    
    Or 
    
    You can also configure SSH via `raspi-config` on the Pi terminal. 
    ```shell script
    sudo raspi-config
   
    # Select 'Interfacing Options'
    # Navigate to and select 'SSH'
    # Choose 'Yes'
    # Select 'Ok'
    # Choose 'Finish'
    ```

## Connect to the Raspberry Pi via SSH described [here](https://www.raspberrypi.org/documentation/remote-access/ssh/unix.md)
1. Open a terminal on your computer.  
2. Get IP address of the Raspberry Pi board by following command on the terminal. 

    ```
    ping raspberrypi.local 
    ```
    If the ping command doesn't reach raspberrypi.local due to DNS issues, you can connect a keyboard and monitor to the Raspberry Pi and run `ifconfig` command in the terminal.

3. Connect to the Pi via SSH using these commands.

    For Linux or macOS, use either of these commands:
    ```shell script
    # replace with current hostname if changed.
    ssh pi@raspberrypi.local  
    ```
    Or 
    ```shell script
    # use IP address from previous step
    ssh pi@ipaddress  
    ```
4. Type the password when prompted. Default login username is `pi` and password is `raspberry`.

## Install essential build tools
1. Update & upgrade all the installed packages.
    ```shell script
    sudo apt-get update
    sudo apt-get upgrade
    ```
2. Install build tools and so on.
    ```shell script
    sudo apt-get install build-essential git curl openssl vim jq tac awk
    ```

## Install cloud server (OCF cloud enabled device) 
1. Download IoTivity-Lite open soure from the GitHub. 

    Continue on your personal PC, from the SSH prompt, go to the home directory and clone the latest open source of IoTivity-Lite.
    
    ```shell script
    git clone https://gitlab.iotivity.org/iotivity/iotivity-lite.git
    ```
    Enter in the iotivity-lite folder you just cloned.
    ```shell script
    cd iotivity-lite
    ls 
    Architecture.png	apps			patches			tools
    LICENSE.md		deps			port			util
    Porting.png		include			security
    README.rst		messaging		swig
    api			onboarding_tool		tests
    ```

2. Get cloud configuration.
    To use cloud server sample of IoTivity-Lite, get cloud configuration from plgd portal or guide documents.
    ```shell script
    # get cloud configuration via portal API
    curl https://portal.try.plgd.cloud/.well-known/ocfcloud-configuration
    ```
    ```shell script
    {
     "access_token_url":"https://auth.plgd.cloud/oauth/token",
     "auth_code_url":"https://auth.plgd.cloud/authorize",
     "cloud_authorization_provider":"plgd",
     "cloud_certificate_authorities":"-----BEGIN CERTIFICATE-----
       MIIBhDCCASmgAwIBAgIQdAMxveYP9Nb48xe9kRm3ajAKBggqhkjOPQQDAjAxMS8w
       LQYDVQQDEyZPQ0YgQ2xvdWQgUHJpdmF0ZSBDZXJ0aWZpY2F0ZXMgUm9vdCBDQTAe
       Fw0xOTExMDYxMjAzNTJaFw0yOTExMDMxMjAzNTJaMDExLzAtBgNVBAMTJk9DRiBD
       bG91ZCBQcml2YXRlIENlcnRpZmljYXRlcyBSb290IENBMFkwEwYHKoZIzj0CAQYI
       KoZIzj0DAQcDQgAEaNJi86t5QlZiLcJ7uRMNlcwIpmFiJf9MOqyz2GGnGVBypU6H
       lwZHY2/l5juO/O4EH2s9h3HfcR+nUG2/tFzFEaMjMCEwDgYDVR0PAQH/BAQDAgEG
       MA8GA1UdEwEB/wQFMAMBAf8wCgYIKoZIzj0EAwIDSQAwRgIhAM7gFe39UJPIjIDE
       KrtyPSIGAk0OAO8txhow1BAGV486AiEAqszg1fTfOHdE/pfs8/9ZP5gEVVkexRHZ
       JCYVaa2Spbg= 
       -----END CERTIFICATE-----",
     "cloud_id":"adebc667-1f2b-41e3-bf5c-6d6eabc68cc6",
     "cloud_url":"coaps+tcp://try.plgd.cloud:5684",
     "jwt_claim_owner_id":"sub",
     "signing_server_address":"portal.try.plgd.cloud"
     }
    ```
    Or
   
    Cloud configuration is also described [here](https://github.com/plgd-dev/cloud/blob/master/docs/guide/deployment/cloud2cloud-gateway.md#device-onboarding) like followings.
    #### Unsecured device
    
    - `apn` : `plgd`
    - `cis` : `coap+tcp://try.plgd.cloud:5683`
    - `sid` : `adebc667-1f2b-41e3-bf5c-6d6eabc68cc6`
    - `at` : `CODE_FROM_PORTAL`
    
    #### Secured device
    
    - `apn` : `plgd`
    - `cis` : `coaps+tcp://try.plgd.cloud:5684`
    - `sid` : `adebc667-1f2b-41e3-bf5c-6d6eabc68cc6`
    - `at` : `CODE_FROM_PORTAL`
    
    #### Conditions
    
    - `Device must be owned.`
    - `Cloud CA  must be set as TRUST CA with subject adebc667-1f2b-41e3-bf5c-6d6eabc68cc6 in device.`
    - `Cloud CA in PEM:`
    
    ```pem
    -----BEGIN CERTIFICATE-----
    MIIBhDCCASmgAwIBAgIQdAMxveYP9Nb48xe9kRm3ajAKBggqhkjOPQQDAjAxMS8w
    LQYDVQQDEyZPQ0YgQ2xvdWQgUHJpdmF0ZSBDZXJ0aWZpY2F0ZXMgUm9vdCBDQTAe
    Fw0xOTExMDYxMjAzNTJaFw0yOTExMDMxMjAzNTJaMDExLzAtBgNVBAMTJk9DRiBD
    bG91ZCBQcml2YXRlIENlcnRpZmljYXRlcyBSb290IENBMFkwEwYHKoZIzj0CAQYI
    KoZIzj0DAQcDQgAEaNJi86t5QlZiLcJ7uRMNlcwIpmFiJf9MOqyz2GGnGVBypU6H
    lwZHY2/l5juO/O4EH2s9h3HfcR+nUG2/tFzFEaMjMCEwDgYDVR0PAQH/BAQDAgEG
    MA8GA1UdEwEB/wQFMAMBAf8wCgYIKoZIzj0EAwIDSQAwRgIhAM7gFe39UJPIjIDE
    KrtyPSIGAk0OAO8txhow1BAGV486AiEAqszg1fTfOHdE/pfs8/9ZP5gEVVkexRHZ
    JCYVaa2Spbg=
    -----END CERTIFICATE-----
    ```
    
    - `ACL for Cloud (Subject: adebc667-1f2b-41e3-bf5c-6d6eabc68cc6) must be set with full access to all published resources in device.`   
   
    #### Root CA Setting on the source 
    ```shell script
    cd apps/pki_certs    
    vi cloudca.pem
    # Copy & paste Cloud CA into cloudca.pem
    ```
3. build cloud server in the iotivity-lite folder. 
    ```shell script
    cd port/linux
    make CLOUD=1 SECURE=1 MNT=1 cloud_server 
    ```   

4. run cloud server in the iotivity-lite folder.
    ```shell script
    cd port/linux
    # execution arguments format : <device-name> <auth-code> <cis> <sid> <apn>
    ./cloud_server 
    ```

## Prepare identity certificate for provisioning cloud server

1. get authorization code to get access token.

    #### Request authorization code
    In the web browser, type auth code url of OAuth provider on the following. 
    ```
    https://auth.plgd.cloud/authorize?response_type=code&client_id=9XjK2mCf2J0or4Ko0ow7wCmZeDTjC1mW&redirect_uri=http://localhost:8080/callback&scope=r:deviceinformation:* r:resources:* w:resources:* w:subscriptions:* offline_access&audience=https://openapi.try.plgd.cloud/&state=STATE
    ```
    You can see redirected login page for user authorization. Log in with Google, Apple and authorize application by user consent. 
    Then you can get json data including `code` and `state` in the browser.
    ```
    http://localhost:8080/callback?code=s65bpdt-ry7QEh6O&state=STATE
    ```
    Use this code `s65bpdt-ry7QEh6O` in {YOUR_AUTHORIZATION_CODE} at the next step. 
    
2. get access token to connect HTTP Gateway service.
    #### Request tokens
    In the command line, input auth token url of OAuth provider with `code` data as you got above step on the following.
    ```
    curl --request POST \
      --url 'https://auth.plgd.cloud/oauth/token' \
      --header 'content-type: application/x-www-form-urlencoded' \
      --data grant_type=authorization_code \
      --data 'client_id=9XjK2mCf2J0or4Ko0ow7wCmZeDTjC1mW' \
      --data client_secret=UTeeIsSugTuDNbn4QMdBaNLDnMiBQzQaa6elm4SDuWOdZUou-aH00EPSbBhgppFD \
      --data code={YOUR_AUTHORIZATION_CODE} \
      --data 'redirect_uri=http://localhost:8080/callback'
    ```
    You can also see json data including `access_token` and so on. 
    ```
    {
      "access_token":"eyJhbGciOiJSUzI1NiIs...",
      "scope":"r:deviceinformation:* r:resources:* w:resources:* w:subscriptions:* offline_access",
      "expires_in":86400,
      "token_type":"Bearer"
    }
    ```
    
    Use this access token `eyJhbGciOiJSUzI1NiIs...` at the next step.   

3. request identity certificate for cloud server in the iotivity-lite folder.   
    ```shell script
    curl https://raw.githubusercontent.com/plgd-dev/cloud/v2/bundle/client/get-manufacturer-certificate.sh --output get-manufacturer-certificate.sh
    chmod 755 get-manufacturer-certificate.sh
   
    # execution arguments format : -t <access_token> -a <http-gateway-host>
    ./get-manufacturer-certificate.sh -t "eyJhbGciOiJSUzI1NiIs..." -a "-k portal.try.plgd.cloud"
    ```

## Prepare authorization code for provisioning cloud server
1. get authorization code for cloud server

    #### Request authorization code
    In the web browser, type auth code url of OAuth provider on the following. 
    ```
    https://auth.plgd.cloud/authorize?response_type=code&client_id=9XjK2mCf2J0or4Ko0ow7wCmZeDTjC1mW&redirect_uri=http://localhost:8080/callback&scope=r:deviceinformation:* r:resources:* w:resources:* w:subscriptions:* offline_access&audience=https://openapi.try.plgd.cloud/&state=STATE
    ```
    You can see redirected login page for user authorization. Log in with Google, Apple and authorize application by user consent. 
    Then you can get json data including `code` and `state` in the browser.
    ```
    http://localhost:8080/callback?code=s65bpdt-ry7QEh6O&state=STATE
    ```
    Use this access token `eyJhbGciOiJSUzI1NiIs...` for provisioning cloud configuration at the next step. 
    
## Install on-boarding tool (OCF OBT & Mediator) for on-boarding and provisioning
1. build on-boarding tool in the iotivity-lite folder. 
    ```shell script
    cd port/linux
    make CLOUD=1 SECURE=1 onboarding-tool 
    ```   
2. run onboarding tool in the iotivity-lite folder.
    ```shell script
    cd port/linux
    ./onboarding-tool
    ```

3. on-board cloud server to acquire device ownership. 

4. provision cloud configuration to cloud server.

5. re-run cloud server.
```shell script
cd port/linux
# execution arguments format : <device-name> <auth-code> <cis> <sid> <apn>
./cloud_server 
```