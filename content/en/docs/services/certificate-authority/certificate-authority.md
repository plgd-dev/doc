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

### User OCF Identity Certificate CSR

A CSR is considered a user identity CSR if the Common Name (CN) matches the "owner" claim of the provided JSON Web Token (JWT) or a derived value from it. The PLGD CA ensures the validity of the JWT before proceeding with the certificate signing process. Multiple user identity certificates can be obtained simultaneously for different key-pairs.

### Device OCF Identity Certificate CSR

If the CSR's Common Name (CN) does not meet the requirements for a user identity CSR, it is treated as a device identity CSR. The PLGD CA has strict policies in place to prevent unauthorized devices or users from obtaining identity certificates that have already been signed for other devices.

## Identity Certificate Management

The PLGD CA includes comprehensive mechanisms for managing the lifecycle and security of identity certificates. This encompasses the issuance, renewal, and deletion of identity certificates. For detailed information on how to manage identity certificates, please refer to the [Identity Certificate Management Documentation](/docs/services/certificate-authority/identity-certificate-management).
