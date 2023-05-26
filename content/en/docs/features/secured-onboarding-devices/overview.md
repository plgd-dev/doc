---
title: 'Secured Onboarding of Devices'
description: 'What are the Secured Onboarding of Devices features of plgd system?'
date: '2023-05-23'
categories: [zero-touch, provisioning. features]
keywords: [provisioning, TPM, device-twin]
weight: 1
---

The Secured Onboarding of Devices feature in the plgd system ensures a secure and reliable procedure for incorporating devices into the system. It encompasses various tasks such as managing device identity certificates, integrating with root certification authorities, and generating per device chain of trust certificates. The system relies on X.509 certificates for device identity and authentication purposes, following the [OCF specifications](https://openconnectivity.org/developer/specifications/).

## Device Identity Certificate Management

The Device Identity Certificate Management component handles the generation and provisioning of per device chain of trust certificates. It ensures that each device has a unique and trusted certificate for secure communication within the system.

## TPM Integration

The TPM Integration feature facilitates the integration of Trusted Platform Modules (TPMs) into the devices. TPMs are microcontroller chips that provide hardware-based security for computer systems. This feature validates and verifies TPMs, enhancing the overall security of the devices and the system.

[Learn more about TPM Integration](/docs/features/secured-onboarding-devices/tpm)

## Certificate Authority Integration

The Certificate Authority Integration feature enables the integration of certificate authorities (CAs) into the system. It ensures the validation and verification of certificates issued by trusted CAs, further enhancing the security and trustworthiness of the system.

## Automatic Device Provisioning

The Automatic Device Provisioning feature enables devices to be automatically provisioned into the system. It simplifies and accelerates the onboarding process by reducing manual intervention, saving time and effort. The Device Provisioning Service (DPS) is an excellent choice for various provisioning scenarios to connect and configure devices to the IoT Hub.

[Learn more about Device Provisioning Service](/docs/device-provisioning-service/)
