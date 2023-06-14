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
- **&#x2611; Automatic Deletion of Expired Certificates:** Non-identity certificates are automatically deleted by the PLGD CA once they expire, ensuring a secure and clutter-free certificate infrastructure.

### Planned features

- **&#x2610; Profiles:** The PLGD CA will introduce the concept of profiles for identity certificates based on the owner's information from the JWT and the type of certificate (device or user). This will allow for different configurations and settings for each type of certificate.
