---
title: 'Attestation Mechanisms'
description: 'How is the device identity confirmed?'
date: '2022-02-15'
categories: [zero-touch, provisioning]
keywords: [attestation, x509, TPM, manufacturer-certificate]
weight: 3
---


Most important part of the zero-touch provisioning is the attestation mechanism, a method used to confirm a device's identity. Each enrollment group needs to have the attestation mechanism configured. Selected method and it's configuration is used to identify to which enrollment group the device opening the connection to the DPS belongs to.
The Device Provisioning Service supports 2 forms of attestations:

- Manufacturer certificates based on the standard X.509 certificate authentication flow
- Trusted Platform Module (TPM) based on a nonce challenge

## X.509 Certificates

Digital certificates allow individuals, organizations, as well as devices to establish trust in the digital world. As the foundation for all digital identities, X.509 certificates are everywhere and are essential to every connected process from websites to applications to devices and online documents. This level of trust is established both by how X.509 certificates work and by how they are issued. The key usage architecture lets certificates verify that:

- A public key belongs to the hostname/domain, organization, or individual contained within the certificate
- It has been signed by a trusted issuer Certificate Authority (CA)

When a certificate is signed by a trusted CA, the certificate user can be confident that the certificate owner has been validated.

By using X.509 certificate as an attestation mechanism, a device's manufacturer certificate is used to verify it's identity. This certificate is typically arranged in a certificate chain of trust in which each certificate in the chain is signed by the private key of the next higher certificate. This arrangement establishes a delegated chain of trust from the CA certificate down through each intermediate CA to the manufacturer, end-identity leaf certificate installed on the device.

The way how the certificate chain of trust is built is usually reflected by the physical or logical hierarchy of your devices / products. For example, a device manufacturer may:

- Issue a single self-signed root CA certificate
- Use the root CA certificate to sign per factory issued intermediate CA
- Use the factory's intermediate CA to sign per production line issued intermediate CA
- Use the production line intermediate CA to sign the end-identity manufacturer certificate installed on the device

{{< note >}}
To govern the issuance of digital certificates in a scalable way, public key infrastructure (PKI) is required.
{{< /note >}}

### Manufacturer certificate

The manufacturer certificate is the leaf certificate signed by the manufacturer's root / intermediate CA. It uniquely identifies the device. This certificate is used only during the provisioning process to verify that the device was manufactured by that particular company. The device connecting to the DPS needs to authenticate using this client certificate during what the DPS verifies it's the public key against the root CA / intermediate CA registered in the matching enrollment group. The device must present the leaf certificate or the certificate chain from leaf certificate all the way to the certificate verified with proof-of-possession, otherwise authentication will fail. Successful verification of the public key results in successfully connected device, developed by the declared manufacturer, eligible for the provisioning. Verification of the DPS server certificate by the device is configurable. To learn more, see [DPS Client API](/docs/services/device-provisioning-service/client-library/#dps-client-api). Successfully provisioned device switches from using Manufacturer certificate to Identity certificates, both for communication with the plgd hub / other devices in the network or when [connecting to the DPS service again]().

{{< note >}}
To uniquely identify each device it's recommended to set the certificate common name (CN) to a unique product identifier, e.g. serial number. This allows you to whitelist or blacklist devices, what's configurable on the enrollment group level. To know more about this feature, see [Whitelisting / BlackListing](/docs/services/device-provisioning-service/whitelisting-blacklisting).
{{< /note >}}

### Certificate chain of trust

A certificate chain is a list of certificates followed by one or more CA certificates. In RFC 5280 the certificate chain or chain of trust is defined as “certification path”. In other words, the chain of trust refers to your certificate and how it is linked back to a trusted Certificate Authority. In order for a certificate to be trusted it has to be traceable back to the trust root it was signed off, meaning all certificates in the chain—server, intermediate, and root, need to be properly trusted. There are three parts to the chain of trust:

- Root Certificate, a digital certificate that belongs to the issuing Certificate Authority. The root certificates are closely guarded by CAs.
- Intermediate Certificate, branch off root certificates like branches of trees. They act as middle-men between the protected root certificates and the manufacturer certificates issued for your devices.
- Manufacturer certificate (end-entity certificate) is the one issued to the specific device the system is needing coverage for.

Certificate chains are used in order to check that the public key and other data contained in an end-entity certificate (the first certificate in the chain) effectively belong to its subject. In order to ascertain this, the signature on the end-target certificate is verified by using the public key contained in the following certificate, whose signature is verified using the next certificate, and so on **until the last certificate in the chain is reached**. As the last certificate is a trust anchor, successfully reaching it will prove that the end-entity certificate can be trusted.

### Certificate chain authentication scenarios

Part of the enrollment group configuration is the certificate chain, used for both authentication and enrollment group selection. The device authenticating against the DPS is required to provide the certificate chain from the end-entity certificate up to a the leaf-most certificate present in the certificate chain configured on the enrollment group. Otherwise the authentication will fail.

For example, consider a manufacturer uses the following certificate chain:
![](/docs/services/device-provisioning-service/static/mfg-certificate-chain.drawio.svg)

If the device sends the following **manufacturer certificate chain**:
![](/docs/services/device-provisioning-service/static/mfg-certificate-intermediate-3.drawio.svg)

... while the certificate chain **configured on the enrollment group** looks like:
![](/docs/services/device-provisioning-service/static/enrollment-group-intermediate-1.drawio.svg)

authentication fails because the DPS can't attempt authentication assuming the validity of the `Intermediate CA #2`.

{{< note >}}
If the device authenticates using the manufacturer certificate chain containing certificates up to `Intermediate CA #2`, or the certificate chain configured on the enrollment group would contain certificates up to `Intermediate CA #2`, the authentication would be successful.
{{< /note >}}

### Enrollment group prioritization

The DPS supports creation of [multiple enrollment groups]() what allows the operator to distribute devices based on the Intermediate CAs. When configuring the certificate chain on the enrollment group, an operator has a possibility to select a certificate from the chain to represent the enrollment group during the search. Selection of the enrollment group is done after successful device authentication, after which the full verified certificate chain is assembled. Search for the matching enrollment group starts by taking the leaf-most intermediate certificate from the verified certificate chain and finding the match across list of certificates representing enrollment groups. If no match was found, the next parent authority certificate from the verified certificate chain is taken and search is repeated. This process is repeated until last root CA certificate present in the chain is reached. If there wasn't any match across enrollment groups, the device is disconnected.

For better understanding, consider 2 enrollment groups configured on the DPS *(certificate with yellow border represents the enrollment group)*:

**Enrollment Group #A**
![](/docs/services/device-provisioning-service/static/enrollment-group-intermediate-sel-3.drawio.svg)

**Enrollment Group #B**
![](/docs/services/device-provisioning-service/static/enrollment-group-intermediate-sel-2.drawio.svg)

{{< note >}}
The device after it authenticates using the following manufacturer certificate chain:
![](/docs/services/device-provisioning-service/static/mfg-certificate-intermediate-3.drawio.svg)
will be provisioned as configured in the **Enrollment Group #A**.
{{< /note >}}

{{< note >}}
The device after it authenticates using the following manufacturer certificate chain:
![](/docs/services/device-provisioning-service/static/mfg-certificate-intermediate-3.1.drawio.svg)
will be provisioned as configured in the **Enrollment Group #B**.
{{< /note >}}

{{< note >}}
The device after it authenticates using the following manufacturer certificate chain:
![](/docs/services/device-provisioning-service/static/mfg-certificate-leaf-3.1.drawio.svg)
will be disconnected as **no matching enrollment group was found**.
{{< /note >}}
