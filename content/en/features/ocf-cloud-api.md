---
title: 'OCF Cloud API for Cloud Services'
description: 'How to validate conformance to OCF Cloud API for Cloud Services?'
date: '2021-11-10'
lastmod: '2021-11-11'
categories: [features]
keywords: ['OCF conformance', 'tutorial']
menu:
  docs:
    parent: features
    weight: 10
toc: true
---

The OCF Cloud API for Cloud Services is a set of well-defined APIs used for device information retrieval, update and event subscription between two OCF Cloud instances. The [full specification](https://openconnectivity.org/specs/OCF_Cloud_API_For_Cloud_Services_Specification_v2.2.4.pdf) is available at The Open Connectivity Foundation (OCF) website.

Plgd.dev services `cloud2cloud-gateway` and `cloud2cloud-connector` provide an implementation of the APIs. To validate conformance OCF uses a Windows-based application called Conformance Test Tool (CTT). To succeed with the validation you must have a correctly set up environment and configuration file for CTT. The following sections will explain all the necessary steps.

## Validating Target Cloud conformance with Conformance Test Tool

The environment used in this tutorial consists of the following parts:

* Windows 10 host machine with Ethernet connection
* Ubuntu 20.04.3 LTS running in Oracle VM VirtualBox 6.1

### Set up the Windows host machine

* Download CTT tool from <https://cms.openconnectivity.org/> (only available to registered users).
* Install CTT on the host machine. The default installation should be located in `C:\Program Files (x86)\OCF Conformance Test Tool`.
* Install [ngrok](https://ngrok.com/) and set it up as is described in the CTT User Guide located in `C:\Program Files (x86)\OCF Conformance Test Tool\Docs\Users Guide.mhtml`. Follow the section named **C2C Accessibility**.
* Examine the Ubuntu machine VirtualBox, go to Settings/Network pane. Make sure that at least one adapter is enabled and the "Attached to" setting is set to `Bridged Adapter` value.

### Run OCF Cloud Bundle in Ubuntu machine

The Target Cloud part of the API is implemented by the `cloud2cloud-gateway` service, which is included by the plgd.dev's [OCF Cloud Bundle implementation](https://github.com/plgd-dev/hub/tree/main/bundle). The services must be available also on the Windows host machine, therefore several environmental variables must be defined.

Run the following commands in your `$HOME` folder in terminal:

```shell
docker run -it --rm \
  -e FQDN=192.168.1.44 \
  -e OAUTH_AUDIENCE=https://try.plgd.cloud \
  -e OAUTH_ENDPOINT=auth.plgd.cloud \
  -e OAUTH_CLIENT_ID=TODO \
  -e OAUTH_CLIENT_SECRET=TODO \
  -e DEVICE_OAUTH_REDIRECT_URL=https://192.168.1.44/things \
  -e DEVICE_OAUTH_SCOPES=offline_access \
  --network=host -v `pwd`/.tmp/data:/data ghcr.io/plgd-dev/hub/bundle:latest
```

Values for `FQDN` and `DEVICE_OAUTH_REDIRECT_URL` are machine dependent. If you have followed the previous steps and changed the network settings of the guest machine to `Bridged Adapter` then you should obtain the address by running `ifconfig` in terminal and taking the ipv4 address of your active network interface.

Additionally, before running the docker command, the `FQDN` and `DEVICE_OAUTH_REDIRECT_URL` must be added to "Allowed Callback URLs" and `FQDN` to "Allowed Web Origins" in the configuration of the auth0 service.

After correctly setting up the variables and auth0, the docker command should start up all services in the guest machine. The GUI provided by the OCF Cloud Bundle should be available at the address set by the `FQDN` environmental variable (`192.168.1.44` in this example). You can verify it by opening the address in a browser in your Windows host machine.

#### Create CTT configuration file

To run CTT test cases, a PICS configuration file is required. Several files should be provided with your CTT installation, but some fields must be modified based on your environment. All fields are explained in the CTT _User Guide_.

```json
{
    ...
    "supportedDeviceTypes": ["oic.d.switch"],
    "authorizationEndpointUrl": "https://auth.plgd.cloud/authorize?audience=https://openapi.try.plgd.cloud/",
    "tokenEndpointUrl": "https://auth.plgd.cloud/oauth/token",
    "c2cApiUrl": "https://192.168.1.44/c2c/gw",
    "retrievalScope": "r:* offline_access",
    "updateScope": "r:* w:* offline_access",
    ...
    "cloudServerTrustAnchorCertificate": "...",
    ...
    "validClientId": "9XjK2mCf2J0or4Ko0ow7wCmZeDTjC1mW",
    "validClientSecret": "UTeeIsSugTuDNbn4QMdBaNLDnMiBQzQaa6elm4SDuWOdZUou-aH00EPSbBhgppFD",
    "supportsResourcesPublishedUnpublishedEvents": true,
    "localEventListenerUri": "https://localhost:55551/events_123",
    "proxyEventListenerUri": "https://4561-95-102-120-86.ngrok.io/events_123"
}
```

The fields `authorizationEndpointUrl`, `tokenEndpointUrl`, `validClientId` and `validClientSecret` values are configured to work with a running plgd-dev OAuth2.0 client. Working `validClientId` and `validClientSecret` should always be available in the [cloud2cloud-gateway documentation](https://plgd.dev/configuration/cloud2cloud-gateway/).

The values for `localEventListenerUri` and `proxyEventListenerUri` are provided by the ngrok application. The guide on how to obtain them is described in the _User Guide_ section **Setup for Target Cloud test cases for Events API (CT5.3.X)**.

The `cloudServerTrustAnchorCertificate` value should contain a correctly formatted certificate. It should be formatted as a one-liner with Windows line endings, such as:

```json
"cloudServerTrustAnchorCertificate": "-----BEGIN CERTIFICATE-----\r\n ... \r\n-----END CERTIFICATE-----",
```

The certificate is found in the folder where you ran the docker command. If you ran it in `$HOME` folder, then the location is `$HOME/.tmp/data/certs/root_ca.crt`. Copy the contents of the file into the `cloudServerTrustAnchorCertificate` value with proper formatting.
The certificate is regenerated whenever OCF Cloud Bundle is started. So if you restart your bundle instance, then you must update your PICS configuration as well.

### Start the Conformance Test Tool

After following all of the previous steps, you should have a running OCF Cloud Bundle in the Ubuntu machine, a running ngrok application and a prepared PICS configuration file in the Windows machine. You can now start the CTT application. Go to `File` -> `Select IUT`, select `Target Cloud` and click `Next`. In the next pane `IUT Selection` click `Browse`, navigate to your created PICS configuration, select it and click `Next` twice. The Cloud tests list should now be loaded with 6 CT5.x.x C2C test cases available.

#### Using Iotivity-lite devices

Several of the CTT test cases require 1 or more devices to by running. Moreover, these devices must support several operations including - register, deregister, login, logout and others. You can use [cloud_tests device](https://github.com/iotivity/iotivity-lite/blob/master/apps/cloud_certification_tests.c) from the Iotivity-lite repository. This device supports all operations required by CTT.

How to compile and run `cloud_tests` device:

1. Checkout [IoTivity-Lite](https://github.com/iotivity/iotivity-lite):

    ```shell
    git clone https://github.com/iotivity/iotivity-lite.git --recursive
    cd iotivity-lite
    ```

2. Build `cloud_tests` in the `port/linux` folder:

    ```shell
    cd port/linux
    make CLOUD=1 SERVER=1 SECURE=0 MNT=0 OSCORE=0 cloud_tests
    ```

3. Get authorization code from try.plgd.cloud

    In the following request, change the `redirect_uri` value to the address of your machine and copy the URL into your browser.

    ```url
    https://auth.plgd.cloud/authorize?response_type=code&client_id=cYN3p6lwNcNlOvvUhz55KvDZLQbJeDr5&scope=offline_access&audience=https://try.plgd.cloud&redirect_uri=https://192.168.1.44/things
    ```

    You should receive an HTTP 302 response and the authorization code will be included at the end of the URL.

    ```url
    https://192.168.1.44/things?code=62InBe9UE_P97kul
    ```

4. Run `cloud_tests` in the `port/linux` folder:

    The `cloud_tests` binary has 6 optional positional arguments:

    ```shell
    1: device name
    2: authorization code
    3: cis
    4: sid
    5: apn
    6: device ID
    ```

    Choose whatever device name you want. The authorization code must be the code received in the previous step and can be used only once to register. If you want to run several devices, you must get a unique authorization code for each. For a device to work with the OCF Cloud Bundle, the `cis`, `sid` and `apn` must have specific value.

    ```shell
    ./cloud_tests "ctt-1" "62InBe9UE_P97kul" "coap+tcp://127.0.0.1:5683" "00000000-0000-0000-0000-000000000001" "plgd"
    ```

    If everything is in order, then the `cloud_tests` binary should print output similar to the following:

    ```shell
    device_name: ctt-1
    auth_code: 62InBe9UE_P97kul
    cis : coap+tcp://127.0.0.1:5683
    sid: 00000000-0000-0000-0000-000000000001
    apn: plgd


    ################################################
    OCF Cloud-connected Device Certification Test Tool
    ################################################
    [0] Display this menu
    -----------------------------------------------
    [1] Cloud Register
    [2] Cloud Login
    [3] Cloud Logout
    [4] Cloud DeRegister
    [5] Cloud Refresh Token
    [6] Publish Resources
    [7] Send Ping
    [8] Unpublish switch resource
    [9] Publish switch resource
    -----------------------------------------------
    -----------------------------------------------
    [10] Exit
    ################################################

    Select option:
    ```

    Input the option number to issue the selected command to the binary.

#### Troubleshooting

##### CT5.3.1 C2C: Events API — Devices-level event types

Some steps of this test case require a device to be deregistered and then registered again with the same device ID. However, the authorization code is for one time use only. Thus, executing the `Cloud Deregister` command and then trying to issue a `Cloud Register` command will fail. The workaround is to exit the currently running `cloud_tests` process, get a new authorization code and start a new `cloud_tests` process with the same device ID.

Use the OCF Cloud Bundle GUI to get the device ID. Then simply use the last positional argument of the `cloud_tests` binary to force the new process to have the same device ID as the previously exited process:

```shell
 ./cloud_tests "DEVICE_NAME" "NEW_AUTHORIZATION_CODE" "coap+tcp://127.0.0.1:5683" "00000000-0000-0000-0000-000000000001" "plgd" "DEVICE_ID"
```

Substitute for `DEVICE_NAME` and `DEVICE_ID` the device name and the device ID of the exited process; for `NEW_AUTHORIZATION_CODE` substitute a new authorization code received from try.plgd.cloud as previously described.

## Validating Origin Cloud conformance with Conformance Test Tool

TODO
