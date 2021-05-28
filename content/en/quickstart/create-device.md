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
This guide will walk you through the process of starting your secure [OCF Device](https://openconnectivity.org/specs/OCF_Device_Specification_v2.2.3.pdf) using [IoTivity](https://iotivity.org/) library on the Raspberry Pi.

## Requirements
- Raspberry Pi board with a microSD card with [Raspberry Pi OS](https://www.raspberrypi.org/software/operating-systems/#raspberry-pi-os-32-bit)
- Optional: HDMI monitor and cable, USB keyboard and mouse for the Pi
- Ethernet or Wi-Fi connection between a DHCP-enabled network and the Pi
- Personal computer(Linux/MacOS recommended) on the same network as the Pi

## Setting up the Raspberry Pi
To set up your Raspberry Pi, please follow [this step-by-step tutorial](https://projects.raspberrypi.org/en/projects/raspberry-pi-setting-up).
    
{{% note %}}
Enabling SSH is highly recommended as it simplifies access to device and fast adoption of the code if required. More information available [here](https://www.raspberrypi.org/documentation/remote-access/ssh/README.md).
{{% note %}}

More simple way to enable SSH is to use the `Preferences` menu on the Desktop window.
```
1. Launch 'Raspberry Pi Configuration' from the 'Preferences' menu
2. Navigate to the 'Interfaces tab'
3. Select 'Enabled' next to 'SSH'
4. Click 'OK'
```

![Interfaces] (/images/quickstart/pi-configuration-interfaces.png)

Or 

You can also enable SSH via `raspi-config` on the Pi terminal. 
```
1. Enter 'sudo raspi-config' in a terminal window
2. Select 'Interfacing Options'
3. Navigate to and select 'SSH'
4. Choose 'Yes'
5. Select 'Ok'
6. Choose 'Finish'
```

## Connect to the Raspberry Pi via SSH
More information how to access Raspberry Pi remotely can be found [here](https://www.raspberrypi.org/documentation/remote-access/ssh/unix.md).

For Linux or macOS, use this commands:
```shell script
# use IP address of the Raspberry Pi.
ssh pi@ipaddress  
```
{{% note %}}
Default login username is `pi` and password is `raspberry` if you didn't change.
{{% note %}}

## Install essential build tools
Before you install Cloud Server example, you'd better update available package information and install build tools and so on.
```shell script
sudo apt-get update & apt-get install build-essential git curl openssl
```

## Install IoTivity Cloud Server example 
1. Checkout [IoTivity-Lite](https://github.com/iotivity/iotivity-lite) libaray. 

    Execute commands on the Raspberry PI.
    
    ```shell script
    git clone https://gitlab.iotivity.org/iotivity/iotivity-lite.git --recursive
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

2. Build Cloud Server example in the iotivity-lite folder. 
    ```shell script
    cd port/linux
    make CLOUD=1 SECURE=1 MNT=1 cloud_server 
    ```   

3. Run Cloud Server example in the iotivity-lite folder.
    ```shell script
    cd port/linux
    # execution arguments format : <device-name> <auth-code> <cis> <sid> <apn>
    ./cloud_server 
    ```
    
Now, Cloud Server example are ready to be on-boarded and be provisioned to connect to the plgd cloud.   
