---
title: 'Features'
description: 'What features provides Device Provisioning Service?'
date: '2023-05-19'
categories: [zero-touch, provisioning]
keywords: [provisioning, TPM, manufacturer-certificate]
weight: 2
---

### Implemented features

- **&#x2611; Device attestation via certificates:** This feature allows devices to prove their identity using [certificates](/docs/services/device-provisioning-service/attestation-mechanism#x509-certificates).
- **&#x2611; Onboarding devices to plgd hub:** This feature enables the process of adding devices for management and control by [plgd hub](/docs/architecture/component-overview).
- **&#x2611; Control the provisioned device from local network:** With this feature, users can access the provisioned devices directly from their [local network](/docs/device-to-device-client/overview), enabling them to perform maintenance tasks and manage the devices conveniently.
- **&#x2611; Synchronize device time with Device Provisioning Service:** This feature ensures that devices have [synchronized time](/docs/services/device-provisioning-service/client-library.md#time-synchronization) with the Device Provisioning Service.
- **&#x2611; Rotate device certificates:** This feature allows for the [rotation of device certificates](/docs/services/device-provisioning-service/client-library#certificate-renewal), enhancing security and maintaining up-to-date certificates.
- **&#x2611; HTTP API to configure access devices:** This feature provides an HTTP API that allows users to configure the enrollment groups.
- **&#x2611; HTTP API to get provisioned configuration to the devices:** This feature offers an HTTP API to retrieve provisioned configurations of the devices.

### Planned features

- **&#x2610; Device attestation via TPM:** This upcoming feature will enable devices to attest their identity using Trusted Platform Modules (TPM).
- **&#x2610; Initial device configuration:** This planned feature aims to provide users with a mechanism to perform the initial configuration of devices during the provisioning process.
- **&#x2610; Blacklisting and whitelisting devices:** With this feature, users will have the ability to blacklist or whitelist specific devices for enhanced access control.
- **&#x2610; Manual approval for device configuration:** This planned feature enables a mechanism where devices require manual approval from the user in order to receive configuration settings. Instead of automatic configuration, users will have control over granting permission for device configuration.
