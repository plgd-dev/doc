---
title: 'Identity Store'
description: 'What is plgd Identity Store?'
date: '2023-06-16'
categories: [storage]
keywords: [users, owners]
weight: 1
isMainSectionPage: true
---

The Identity Store serves as a repository for maintaining the relationship between devices and their owners. When a device is registered with the Hub, it indicates that the device is assigned to a specific owner in the Identity Store. Conversely, when a device is unregistered from the Hub, it means that the device is no longer associated with an owner in the Identity Store. In essence, the Identity Store functions as a means to manage the ownership of devices within the Hub.

To ensure synchronization, any modifications made to the Identity Store are communicated to the Hub through two events: `DevicesRegistered` and `DevicesUnregistered`. These events serve as mechanisms for propagating changes in the Identity Store to the Hub, ensuring that the device ownership is accurately reflected across the system.
