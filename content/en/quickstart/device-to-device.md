---
title: 'Discover & control device locally'
description: 'Discover devices using UDP CoAP multicast and control them locally'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [quickstart]
keywords: [quickstart, discovery, coap, multicast, d2d]
menu:
  docs:
    parent: quickstart
    weight: 20
toc: true
---
This guide will walk you through the process of discovering/on-boarding/controlling your secure [OCF Device](https://openconnectivity.org/specs/OCF_Device_Specification_v2.2.3.pdf) using **plgd/sdk** library on your PC.

## Prerequisite
- The Cloud Server example should be running on your Raspberry Pi at the step of [Create device](../create-device)

## Install OCF Client example
1. Checkout [plgd/sdk](https://github.com/plgd-dev/sdk/tree/v2) library:
    ```shell script
    # execute commands on your PC
    git clone -b v2 https://github.com/plgd-dev/sdk.git --recursive
    cd sdk
    ```
2. Build OCF Client example:
    ```shell script
    cd cmd/ocfclient
    go build
    ```   

3. Run OCF Client example in the same folder:
    ```shell script
    ./ocfclient 
    ```

## Discover device on local network 
To discover OCF devices (i.e. Cloud Server example), please follow below step: 
```shell script
#################### OCF Client for D2D ####################
[0] Display this menu
--------------------------------------------------------------
[1] Discover devices
[2] Transfer ownership to the device (On-boarding)
[3] Retrieve resources of the device
[4] Retrieve a resource of the device
[5] Update a resource of the device
[6] Reset ownership of the device (Off-boarding)
--------------------------------------------------------------
[99] Exit
##############################################################

Select menu : 1

Discovered devices : 
[
    {
        "details": {
            "di": "e2f7b281-d919-4e4b-4ed9-6156caead050",
            "rt": [
                "oic.d.cloudDevice",
                "oic.wk.d"
            ],
            "if": [
                "oic.if.r",
                "oic.if.baseline"
            ],
            "n": "",
            "dmn": null,
            "dmno": "",
            "piid": "5562fdd5-cdc1-49a3-63c5-591be089b393"
        },
        "id": "e2f7b281-d919-4e4b-4ed9-6156caead050",
        "name": "",
        "owned": false,
        "ownerID": "00000000-0000-0000-0000-000000000000"
    }
]
```

## Transfer ownership (On-boarding)
To transfer ownership to the OCF device, please follow below step: 
```shell script
#################### OCF Client for D2D ####################
[0] Display this menu
--------------------------------------------------------------
[1] Discover devices
[2] Transfer ownership to the device (On-boarding)
[3] Retrieve resources of the device
[4] Retrieve a resource of the device
[5] Update a resource of the device
[6] Reset ownership of the device (Off-boarding)
--------------------------------------------------------------
[99] Exit
##############################################################

Select menu : 2

Input device ID : e2f7b281-d919-4e4b-4ed9-6156caead050

Transferring Ownership of e2f7b281-d919-4e4b-4ed9-6156caead050 was successful  : 
e2f7b281-d919-4e4b-4ed9-6156caead050
```


## Interact with the device locally
To interact with OCF device including retrieving, updating via CoAP over udp message, please follow below step: 
1. Retrieve resources of the device:
    ```shell script
    #################### OCF Client for D2D ####################
    [0] Display this menu
    --------------------------------------------------------------
    [1] Discover devices
    [2] Transfer ownership to the device (On-boarding)
    [3] Retrieve resources of the device
    [4] Retrieve a resource of the device
    [5] Update a resource of the device
    [6] Reset ownership of the device (Off-boarding)
    --------------------------------------------------------------
    [99] Exit
    ##############################################################
    
    Select menu : 3
    
    Input device ID : e2f7b281-d919-4e4b-4ed9-6156caead050
    
    Resources of e2f7b281-d919-4e4b-4ed9-6156caead050 : 
    [
        {   "href": "/oic/p"   },
        {   "href": "/oic/res"   },
        {   "href": "/oic/d"   },
        {   "href": "/oc/wk/introspection"   },
        {   "href": "/oc/con"   },
        {   "href": "/oic/mnt"   },
        {   "href": "/oic/sec/doxm"   },
        {   "href": "/oic/sec/pstat"   },
        {   "href": "/oic/sec/acl2"   },
        {   "href": "/oic/sec/ael"   },
        {   "href": "/oic/sec/cred"   },
        {   "href": "/oic/sec/sp"   },
        {   "href": "/oic/sec/csr"   },
        {   "href": "/oic/sec/roles"   },
        {   "href": "/oic/sec/sdi"      },
        {   "href": "/CoapCloudConfResURI"   },
        {   "href": "/light/1"   },
        {   "href": "/switches"   }
    ]
    ```

2. Retrieve a resource of the device:
    ```shell script
    #################### OCF Client for D2D ####################
    [0] Display this menu
    --------------------------------------------------------------
    [1] Discover devices
    [2] Transfer ownership to the device (On-boarding)
    [3] Retrieve resources of the device
    [4] Retrieve a resource of the device
    [5] Update a resource of the device
    [6] Reset ownership of the device (Off-boarding)
    --------------------------------------------------------------
    [99] Exit
    ##############################################################
    
    Select menu : 4
    
    Input device ID : e2f7b281-d919-4e4b-4ed9-6156caead050
    ...
    Input resource href : /light/1
    
    Resource properties of e2f7b281-d919-4e4b-4ed9-6156caead050/light/1 : 
    {
        "if": [
            "oic.if.rw",
            "oic.if.baseline"
        ],
        "name": "Light",
        "power": 100,
        "rt": [
            "core.light"
        ],
        "state": true
    }
    ```

3. Update a resource of the device:
    ```shell script
    #################### OCF Client for D2D ####################
    [0] Display this menu
    --------------------------------------------------------------
    [1] Discover devices
    [2] Transfer ownership to the device (On-boarding)
    [3] Retrieve resources of the device
    [4] Retrieve a resource of the device
    [5] Update a resource of the device
    [6] Reset ownership of the device (Off-boarding)
    --------------------------------------------------------------
    [99] Exit
    ##############################################################
    
    Select menu : 5
    
    Input device ID : e2f7b281-d919-4e4b-4ed9-6156caead050
    ...
    Input resource href : /light/1
   
    Resource properties of e2f7b281-d919-4e4b-4ed9-6156caead050/light/1 : 
    {
        "if": [
            "oic.if.rw",
            "oic.if.baseline"
        ],
        "name": "Light",
        "power": 100,
        "rt": [
            "core.light"
        ],
        "state": true
    }
   
    Input property name : power
    
    Input property value : 55
    
    Property data to update : {"power":55}
    
    Updating resource property of e2f7b281-d919-4e4b-4ed9-6156caead050/light/1 was successful
    ```
      
## Reset ownership (Off-boarding)
To reset ownership of the OCF device, please follow below step: 
```shell script
#################### OCF Client for D2D ####################
[0] Display this menu
--------------------------------------------------------------
[1] Discover devices
[2] Transfer ownership to the device (On-boarding)
[3] Retrieve resources of the device
[4] Retrieve a resource of the device
[5] Update a resource of the device
[6] Reset ownership of the device (Off-boarding)
--------------------------------------------------------------
[99] Exit
##############################################################

Select menu : 6

Input device ID : e2f7b281-d919-4e4b-4ed9-6156caead050

Off-boarding e2f7b281-d919-4e4b-4ed9-6156caead050 was successful
```
  
  