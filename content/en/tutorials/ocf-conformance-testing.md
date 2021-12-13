---
title: 'OCF Conformance Testing'
description: 'How to validate conformance to OCF C2C and D2C API?'
date: '2021-12-11'
lastmod: '2021-12-11'
categories: [tutorials, ctt]
keywords: ['OCF conformance', 'tutorial', 'CTT tool']
menu:
  docs:
    parent: tutorials
    weight: 60
toc: true
---

The [Open Connectivity Foundation](https://openconnectivity.org) (OCF) is a global, member-driven technical standard development organization. Its [500+ members](https://openconnectivity.org/foundation/membership-list/) are working to enable trust, interoperability, and secure communication between IP-connected IoT devices and services. It does this by fostering collaboration between stakeholders across the IoT ecosystem to deliver the freely-available [ISO/IEC specifications](https://openconnectivity.org/developer/specifications/), including the Secure IP Device Framework, its [open-source reference implementation](https://openconnectivity.org/technology/), and an industry-recognized [certification program](https://openconnectivity.org/certification).

The OCF Certification Program includes conformance testing to ensure robust and secure connectivity, and to help manufacturers create products that “just work” with other OCF Certified IoT devices regardless of their form factors, operating systems, service providers or transports.

The plgd hub, reference implementation of [OCF Device to Cloud Services](https://openconnectivity.org/specs/OCF_Device_To_Cloud_Services_Specification_v2.2.4.pdf) and [OCF Cloud API for Cloud Services](https://openconnectivity.org/specs/OCF_Cloud_API_For_Cloud_Services_Specification_v2.2.4.pdf) specifications is automatically tested with the OCF Conformance Test Tool (CTT) what guaruantees interoperability between OCF Certified IoT devices and Clouds.

If you are user of the plgd hub, interested in the OCF Certification, this tutorial will guide you on how to execute required OCF test cases.

To validate conformance OCF uses a Windows-based application called Conformance Test Tool (CTT). To succeed with the validation you must have a correctly set up environment and configuration file for CTT. The following sections will explain all the necessary steps.

## Validating C2C Target Cloud Conformance

The environment used in this tutorial consists of the following parts:

* Windows 10 host machine with Ethernet connection
* Ubuntu 20.04.3 LTS running in Oracle VM VirtualBox 6.1

### Set up the Windows host machine

* Download CTT tool from <https://cms.openconnectivity.org/> (only available to registered users).
* Install CTT on the host machine. The default installation should be located in `C:\Program Files (x86)\OCF Conformance Test Tool`.
* Install [ngrok](https://ngrok.com/) and set it up as is described in the CTT User Guide located in `C:\Program Files (x86)\OCF Conformance Test Tool\Docs\Users Guide.mhtml`. Follow the section named **C2C Accessibility**.
* Examine the Ubuntu machine VirtualBox, go to Settings/Network pane. Make sure that at least one adapter is enabled and the "Attached to" setting is set to `Bridged Adapter` value.

### Run plgd hub #bundle in Ubuntu machine

The Target Cloud API is implemented by the `cloud2cloud-gateway` service, which is part of the [plgd hub #bundle](https://plgd.dev/quickstart/deploy-plgd-hub/#plgd-bundle). The services must be available also on the Windows host machine, therefore several environmental variables must be defined.

Run the following commands in your `$HOME` folder in terminal:

```shell
docker run -it --rm \
  -e FQDN=192.168.1.44 \
  --network=host ghcr.io/plgd-dev/hub/bundle:latest
```

Value for `FQDN` is machine dependent. If you have followed the previous steps and changed the network settings of the guest machine to `Bridged Adapter` then you should obtain the address by running `ifconfig` in terminal and taking the ipv4 address of your active network interface.

After correctly setting up the FQDN variable, the docker command should start up all services in the guest machine. The GUI provided by the plgd hub #bundle should be available at the address set by the `FQDN` environmental variable, in our example at `https://192.168.1.44`. Please verify it by opening the address in a browser in your Windows host machine.

#### Create CTT configuration file

To run CTT test cases, a PICS configuration file is required. Several files should be provided with your CTT installation, but some fields must be modified based on your environment. All fields are explained in the CTT _User Guide_.

```json
{
    ...
    "supportedDeviceTypes": ["oic.d.switch"],
    "authorizationEndpointUrl": "https://{FQDN}/authorize?audience=https://{FQDN}/",
    "tokenEndpointUrl": "https://{FQDN}/oauth/token",
    "c2cApiUrl": "https://{FQDN}/c2c/gw",
    "retrievalScope": "r:* offline_access",
    "updateScope": "r:* w:* offline_access",
    ...
    "cloudServerTrustAnchorCertificate": "...",
    ...
    "validClientId": "testC2C",
    "validClientSecret": "testC2CSecret",
    "supportsResourcesPublishedUnpublishedEvents": true,
    "localEventListenerUri": "https://localhost:55551/events_123",
    "proxyEventListenerUri": "https://4561-95-102-120-86.ngrok.io/events_123"
}
```

Properties `authorizationEndpointUrl`, `tokenEndpointUrl` and `c2cApiUrl` contain the address of the plgd hub #bundle against which the CTT executes tests. Please update it to match the `FQDN` value.

{{% note %}}
plgd hub #bundle contains mock OAuth2.0 Server, which simplifies development and conformance testing.
{{% /note %}}

The values for `localEventListenerUri` and `proxyEventListenerUri` are provided by the ngrok application. The guide on how to obtain them is described in the _User Guide_ section **Setup for Target Cloud test cases for Events API (CT5.3.X)**.

The `cloudServerTrustAnchorCertificate` value should contain a correctly formatted certificate. It should be formatted as a one-liner with Windows line endings, such as:

```json
"cloudServerTrustAnchorCertificate": "-----BEGIN CERTIFICATE-----\r\n ... \r\n-----END CERTIFICATE-----",
```

The certificate can be retrieved from the plgd hub #bundle by opening `https://{FQDN}/.well-known/hub-configuration`. Copy the value of the `certificateAuthorities` property into the `cloudServerTrustAnchorCertificate` value with proper formatting.

{{% warning %}}
The certificate is regenerated whenever plgd hub #bundle is started. So if you restart your bundle instance, then you must update your PICS configuration as well.
{{% /warning %}}

### Start the Conformance Test Tool

After following all of the previous steps, you should have a running plgd hub #bundle in the Ubuntu machine, a running ngrok application and a prepared PICS configuration file in the Windows machine. You can now start the CTT application. Go to `File` -> `Select IUT`, select `Target Cloud` and click `Next`. In the next pane `IUT Selection` click `Browse`, navigate to your created PICS configuration, select it and click `Next` twice. The Cloud tests list should now be loaded with 6 CT5.x.x C2C test cases available.

#### Using Iotivity-lite devices

Several of the CTT test cases require 1 or more devices to be running. Moreover, these devices must support several operations including - register, deregister, login, logout and others. Please use [cloud_tests device](https://github.com/iotivity/iotivity-lite/blob/master/apps/cloud_certification_tests.c) from the Iotivity-lite repository, which is a device created for the purpose of running cloud conformance tests.

The steps needed to compile and run `cloud_tests` device with plgd hub #bundle's mock OAuth2.0 server follow:

1. Checkout [IoTivity-Lite](https://github.com/iotivity/iotivity-lite):

    ```shell
    git clone https://github.com/iotivity/iotivity-lite.git --recursive
    cd iotivity-lite
    ```

2. Build `cloud_tests` in the `port/linux` folder:

    ```shell
    cd port/linux
    make CLOUD=1 SECURE=0 OSCORE=0 cloud_tests
    ```

3. Run `cloud_tests` in the `port/linux` folder:

    The `cloud_tests` binary has 6 optional positional arguments:

    ```shell
    1: device name
    2: authorization code
    3: cis
    4: sid
    5: apn
    6: device ID
    ```

    Choose whatever device name you want. The authorization code is not verified by the mock OAuth2.0 server, but it must be non-empty. For a device to work with the plgd hub #bundle, the `cis`, `sid` and `apn` arguments must have specific values.

    ```shell
    ./cloud_tests "ctt-1" "test" "coap+tcp://127.0.0.1:5683" "00000000-0000-0000-0000-000000000001" "plgd"
    ```

    If everything is in order, then the `cloud_tests` binary should print output similar to the following:

    ```shell
    device_name: ctt-1
    auth_code: test
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

## Validating C2C Origin Cloud Conformance
