---
title: 'Overview'
description: 'What are the features of plgd system?'
date: '2023-05-23'
categories: [zero-touch, provisioning. features]
keywords: [provisioning, TPM, device-twin]
weight: 1
---

## Secured Onboarding of Devices

The plgd system incorporates a Secured Onboarding of Devices feature that guarantees a secure and dependable procedure for incorporating devices into the system. This functionality encompasses various tasks such as overseeing device identity certificates, integrating with root certification authorities, and generating and providing per device chain of trust certificates. In line with the [OCF specifications](https://openconnectivity.org/developer/specifications/), the system relies on X.509 certificates for device identity and authentication purposes.

### Device Identity Certificate Management

The Device Identity Certificate Management component handles the generation and provisioning of per device chain of trust certificates. It ensures that each device has a unique and trusted certificate for secure communication within the system.

#### Per Device Chain of Trust Certificate Generation & Provisioning

System for certificate signing request (CSR) from each device generates and provisions chain of trust certificates. This ensures that each device has a unique and trusted certificate for secure communication within the system.

#### Root Certification Integration

The Root Certification Integration feature enables the seamless integration of root certificates into the system. Root certificates are the foundation of trust within the certificate hierarchy.

### File-based Root Certificate

The File-based Root Certificate feature allows the system to use a file-based approach for managing and storing root certificates. This ensures secure and reliable storage of the certificates.

### TPM integration

The TPM Integration feature facilitates the integration of Trusted Platform Modules (TPMs) into the devices. It enables the validation and verification of TPMs for enhanced security.

### Certificate Authority Integration

The Certificate Authority Integration feature facilitates the integration of certificate authorities (CAs) into the system. It enables the validation and verification of certificates issued by trusted CAs.

#### Identity Certificate Rotation

The Identity Certificate Rotation component handles the periodic rotation of identity certificates for enhanced security. This ensures that devices regularly receive updated and renewed certificates.

### Zero Trust Environment

In a Zero Trust Environment, the Device Provisioning Service Certificate / Hub is available at the device. This means that devices undergo validation and verification processes to establish trust before being granted access to the system.

### Trusted Environment

In a Trusted Environment, there is no Device Provisioning Service Certificate required for device validation. Devices in a trusted environment can skip the validation process at the device side, streamlining the onboarding process.

### Automatic Device Provisioning

The Automatic Device Provisioning feature enables devices to be automatically provisioned into the system. This simplifies and accelerates the onboarding process, reducing manual intervention.

#### Time Service

The Time Service feature automatically sets the device time during onboarding and device connection reestablishment. This ensures accurate and synchronized timekeeping across devices, improving system functionality.

#### Configure access from local network

The Configure access from local network feature enables users to access devices directly from their local network. This allows them to perform maintenance tasks and manage the devices conveniently.

## Managing Devices

The Managing Devices feature enables users to manage the devices in the system. This includes the ability to perform maintenance tasks, manage device metadata, and control the devices from the cloud or local network.

### Device Twin

The Device Twin feature enables the synchronization of device state with the cloud. This ensures that the device state is always up-to-date, improving system functionality.

### Control plane

The Control Plane feature enables users to control the provisioned devices. This allows them to perform maintenance tasks and manage the devices conveniently from the cloud or local network.

## Monitoring and Diagnostics

The Monitoring and Diagnostics feature enables users to monitor and diagnose the system. This includes the ability to view device status, check device health, and monitor system performance.

## Audit Logs

The Audit Logs feature enables the recording of all events that occur within the system. This ensures that all actions are logged and can be reviewed for auditing purposes.

## Traceability

The Traceability feature enables the tracking of all events that occur within the system. This ensures that all actions are tracked and can be reviewed for auditing purposes.

## Device connection monitoring

The Device Connection Monitoring feature enables users to monitor the connection status of devices. This ensures that devices are always connected and can be managed conveniently.