---
title: 'Advanced security'
description: 'How plgd hub tests iotivity-lite and vice versa'
date: '2021-10-14'
lastmod: '2021-10-14'
categories: [developer guide, testing]
keywords: [developer guide, testing]
menu:
  docs:
    parent: developer-guide
    weight: 10
toc: true
---


## Testing Plgd Hub pull requests

When a developer creates a pull request at [plgd hub](https://github.com/plgd-dev/hub), it automatically triggers tests. The tests pulls the latest IoTivity-lite release, which is represented by docker image `ghcr.io/iotivity/iotivity-lite/cloud-server-debug:latest`. It contains all functionality to run with the hub and it is updated when new IoTivity-lite release is published.

{{< plantuml id="device-onboarding" >}}
@startuml
skinparam backgroundColor transparent
hide footbox

participant D as "Developer"
participant CI as "Continuous integration"
participant IL as "IoTivite Lite"


D -> CI: Create or Update pull request
group Continuous integration
    CI -> IL: Pull the latest cloud server image from IoTivity Lite
    return the latest cloud server image
    CI -> CI: Setup environment
    CI -> CI: Run all hub tests over the pull request
end
CI -> D: Inform developer when an error occurs by email
@enduml
{{< /plantuml >}}


## Testing IoTivity-lite pull requests

When a developer creates a pull request at [iotivity-lite](https://github.com/iotivity/iotivity-lite), it automatically triggers tests. The tests pulls the latest Plgd Hub release, which is represented by docker image `ghcr.io/plgd-dev/hub/test-cloud-server:latest`. It contains functionality which is used by plgd hub and it is updated when new Plgd Hub release is published.

{{< plantuml id="device-onboarding" >}}
@startuml
skinparam backgroundColor transparent
hide footbox

participant D as "Developer"
participant CI as "Continuous integration"
participant IL as "IoTivite Lite"

D -> CI: Create or Update pull request
group Continuous integration
    CI -> IL: Pull the latest test cloud server image from Plgd Hub
    return the latest test cloud server image
    CI -> CI: Setup environment
    CI -> CI: Run test cloud server againt current iotivity-lite cloud-server.
end
CI -> D: Inform developer when an error occurs by email
@enduml
{{< /plantuml >}}