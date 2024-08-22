---
title: 'Verify Device Onboarding'
description: 'How to test Device Provisioning Service'
docsOthersDisplay: true
date: '2023-05-18'
categories: [deployment, kubernetes]
keywords: [deployment, kubernetes, k8s, helm, chart]
weight: 20
---

To verify device onboarding to the Device Provisioning Service for [Trusted Network](#onboarding-device-in-trusted-network) or [Zero Trusted Network](#onboarding-device-in-zero-trust-network) follow these steps:

{{< note >}}

To generate the ECDSA root CA and intermediate CA for signing the device manufacturer certificate (IDevId), you have the option to use tools such as OpenSSL or the plgd cert-tool.

To install the cert-tool, execute the following command in your shell:

```sh
go install github.com/plgd-dev/hub/v2/tools/cert-tool@latest
```

Similarly, to install the yq tool for updating YAML files, use the following command:

```sh
go install github.com/mikefarah/yq/v4@latest
```

Finally, ensure that the go bin directory is included in your PATH by executing the following command:

```sh
export PATH=$PATH:$(go env GOPATH)/bin
```

These steps will enable you to generate the necessary certificates and configure your environment appropriately.

{{< /note >}}

### Generate certificates

1. Generate the ECDSA root CA with a validity of 100 years:

   ```sh
   umask 0077
   mkdir -p "$HOME/plgd_certs"
   cd "$HOME/plgd_certs"
   ~/go/bin/cert-tool --cmd.generateRootCA --outCert=./root_ca.crt --outKey=./root_ca.key --cert.subject.cn=RootCA --cert.validFrom=2023-01-01T12:00:00Z --cert.validFor=876000h
   ```

2. Generate the ECDSA intermediate CA signed by the root CA with a validity of 100 years:

   ```sh
   umask 0077
   cd "$HOME/plgd_certs"
   ~/go/bin/cert-tool --cmd.generateIntermediateCA --signerCert=./root_ca.crt --signerKey=./root_ca.key --outCert=./intermediate_ca.crt --outKey=./intermediate_ca.key --cert.basicConstraints.maxPathLen=0 --cert.subject.cn="IntermediateCA" --cert.validFrom=2023-01-01T12:00:00Z --cert.validFor=876000h
   ```

   {{< note >}}

   Set `maxPathLen` to `0` because the intermediate CA will only sign leaf certificates like the device manufacturer certificate (IDevId).

   {{< /note >}}

3. Generate the device manufacturer certificate (IDevId) signed by the intermediate CA with a validity of 100 years and store it in the `mfgcrt.pem` and `mfgkey.pem` files:

   ```sh
   umask 0077
   mkdir -p "$HOME/plgd_certs/device/pki_certs"
   cd "$HOME/plgd_certs"
   ~/go/bin/cert-tool --cmd.generateCertificate --signerCert=./intermediate_ca.crt --signerKey=./intermediate_ca.key --outCert=./device/pki_certs/mfgcrt.pem --outKey=./device/pki_certs/mfgkey.pem --cert.subject.cn="IDevId Device01"
   ```

4. Set the content of `intermediate_ca.crt` to the `deviceProvisioningService.enrollmentGroups[].attestationMechanism.x509.certificateChain` field using the [yq](https://github.com/mikefarah/yq) tool:

   ```sh
   cd "$HOME"
   cat ./withMock.yaml | yq -e ".deviceProvisioningService.enrollmentGroups[0].attestationMechanism.x509.certificateChain=\"$(cat ./plgd_certs/intermediate_ca.crt)\"" > ./withUpdatedMock.yaml
   helm upgrade -i -n plgd --create-namespace -f withUpdatedMock.yaml hub plgd/plgd-hub
   kubectl -n plgd delete $(kubectl -n plgd get pods -o name | grep "hub-plgd")
   ```

Now, you can test the Device Provisioning Service with the following methods depending on the network trust level:

### Onboarding device in Zero trust network

To set up a Zero Trust network, it is essential for the device to authenticate the Device Provisioning Service certificate. Therefore, you must acquire the root certificate authority responsible for signing the device provisioning service and save it as the dpsca.pem file specifically for the example device.

1. Obtain the root certificate authority that signs the device provisioning service and store it in the `dpsca.pem` file:

   ```sh
   kubectl -n plgd get secret plgd-ca -o 'go-template={{index .data "ca.crt"}}' | base64 -d > "$HOME/plgd_certs/device/pki_certs/dpsca.pem"
   ```

2. Run the example device with the device manufacturer certificate (IDevId):

   ```sh
   docker run -it --rm -v $HOME/plgd_certs/device/pki_certs:/dps/pki_certs ghcr.io/iotivity/iotivity-lite/dps-cloud-server-debug:latest test-device "coaps+tcp://example.com:15684"
   ```

{{< note >}}

In a real-world scenario, as a device manufacturer, it is crucial to ship the device to customers with the RootCA (Root Certificate Authority) of the Device Provisioning Service pre-installed during manufacturing process. This ensures that the device can trust the certificates issued by the Device Provisioning Service. The RootCA should be securely embedded in the device's firmware and can only be modified through firmware updates.

Additionally, as the manufacturer, you provide the customer with an Intermediate CA (Intermediate Certificate Authority) or offer a service that allows them to obtain a signed certificate for their specific device provisioning service. This Intermediate CA enables the customer to sign certificates for their devices, ensuring secure communication within their network.

{{< /note >}}

### Onboarding device in Trusted network

In Trusted network device can skip validation of the Device Provisioning Service certificate. In this mode device doesn't need to obtain the root certificate authority that signs the device provisioning service.

{{< note >}}

   If file `$HOME/plgd_certs/device/pki_certs/dpsca.pem` exists, you can remove it with the following command:

   ```sh
   rm "$HOME/plgd_certs/device/pki_certs/dpsca.pem"
   ```

{{< /note >}}

1. Run the example device with the device manufacturer certificate (IDevId):

   ```sh
   docker run -it --rm -v $HOME/plgd_certs/device/pki_certs:/dps/pki_certs ghcr.io/iotivity/iotivity-lite/dps-cloud-server-debug:latest test-device "coaps+tcp://example.com:15684" --no-verify-ca
   ```

{{< warning >}}

In a specific use-case, there might be a requirement to allow onboarding of devices using any vendor's Device Provisioning Service without validating the Device Provisioning Service certificate. However, it is important to note that this approach poses a significant security risk in a real-world scenario. It is essential to exercise caution and consider the potential consequences of bypassing certificate validation, as it could potentially compromise the security and integrity of the network.

{{< /warning >}}
