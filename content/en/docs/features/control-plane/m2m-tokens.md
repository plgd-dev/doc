---
title: 'Machine to Machine Tokens'
description: 'What is a Machine to Machine token?'
date: '2024-07-05'
categories: [oauth]
keywords: [oauth, m2m, token]
weight: 70
---

The Machine to Machine OAuth Server (M2M OAuth Server) is used to generate API tokens to access the plgd services by other services. These tokens are acquired via the client credentials flow with the client assertion type JWT or secret in the case of service. The tokens are exclusively used with plgd services.

## Why Use M2M Tokens

Machine to Machine tokens are crucial for secure and efficient communication between services without human intervention. Here are some of the key benefits:

- **Security**: M2M tokens ensure that only authorized services can access your APIs, providing a secure way to manage access control.
- **Automation**: They enable automated workflows by allowing services to authenticate and authorize themselves, reducing the need for manual intervention.
- **Scalability**: M2M tokens facilitate the seamless scaling of services by allowing them to communicate and perform operations independently.
- **Efficiency**: Using M2M tokens can reduce latency and improve the performance of interactions between services since tokens are managed programmatically.

## How to Use M2M Tokens in plgd

In plgd, M2M tokens can be used for various purposes to enhance the functionality and security of your services. Here are some practical applications:

- **Device Provisioning**: M2M tokens are used for [automatic device provisioning](/docs/features/secured-onboarding-devices/secured-onboarding-devices/#automatic-device-provisioning), allowing you to onboard devices securely and efficiently.
- **Device Configuration**: After a device has been onboarded, M2M tokens enable you to [preconfigure devices](TODO) with the necessary settings.
- **Service Access**: If you have a service that needs to access plgd services, M2M tokens ensure secure and authorized interactions.

To start using M2M tokens, follow these steps:

1. **Obtain Client Credentials**: Register your service with the M2M OAuth Server to obtain the necessary client credentials (client ID and secret).
2. **Acquire Tokens**: Use the client credentials flow to acquire an M2M token. This involves sending a request to the OAuth server with your client credentials to receive an access token.
3. **Use Tokens for Authentication**: Include the acquired token in the authorization header of your API requests to authenticate and access plgd services.

By leveraging M2M tokens, you can enhance the security, automation, and efficiency of your service interactions with plgd, ensuring a robust and scalable integration.
