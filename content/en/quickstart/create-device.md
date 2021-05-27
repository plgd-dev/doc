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
This guide will cover how to get the first steps while using the plgd cloud in the Raspberry Pi. This includes how to create cloud server (OCF Device) available from IoTivity-Lite open source project on the Github.

## Requirements
You will need the following: 
- Raspberry Pi board with a microSD card with [Raspberry Pi OS](https://www.raspberrypi.org/software/operating-systems/#raspberry-pi-os-32-bit)
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

2. build cloud server in the iotivity-lite folder. 
    ```shell script
    cd port/linux
    make CLOUD=1 SECURE=1 MNT=1 cloud_server 
    ```   

3. run cloud server in the iotivity-lite folder.
    ```shell script
    cd port/linux
    # execution arguments format : <device-name> <auth-code> <cis> <sid> <apn>
    ./cloud_server 
    ```
    
    Now, cloud server are ready to be on-boarded and be provisioned to connect to the plgd cloud.   
