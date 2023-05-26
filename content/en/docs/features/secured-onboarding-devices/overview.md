---
title: 'Secured Onboarding of Devices'
description: 'What are the Secured Onboarding of Devices features of plgd system?'
date: '2023-05-23'
categories: [zero-touch, provisioning. features]
keywords: [provisioning, TPM, device-twin]
weight: 1
---

The Secured Onboarding of Devices feature in the plgd system ensures a secure and reliable procedure for incorporating devices into the system. It encompasses various tasks such as managing device identity certificates, integrating with root certification authorities, and generating per device chain of trust certificates. The system relies on X.509 certificates for device identity and authentication purposes, following the OCF specifications.

## Device Identity Certificate Management

The Device Identity Certificate Management component handles the generation and provisioning of per device chain of trust certificates. It ensures that each device has a unique and trusted certificate for secure communication within the system.

### Per Device Chain of Trust Certificate Generation & Provisioning

This feature generates and provisions chain of trust certificates based on certificate signing requests (CSRs) from each device. It guarantees that every device has a unique and trusted certificate, enabling secure communication within the system.

### Root Certification Integration

The Root Certification Integration feature seamlessly integrates root certificates into the system. Root certificates serve as the foundation of trust within the certificate hierarchy, enhancing the overall security of the system.

## File-based Root Certificate

The File-based Root Certificate feature enables the system to use a file-based approach for managing and storing root certificates. This ensures secure and reliable storage of the certificates, contributing to the overall security of the system.

## TPM Integration

The TPM Integration feature facilitates the integration of Trusted Platform Modules (TPMs) into the devices. TPMs are microcontroller chips that provide hardware-based security for computer systems. This feature validates and verifies TPMs, enhancing the overall security of the devices and the system.

[Learn more about TPM Integration](../tpm)

## Certificate Authority Integration

The Certificate Authority Integration feature enables the integration of certificate authorities (CAs) into the system. It ensures the validation and verification of certificates issued by trusted CAs, further enhancing the security and trustworthiness of the system.

### Identity Certificate Rotation

The Identity Certificate Rotation component handles the periodic rotation of identity certificates for enhanced security. This ensures that devices regularly receive updated and renewed certificates, minimizing the risk of compromised security.

## Zero Trust Environment

In a Zero Trust Environment, the Device Provisioning Service Certificate/Hub is available at the device. This means that devices undergo validation and verification processes to establish trust before being granted access to the system. The Zero Trust approach ensures a high level of security by validating each device's identity and authorization.

## Trusted Environment

In a Trusted Environment, there is no Device Provisioning Service Certificate required for device validation. Devices in a trusted environment can skip the validation process at the device side, streamlining the onboarding process. This feature is useful in scenarios where devices are already known and trusted within the system.

## Automatic Device Provisioning

The Automatic Device Provisioning feature enables devices to be automatically provisioned into the system. It simplifies and accelerates the onboarding process by reducing manual intervention, saving time and effort. The Device Provisioning Service (DPS) is an excellent choice for various provisioning scenarios to connect and configure devices to the IoT Hub. 

[Learn more about Automatic Device Provisioning](../provisioning)

