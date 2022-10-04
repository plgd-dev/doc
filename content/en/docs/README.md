## Introduction

What are the biggest problems in IoT? Where do the current market IoT solutions fall short? How should you consider IoT for your projects?

To phrase the issue in another way, _"What are the most common issues preventing companies from fully realizing the benefits of IoT?"_ This question was asked by [DZone](https://dzone.com/articles/most-common-problems-with-iot) and answered by 23 executives involved with the Internet of Things.

**Considerations:**

- Companies are not able or do not have the talent, to complete an end-to-end IoT solution.
- Deployment is unexpectedly complex and many companies do not have the skills to accomplish a proper strategy.
- Providing Security needs to be a seamless and secure data fabric platform.
- It’s difficult to make something at scale while maintaining quality.
- Devices need to be scalable so that they connect to everything they need to.
- There will be a large amount of data to run IoT that will need to be stored.

The IoT industry, across numerous market verticals, is at an impasse because customers are demanding increased sophistication at lower prices. Given the complexity and importance of IoT, no single company can or should dictate the path forward for the entire IoT industry.

The only viable path forward is a collaboration between companies and market verticals to develop, test, and standardize functionality. The [Open Connectivity Foundation](https://openconnectivity.org/) is currently working to achieve this goal.

The device-to-hub communication represents a unique challenge for engineers because there has never been a historical need for engineers to become knowledgeable in both embedded systems and cloud-native application development.

Our proposed solution to this problem is to emulate a container runtime interface (CRI) architecture and embody Conway’s law to establish a loose coupling between the "IoT code" (CoAP / IoTivity) and the portions of the system that are much more familiar to the cloud developers.

## IoT Challenges

- Embedded systems engineering and cloud-native application development are typically orthogonal skill sets in the companies whose products would benefit the most from internet connectivity.
- The immense complexity of managing IoT deployments requires managed services.
- There is no seamless portability for IoT devices between clouds.
  - Hardware costs should be decoupled from networking costs like cell phones.
- The Lack of industry standards increases the attack surface of the entire IoT industry.

## plgd Goals

- Address the above challenges in a way that is easy for companies and public clouds to adopt and offer a managed service.
- Ensure a loose coupling between database, messaging, and authorization in the plgd hub implementation.
- Demonstrate a system that integrates database, messaging, and authorization, and to serve as the default choice for companies with common OLTP issues.
