---
title: 'Feature list'
description: 'What features provides Certificate Authority?'
date: '2023-06-14'
categories: [security, certificate]
keywords: [security, identity-certificate, certificate, features]
weight: 2
---

### Implemented features

- **&#x2611; OCF Identity Certificate Issuance:** The PLGD Certificate Authority is capable of issuing OCF identity certificates for users and devices by signing Certificate Signing Requests (CSRs).
- **&#x2611; JWT Verification:** The PLGD CA verifies the validity of JSON Web Tokens (JWTs) provided with the CSR to ensure the authenticity of the request.
- **&#x2611; Protection Against Unauthorized Identity Certificate Issuance:** The PLGD CA has stringent policies in place to prevent devices or users with malicious intent from obtaining identity certificates that have already been signed for other devices.
- **&#x2611; HTTP and gRPC API:** The PLGD CA provides both an [HTTP](/docs/services/certificate-authority/http-api/) and [gRPC](/docs/services/certificate-authority/grpc-api/) API, allowing users to interact with the CA and perform certificate-related operations.
- **&#x2611; Automatic Deletion of Expired Certificates:** Non-identity certificates are [automatically deleted](/docs/services/certificate-authority/deletion-certificates) by the PLGD CA once they expire, ensuring a secure and clutter-free certificate infrastructure.

### Planned features

- **&#x2610; Profiles:** The PLGD CA will introduce the concept of profiles for identity certificates. These profiles will be based on the owner's information from the JSON Web Token (JWT) and the type of certificate (device or user). With profiles, different configurations and settings can be applied to each type of certificate, allowing for more customization and flexibility.
- **&#x2610; Certificate Revocation:** The PLGD CA will implement a certificate revocation mechanism. This will involve maintaining a Certificate Revocation List (CRL) that keeps track of revoked certificates. The CA will regularly check the validity of certificates against the CRL to ensure that any compromised or invalid certificates are promptly revoked and no longer considered valid.
