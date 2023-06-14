---
title: 'Certificate Authority'
description: 'What is plgd Certificate Authority?'
date: '2023-06-14'
categories: [security, certificate]
keywords: [security, identity-certificate, certificate]
weight: 1
isMainSectionPage: true
---

The PLGD Certificate Authority (CA) is responsible for issuing OCF (Open Connectivity Foundation) identity certificates to users and devices. These certificates are obtained by signing Certificate Signing Requests (CSRs) and are subject to strict security measures.

## Certificate Signing Request (CSR)

A Certificate Signing Request (CSR) is a request sent to the PLGD CA by a user or device, seeking the issuance of an identity certificate. The CSR includes a Universally Unique Identifier (UUID) in the Common Name (CN) field, following the format `uuid:<ID>`. Before signing the CSR and issuing the OCF identity certificate, the PLGD CA verifies the authenticity of the request.

### User Identity Certificate CSR

A CSR is considered a user identity CSR if the Common Name (CN) matches the "owner" claim of the provided JSON Web Token (JWT) or a derived value from it. The PLGD CA ensures the validity of the JWT before proceeding with the certificate signing process. Multiple user identity certificates can be obtained simultaneously for different key-pairs.

### Device Identity Certificate CSR

If the CSR's Common Name (CN) does not meet the requirements for a user identity CSR, it is treated as a device identity CSR. The PLGD CA has strict policies in place to prevent unauthorized devices or users from obtaining identity certificates that have already been signed for other devices.

## Identity Certificate Management

The PLGD CA includes comprehensive mechanisms for managing the lifecycle and security of identity certificates.

## gRPC API

The PLGD CA provides a gRPC API that enables requesting the issuance of identity certificates and managing their lifecycle.

[Link to gRPC API Documentation](https://htmlpreview.github.io/?https://raw.githubusercontent.com/plgd-dev/hub/main/certificate-authority/pb/doc.html#certificateauthority.pb.CertificateAuthority)

## HTTP API

The PLGD CA also offers an HTTP API for requesting the issuance of identity certificates and managing their lifecycle.

[Link to HTTP API Documentation](https://petstore.swagger.io/?url=https://raw.githubusercontent.com/plgd-dev/hub/main/certificate-authority/pb/service.swagger.json)

### Deletion of Identity Certificates

Identity certificates can be deleted using the following methods:

1. **Device Signoff**: Devices can initiate the deletion of their own identity certificates by sending signoff requests to the PLGD CA.

2. **Device Deletion from Hub**: When a device is deleted from the hub through the grpc-gw (gRPC Gateway) interface, the associated identity certificate records are also deleted.

3. **Certificate Authority API**: The PLGD CA provides an API ([HTTP](#http-api)/[gRPC)](#grpc-api)) that allows requesting the deletion of identity certificates.

### Automatic Deletion of Other Certificates

Certificates other than identity certificates are automatically deleted once they expire. This automated process ensures a clean and secure certificate infrastructure by removing expired certificates from the system.

{{< note >}}

For more detailed information and usage instructions, please contact us at [connect@plgd.dev](mailto:connect@plgd.dev).

{{< /note >}}
