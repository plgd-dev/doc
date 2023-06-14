---
title: 'Certificate Authority'
description: 'What is plgd Certificate Authority?'
date: '2022-01-26'
categories: [security, certificate]
keywords: [security, identity-certificate, certificate]
weight: 1
---

The PLGD Certificate Authority (CA) is a component that provides OCF (Open Connectivity Foundation) identity certificates for users and devices. These certificates are issued by signing Certificate Signing Requests (CSRs). The PLGD CA enforces certain rules and processes to ensure the security and integrity of the certificate issuance.

## Certificate Signing Request (CSR)

A Certificate Signing Request (CSR) is a request sent by a user or device to the PLGD CA, requesting the issuance of an identity certificate. The CSR contains a UUID (Universally Unique Identifier) in the Common Name (CN) field, following the format `uuid:<ID>`. The PLGD CA verifies the authenticity of the request before signing the CSR and issuing the OCF identity certificate.

### User Identity Certificate CSR

If the CSR's Common Name (CN) contains the same as value or derived value from the `owner` claim  of the JWT (JSON Web Token) provided with the request, or a derived value from it, the CSR is considered a user identity CSR. The PLGD CA verifies the validity of the JWT before proceeding with the certificate signing process. The User Identity Certificate CSR could be obtained for various key-pairs in same time without any limitation.

### Device Identity Certificate CSR

If the CSR's Common Name (CN) is not considered User Identity Certificate CSR then it is treated as a device identity CSR. The PLGD CA follows a strict policy to prevent devices or users with malicious intent from obtaining identity certificates that have already been signed for different devices. For t

## Identity Certificate Management

The PLGD CA includes mechanisms to manage the lifecycle of identity certificates and ensure their security.

## GRPC API

The PLGD CA provides a gRPC API that can be used to request the issuance of identity certificates. The API also provides methods for managing the lifecycle of identity certificates.

[Link to GRPC API Documentation](https://htmlpreview.github.io/?https://raw.githubusercontent.com/plgd-dev/hub/jkralik/fix/ca-docs/certificate-authority/pb/doc.html#certificateauthority.pb.CertificateAuthority)

## HTTP API

The PLGD CA provides an HTTP API that can be used to request the issuance of identity certificates. The API also provides methods for managing the lifecycle of identity certificates.

[Link to HTTP API Documentation](https://petstore.swagger.io/?url=https://raw.githubusercontent.com/plgd-dev/hub/main/certificate-authority/pb/service.swagger.json)

### Deletion of Identity Certificates

Identity certificates can be deleted using various methods:

1. **Device Signoff**: A device can initiate the deletion of its own identity certificate by sending a signoff request to the PLGD CA.

2. **Device Deletion from Hub**: If a device is deleted from the hub using the grpc-gw (gRPC Gateway) interface, the associated identity certificate records are also deleted.

3. **Certificate Authority API**: The PLGD CA provides an API ([HTTP](#http-api)/[gRPC](#grpc-api)) that can be used to request the deletion of identity certificates.

### Automatic Deletion of Other Certificates

All other certificates, apart from identity certificates, are automatically deleted once they are no longer valid. This automatic deletion process ensures that expired certificates do not clutter the system and helps maintain a clean and secure certificate infrastructure.

For more information and detailed usage instructions, please refer to the PLGD CA documentation or contact the PLGD support team.
