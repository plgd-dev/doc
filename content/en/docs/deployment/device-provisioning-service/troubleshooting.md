---
title: 'Troubleshooting'
description: 'How to troubleshooting issues with Device Provisioning Service'
docsOthersDisplay: true
date: '2023-05-18'
categories: [deployment, kubernetes]
keywords: [deployment, kubernetes, k8s, helm, chart]
weight: 30
---

If you encounter issues with the Device Provisioning Service or Hub, follow these steps to enable debug logs in the configuration:

1. Update the YAML configuration file with the following content:

   ```yaml
   coapgateway:
     log:
       dumpBody: true
       level: debug
   deviceProvisioningService:
     log:
       dumpBody: true
       level: debug
   ```

2. Apply the helm configuration by running the following commands:

   ```sh
   helm upgrade -i -n plgd --create-namespace -f ./withUpdatedMock.yaml hub plgd/plgd-hub
   helm upgrade -i -n plgd --create-namespace -f ./withUpdatedMock.yaml dps plgd/plgd-dps
   ```

3. Restart the pods by deleting them:

   ```sh
   kubectl -n plgd delete $(kubectl -n plgd get pods -o name | grep "hub-plgd")
   kubectl -n plgd delete $(kubectl -n plgd get pods -o name | grep "dps-plgd")
   ```

These steps will enable debug logging and restart the necessary components, providing more detailed information for troubleshooting the issues with the Device Provisioning Service or Hub.

### Issue: Device unable to connect to DPS

If your device is unable to connect to DPS, follow these steps:

1. Check the DPS logs by running the following command:

   ```sh
   kubectl -n plgd logs $(kubectl -n plgd get pods -o name | grep "dps-plgd")
   ```

2. Check the device logs in the console.

   In most cases, the issue lies in the certificate chain or resolving the domain name. If it's a certificate chain problem, do the following:

   - Verify that the certificate chain is in the correct order, with the root CA as the last certificate in the chain.
   - Ensure that the certificate chain is set in the `deviceProvisioningService.enrollmentGroups[].attestationMechanism.x509.certificateChain` field.
   - Confirm that the device certificates are stored in the correct files: `$HOME/plgd_certs/device/pki_certs/mfgkey.pem`, `$HOME/plgd_certs/device/pki_certs/mfgkey.pem`, `$HOME/plgd_certs/device/pki_certs/dpsca.pem`.

### Issue: Device unable to connect to Hub

If your device is unable to connect to the Hub, follow these steps:

1. Check the Hub logs by running the following command:

   ```sh
   kubectl -n plgd logs $(kubectl -n plgd get pods -o name | grep "coap-gateway")
   ```

2. Check the device logs in the console.

   In most cases, the issue is related to resolving the domain name or a misconfigured provider. If it's a provider configuration problem, do the following:

   - Verify that the provider configuration is set in the `deviceProvisioningService.enrollmentGroups[].hub.authorization.provider` field.
   - Compare the provider configuration in DPS with the coap-gateway provider configuration by running the following command:

   ```sh
   kubectl -n plgd get $(kubectl -n plgd get configmap -o name | grep "coap-gateway") -o yaml | yq '.data["service.yaml"]' | yq '.apis.coap.authorization.providers'
   ```

### Device can connect to DPS but unable to retrieve certificates or authorization code

If your device can connect to the DPS service but is unable to retrieve certificates from the certificate authority or obtain an authorization code due to lack of trust, follow these steps:

- For the certificate authority, you need to append the certificate authority for that endpoint to the `global.extraCAPool.authorization` and set `deviceProvisioningService.enrollmentGroups[].hub.certificateAuthority.grpc.tls.caPool` to `/certs/extra/ca.crt` as described in the [Customize client certificates for DPS](/docs/deployment/device-provisioning-service/advanced#customize-client-certificates-for-dps) section. Alternatively, you can create an extra volume, mount it, and set the `deviceProvisioningService.enrollmentGroups[].hub.certificateAuthority.grpc.tls.caPool` field to the CA in that volume.
- For the authorization provider, follow similar steps as for the certificate authority, but set `enrollmentGroups.[].hub.authorization.provider.http.tls.caPool`.
