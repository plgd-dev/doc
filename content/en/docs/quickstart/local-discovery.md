---
title: 'Local Discovery'
description: 'Discover, update and subscribe to changes with ease! This guide will show you how to find your device, update its properties and stay up-to-date.'
docsTitleDisplay: true
docsTitleIndex: 2
date: '2023-02-03'
categories: ['quickstart']
keywords: ['quickstart', 'discovery', 'coap', 'multicast', 'd2d']
weight: 2
---
This guide will walk you through the process of discovering, on-boarding, controlling your secure [OCF Device](https://openconnectivity.org/specs/OCF_Device_Specification_v2.2.6.pdf) using **plgd/client-application** on your PC.

## Prerequisite

- The Cloud Server is running on your Raspberry Pi.

## Install client application

1. Download the latest version of the client application from [here](https://github.com/plgd-dev/client-application/releases)

    ```shell script
    curl https://github.com/plgd-dev/client-application/releases/download/v0.4.3/client-application_0.4.3_linux_amd64.tar.gz --output client-application_0.4.3_linux_amd64.tar.gz
    tar -xzf client-application_0.4.3_linux_amd64.tar.gz
    ```

2. Run the client application:

    ```shell script
    cd client-application_0.4.3_linux_amd64
    ./client-application
    ```

The Client Application is now running on your PC. The application has opened a port for access from a browser via [http://localhost:8080](http://localhost:8080).
By running the client application, the client application will automatically create a config.yaml file and www directory in the current directory.

{{< note >}}
To onboard devices to the try.plgd.cloud, use the [config.yaml](../remote-access/#configyaml-file) file.
To read more about how to change the config.yaml, continue [here](https://github.com/plgd-dev/client-application#yaml-configuration).
{{< /note >}}

## How to Use the Client Application

### Initializing the Client Application

By default, the client application will ask you to set `Subject ID` and `Key` for the first time. The `Subject ID` is the unique identifier of the client-application, and the `Key` is the password for communication with the devices. The `Subject ID` and `Key` will be used to authenticate the device. After pressing the `Initialize` button, the client application will store the `Subject ID` and `Key` in the config.yaml file.

### Discovering Devices on the Local Network

After initializing the client application, the discovery of devices on the local network will start automatically. The client application will discover all devices on the local network and display the list of devices on the web page. The `Discover` button appends new devices to the list of devices. To remove old devices, please click the `Flush Cache` button and then Discover.

### Transfer ownership (On-boarding)

Click on an unowned device to open the device section. After that, just click the `Own` button. After that, the `Subject ID` with `Key` becomes the owner of the device.

{{< note >}}
The device needs to support the [OCF Secure Specification](https://openconnectivity.org/specs/OCF_Secure_Specification_v2.2.6.pdf) without oscore.
For iotivity-lite, it needs to be compiled with the `-DOC_SECURE_ENABLED=ON -DOC_OSCORE_ENABLED=OFF` cmake flag.
{{< /note >}}

To reset ownership of the OCF device, click on `Disown` button.

### Onboard the device to hub

Once a device is owned, it can be on-boarded to the hub. To onboard a device to the hub, click on the Onboard button. If the hub configuration is set in the config.yaml file, the device will be onboarded to the hub automatically. Otherwise, you will need to fill out the modal window with the onboard configuration of the hub. To obtain the configuration of the hub, open the hub UI and click on the `+ Device` button, then enter the `device ID`. You can then copy the configuration from the hub modal window to the client application modal window. After that, the device will be onboarded to the hub.

{{< note >}}
To be onboarded to the hub, the device must support the [OCF Device to Cloud Services Specification](https://openconnectivity.org/specs/OCF_Device_To_Cloud_Services_Specification.pdf).
For iotivity-lite, it needs to be compiled with the `-DOC_CLOUD_ENABLED=ON` CMake flag.
{{< /note >}}

To off-board the device from the hub, click on the `Offboard` button. After that, the device will be off-boarded from the hub.

### Update / Retrieve a resource of the device

To update or retrieve a resource of the device, locate the resource in the list of discovered devices and click on its name. This will open a modal window displaying the resource's details, including its value, resource type, and attributes.

To update the resource, modify the value field and click the `Update` button. To retrieve the resource, click the `Retrieve` button.