---
title: 'Deletion of Identity Certificates'
description: 'How to delete certificates from Certificate Authorities?'
date: '2023-06-14'
categories: [security, certificate]
keywords: [security, identity-certificate, certificate, features]
weight: 30
---

Identity certificates can be deleted using the following methods:

1. **Device Signoff**: Devices can initiate the deletion of their own identity certificates by sending signoff requests to the PLGD Hub via the CoAP Gateway.

2. **Device Deletion from Hub**: When a device is deleted from the hub using the [gRPC Gateway API](/docs/features/control-plane/grpc-api) or [HTTP Gateway API](/docs/features/control-plane/http-api), the associated identity certificate records are also deleted.

3. **Certificate Authority API**: The PLGD CA provides the [HTTP API](/docs/services/certificate-authority/http-api) and [gRPC API](/docs/services/certificate-authority/grpc-api) that allow requesting the deletion of certificates.

### Automatic Deletion of Other Certificates

Certificates other than OCF device identity certificates are automatically deleted once they expire. This automated process ensures a clean and secure certificate infrastructure by removing expired certificates from the system.

{{< note >}}

For more detailed information and usage instructions, please contact us at [connect@plgd.dev](mailto:connect@plgd.dev).

{{< /note >}}
