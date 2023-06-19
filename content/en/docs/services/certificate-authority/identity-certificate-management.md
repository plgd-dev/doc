---
title: 'Identity Certificate Management'
description: 'How to manage certificates?'
date: '2023-06-14'
categories: [certificate]
keywords: [identity-certificate, certificate, features]
weight: 30
---

Identity Certificate Management in the PLGD Certificate Authority (CA) involves the processes and mechanisms for effectively managing the lifecycle and security of identity certificates issued to users and devices.

### Issuance of Identity Certificates

The PLGD CA is responsible for issuing OCF (Open Connectivity Foundation) identity certificates to users and devices. These certificates are obtained by signing Certificate Signing Requests (CSRs) submitted by the entities. The CA verifies the authenticity of the CSR and, if validated, issues the corresponding identity certificate.

### Renewal and Expiration

Identity certificates have a validity period, after which they expire. The PLGD CA automatically deletes non-device-identity certificates once they become invalid. This process ensures that expired certificates do not clutter the system and helps maintain a secure certificate infrastructure.

### Deletion of Identity Certificates

Identity certificates can be deleted using various methods:

- **Device Signoff**: Devices can initiate the deletion of their own identity certificates by sending signoff requests to the PLGD CA.
- **Device Deletion from Hub**: When a device is deleted from the hub using the appropriate interface [gRPC Gateway API](/docs/features/control-plane/grpc-api) or [HTTP Gateway API](/docs/features/control-plane/http-api), the associated identity certificate records are also deleted.
- **Certificate Authority API**: The PLGD CA provides both an [gRPC API](/docs/services/certificate-authority/grpc-api) and a [HTTP API](/docs/services/certificate-authority/http-api)  that allow requesting the deletion of identity certificates.

These deletion mechanisms provide flexibility and control over the management of identity certificates, ensuring that certificates are removed as needed to maintain security and integrity.

{{< note >}}

For more detailed information and usage instructions, please contact us at [connect@plgd.dev](mailto:connect@plgd.dev).

{{< /note >}}
