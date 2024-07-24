---
title: 'NATS'
description: 'How does plgd publish and subscribe to events?'
date: '2024-07-18'
categories: [features]
keywords: ['events', 'nats']
weight: 75
---

Plgd hub services use NATS as an EventBus and MongoDB as an EventStore.

## NATS Subjects Overview

### Definitions

- `ownerID`: The owner of the device, calculated as `uuid.NewV5(uuid.NamespaceURL, value of JWT ownerClaim)`.
- `deviceID`: The UUID of the device.
- `hrefID`: The identifier of a resource, calculated as `uuid.NewV5(uuid.NamespaceURL, href)`, where `href` is a resource path (e.g. "/oic/d").
- `leadResourceType`: The selected resource type for resource events.
- `leadResourceTypeID`: The identifier of the selected resource type, calculated as `uuid.NewV5(uuid.NamespaceURL, resourceType)`, where `resourceType` is a resource type (e.g. "oic.wk.d").

### Device Events

Each event is compressed by [snappy](https://github.com/google/snappy) and encoded in protobuf [devices event envelope](https://github.com/plgd-dev/hub/blob/main/resource-aggregate/cqrs/eventbus/pb/eventbus.proto). The event envelope consists of `Event.data` containing the event and `Event.event_type` describing the type of the event.

#### Resource Links

- `plgd.owners.{ownerID}.devices.{deviceID}.resource-links.{eventType}` publishes resource-links events of types `resourcelinkspublished`, `resourcelinksunpublished`, and `resourcelinkssnapshottaken` for device `deviceID` and `ownerID`.

#### Metadata Events

- `plgd.owners.{ownerID}.devices.{deviceID}.metadata.{eventType}` publishes metadata events of types `devicemetadataupdatepending`, `devicemetadataupdated`, and `devicemetadatasnapshottaken` for device `deviceID` and `ownerID`.

#### Resource Events

- `plgd.owners.{ownerID}.devices.{deviceID}.resources.{hrefID}.{eventType}` publishes resource events of types `resourcechanged`, `resourcecreated`, `resourcecreatepending`, `resourcedeleted`, `resourcedeletepending`, `resourceretrieved`, `resourceretrievepending`, `resourcestatesnapshottaken`, `resourceupdated`, and `resourceupdatepending` for resource `hrefID`, `deviceID`, and `ownerID`.

##### Lead Resource Type

The Lead Resource Type feature in the plgd hub allows for the specification and handling of a primary resource type for resource events. It is disabled by default but can be enabled through configuration settings for both NATS publishers and subscribers.

###### Configuration

To enable the Lead Resource Type feature, modify the YAML configuration files of your service:

1. **Publisher Configuration:**

    ```yaml
    leadResourceType:
        enabled: {bool value}
        regexFilter: {array of REGEX expressions}
        filter: {"first", "last", or ""}
        useUUID: {bool value}
    ```

2. **Subscriber Configuration:**

    ```yaml
    leadResourceType:
        enabled: {bool value}
    ```

###### Subjects with Lead Resource Type

When enabled, the subject is modified based on the selected resource type. If the resource type is not empty, the subject will include the `leadrt` prefix and the resource type as a string. If `useUUID` is set to true, the resource type is encoded to a UUID string. The subject format is modified as follows:

1. **With `useUUID=false`:**

    ```text
    plgd.owners.{ownerID}.devices.{deviceID}.resources.{hrefID}.{eventType}.leadrt.{leadResourceType}
    ```

2. **With `useUUID=true`:**

    ```text
    plgd.owners.{ownerID}.devices.{deviceID}.resources.{hrefID}.{eventType}.leadrt.{leadResourceTypeID}
    ```

3. **When the resource type is empty:**

    ```text
    plgd.owners.{ownerID}.devices.{deviceID}.resources.{hrefID}.{eventType}.leadrt
    ```

Even when no resource type is selected, the `leadrt` token is added to the subject, allowing the use of the `.>` wildcard to subscribe to resource events with no or any lead resource type.

###### Selecting the Lead Resource Type

The lead resource type for a given resource is selected based on the following criteria:

- **Regex Filter:**

  - `regexFilter` is an array of regex expressions used to match resource types.
  - The first matching resource type in the array is used.

- **Filter:**

  - `filter` is an enumeration of string values ("first", "last").
  - If `regexFilter` does not match any resource type, the `filter` property determines which resource type to use:
    - If `filter` is "first", the first resource type is used.
    - If `filter` is "last", the last resource type is used.
    - Otherwise, no resource type is used.

The algorithm for selecting the lead resource type is as follows:

```pseudocode
# Iterate regexFilter first
iterate resource types of given resource
    iterate regexFilter expressions
        if the expression matches the resource type
            use the resource type

# regexFilter was empty or no resource type was matched
if filter equals "first"
    use the first resource type
else if filter equals "last"
    use the last resource type
else
    don't use resource type
```

###### Configure Lead Resource Type at plgd #bundle

The following environmental variables can be used to configure the Lead Resource Type feature for plgd #bundle:

- set `LEAD_RESOURCE_TYPE_ENABLED`, `LEAD_RESOURCE_TYPE_USE_UUID` to boolean values
- set `LEAD_RESOURCE_TYPE_FILTER` to "first", "last" or ""
- set `LEAD_RESOURCE_TYPE_REGEX_FILTER` to comma separated list of REGEX expressions

For example:

```bash
LEAD_RESOURCE_TYPE_ENABLED=true
LEAD_RESOURCE_TYPE_FILTER="last"
LEAD_RESOURCE_TYPE_REGEX_FILTER="^oic.*,core.*"
LEAD_RESOURCE_TYPE_USE_UUID=true
```

```bash
docker run -it --rm -e LEAD_RESOURCE_TYPE_ENABLED=true -e  LEAD_RESOURCE_TYPE_FILTER="last" -e  LEAD_RESOURCE_TYPE_REGEX_FILTER="^oic.*,core.*" -e  LEAD_RESOURCE_TYPE_USE_UUID=true --network=host -v `pwd`/.tmp/data:/data ghcr.io/plgd-dev/hub/bundle:latest
```

### Owner events

Each event is encoded in protobuf [event envelope](https://github.com/plgd-dev/hub/blob/main/authorization/pb/events.proto) and then compressed by [snappy](https://github.com/google/snappy).

#### Registration events

- `plgd.owners.{ownerID}.registrations.{eventType}` publishes owner events of types `devicesregistered`,`devicesunregistered` for `ownerID`.

### Consumer subscriptions options

For the consumers of events you can subscribe to:

- `plgd.owners.>` retrieves all events of the hub.
- `plgd.owners.{ownerId}.>` retrieves all events of the owner with the `ownerId`.
- `plgd.owners.*.devices.{deviceId}.>` retrieves all events of the device with the `deviceId`.
- `plgd.owners.*.devices.{deviceId}.resource-links.>` retrieves all resource link events of the device with the `deviceId`.
- `plgd.owners.*.devices.{deviceId}.resource-links.resourcelinkspublished` retrieves the `resourcelinkspublished` event of the device with the `deviceId`.
- `plgd.owners.*.devices.*.resource-links.>` retrieves all resource link events of all devices.
- `plgd.owners.{ownerId}.devices.*.resource-links.>` retrieves all resource link events of all devices belonging to the owner with the `ownerId`.
- `plgd.owners.*.devices.{deviceId}.metadata.>` retrieves all metadata events of the device with the `deviceId`.
- `plgd.owners.*.devices.{deviceId}.metadata.devicemetadataupdated` retrieves the `devicemetadataupdated` event of the device with the `deviceId`.
- `plgd.owners.*.devices.*.metadata.>` retrieves all metadata events of all devices.
- `plgd.owners.{ownerId}.devices.*.metadata.>` retrieves all metadata events of all devices belonging to the owner with the `ownerId`.
- `plgd.owners.*.devices.{deviceId}.resources.>` retrieves all resource events of the device with the `deviceId`.
- `plgd.owners.*.devices.{deviceId}.resources.{hrefId}.>` retrieves all events of the resource with the `hrefId` for the device with the `deviceId`.

If the Lead Resource Type feature is disabled:

- `plgd.owners.*.devices.{deviceId}.resources.{hrefId}.{eventType}` retrieves the `eventType` events of the resource with the `hrefId` for the device with the `deviceId`.
- `plgd.owners.*.devices.{deviceId}.resources.*.{eventType}` retrieves the `eventType` events of all resources for the device with the `deviceId`.
- `plgd.owners.*.devices.*.resources.*.{eventType}` retrieves the `eventType` events of all resources for all devices.
- `plgd.owners.*.devices.*.resources.{hrefId}.{eventType}` retrieves the `eventType` events of the resource with the `hrefId` for all devices.
- `plgd.owners.{ownerId}.devices.*.resources.*.{eventType}` retrieves the `eventType` events of all resources for all devices belonging to the owner with the `ownerId`.
- `plgd.owners.{ownerId}.devices.*.resources.{hrefId}.{eventType}` retrieves the `eventType` events of the resource with the `hrefId` for all devices belonging to the owner with the `ownerId`.

With the Lead Resource Type feature enabled, the equivalent subjects are modified to:

- `plgd.owners.*.devices.{deviceId}.resources.{hrefId}.{eventType}.leadrt.{leadResourceType}`
- `plgd.owners.*.devices.{deviceId}.resources.*.{eventType}.>`
- `plgd.owners.*.devices.*.resources.*.{eventType}.>`
- `plgd.owners.*.devices.*.resources.{hrefId}.{eventType}.>`
- `plgd.owners.{ownerId}.devices.*.resources.*.{eventType}.>`
- `plgd.owners.{ownerId}.devices.*.resources.{hrefId}.{eventType}.>`

These subscription options provide flexibility for consumers to filter events based on specific criteria, such as device ID, owner ID, and resource ID, and they also support wildcard subscriptions for broader event capture.
