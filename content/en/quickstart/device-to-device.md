---
title: 'Discover & control device locally'
description: 'Discover devices using UDP CoAP multicast and control them locally'
date: '2021-05-13'
lastmod: '2021-06-18'
categories: [quickstart]
keywords: [quickstart, discovery, coap, multicast, d2d]
menu:
  docs:
    parent: quickstart
    weight: 20
toc: true
---
This guide will walk you through the process of discovering, on-boarding, controlling your secure [OCF Device](https://openconnectivity.org/specs/OCF_Device_Specification_v2.2.3.pdf) using **plgd/sdk** library on your PC.

## Prerequisite
- The Cloud Server runs on your Raspberry Pi

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
Now, the OCF Client is running on your PC to discover your device and so on. More information about full application sources can be found [here](https://github.com/plgd-dev/sdk/tree/v2/cmd/ocfclient).

## How to use OCF Client example
### Discover devices on local network 

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

### Transfer ownership (On-boarding)
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


### Interact with the device locally
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
      
### Reset ownership (Off-boarding)
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
  
## How to make OCF Client on your own
To make your own OCF Client application, you can use following codes included in [main.go](https://github.com/plgd-dev/sdk/blob/v2/cmd/ocfclient/main.go), [ocfclient.go](https://github.com/plgd-dev/sdk/blob/v2/cmd/ocfclient/ocfclient.go).

### Initialize OCF Client 
```gotemplate
func (c *OCFClient) Initialize() error {

    localClient, err := NewSDKClient()
    if err != nil {
        return err
    }

    c.client = localClient
    return nil
}

func (c *OCFClient) Close() error {
    if c.client != nil {
        return c.client.Close(context.Background())
    }
    return nil
}
```

### Create secure SDK Client: 
```gotemplate
func NewSDKClient() (*local.Client, error) {
    mfgTrustedCABlock, _ := pem.Decode(MfgTrustedCA)
    if mfgTrustedCABlock == nil {
        return nil, fmt.Errorf("mfgTrustedCABlock is empty")
    }
    mfgCA, err := x509.ParseCertificates(mfgTrustedCABlock.Bytes)
    if err != nil {
        return nil, err
    }
    mfgCert, err := tls.X509KeyPair(MfgCert, MfgKey)
    if err != nil {
        return nil, fmt.Errorf("cannot X509KeyPair: %w", err)
    }

    identityTrustedCABlock, _ := pem.Decode(IdentityTrustedCA)
    if identityTrustedCABlock == nil {
        return nil, fmt.Errorf("identityTrustedCABlock is empty")
    }
    identityTrustedCACert, err := x509.ParseCertificates(identityTrustedCABlock.Bytes)
    if err != nil {
        return nil, fmt.Errorf("cannot parse cert: %w", err)
    }

    cfg := local.Config{
        DisablePeerTCPSignalMessageCSMs: true,
        DeviceOwnershipSDK: &local.DeviceOwnershipSDKConfig{
            ID:      CertIdentity,
            Cert:    string(IdentityIntermediateCA),
            CertKey: string(IdentityIntermediateCAKey),
        },
    }

    client, err := local.NewClientFromConfig(&cfg, &SetupSecureClient{
        mfgCA:   mfgCA,
        mfgCert: mfgCert,
        ca:      append(identityTrustedCACert),
    }, test.NewIdentityCertificateSigner, func(err error) {},
    )
    if err != nil {
        return nil, err
    }
    err = client.Initialization(context.Background())
    if err != nil {
        return nil, err
    }

    return client, nil
}
``` 

### Discover devices on the local network
```gotemplate
func (c *OCFClient) Discover(timeoutSeconds int) (string, error) {
	ctx, cancel := context.WithTimeout(context.Background(), time.Duration(timeoutSeconds)*time.Second)
	defer cancel()
	res, err := c.client.GetDevices(ctx, local.WithError(func(error) {}))
	if err != nil {
		return "", err
	}

	deviceInfo := []interface{}{}
	devices := []local.DeviceDetails{}
	for _, device := range res {
		if device.IsSecured {
			devices = append(devices, device)
			devInfo := map[string]interface{}{"id":device.ID, "name":device.Ownership.Name, "owned":device.Ownership.Owned,
				"ownerID":device.Ownership.OwnerID, "details":device.Details}
			deviceInfo = append(deviceInfo, devInfo)
		}
	}
	c.devices = devices

	devicesJSON, err := enjson.MarshalIndent(deviceInfo, "", "    ")
	if err != nil {
		return "", err
	}
	return string(devicesJSON), nil
}
```

### Transfer ownership to the device
```gotemplate
func (c *OCFClient) OwnDevice(deviceID string) (string, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	defer cancel()
	return c.client.OwnDevice(ctx, deviceID, local.WithOTM(local.OTMType_JustWorks))
}
```

### Get resources of the device 
```gotemplate
func (c *OCFClient) GetResources(deviceID string) (string, error) {
	ctx, cancel := context.WithTimeout(context.Background(), Timeout)
	defer cancel()
	_, links, err := c.client.GetRefDevice(ctx, deviceID)

	resourcesInfo := []map[string]interface{}{}
	for _, link := range links {
		info := map[string]interface{}{"href":link.Href} 
		resourcesInfo = append(resourcesInfo, info)
	}

	linksJSON, err := enjson.MarshalIndent(resourcesInfo, "", "    ")
	if err != nil {
		return "", err
	}
	return string(linksJSON), nil
}
```

### Get a resource of the device 
```gotemplate
func (c *OCFClient) GetResource(deviceID, href string) (string, error) {
	ctx, cancel := context.WithTimeout(context.Background(), Timeout)
	defer cancel()

	var got interface{} 
	opts := []local.GetOption{local.WithInterface("oic.if.baseline")}
	err := c.client.GetResource(ctx, deviceID, href, &got, opts...)
	if err != nil {
		return "", err
	}

	var resourceJSON bytes.Buffer
	resourceBytes, err := json.Encode(got)
	err = enjson.Indent(&resourceJSON, resourceBytes, "", "    ")
	if err != nil {
		return "", err
	}
	return string(resourceJSON.Bytes()), nil
}
```
### Update a resource of the device 
```gotemplate
func (c *OCFClient) UpdateResource(deviceID string, href string, data interface{}) (string, error) {

	ctx, cancel := context.WithTimeout(context.Background(), Timeout)
	defer cancel()

	opts := []local.UpdateOption{local.WithInterface("oic.if.rw")}
	err := c.client.UpdateResource(ctx, deviceID, href, data, nil, opts...)
	if err != nil {
		return "", err
	}

	return "", nil
}
```

### Reset ownership of the device 
```gotemplate
func (c *OCFClient) DisownDevice(deviceID string) error {
	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	defer cancel()
	return c.client.DisownDevice(ctx, deviceID)
}
```