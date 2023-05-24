---
title: 'Device access control list'
description: 'How to configure access to the device?'
date: '2022-12-20'
categories: [architecture, d2c, provisioning, acl]
keywords: [architecture, d2c, provisioning, acl]
weight: 7
---

Defines who can access to the device per resource. The access control list is stored in the device in the `/oic/sec/acl2` resource. The access control list is a list of access control entries. Each access control entry defines a subject and a set of permissions for the subject. The subject can be a connection type, a device, or a role. The permissions define what the subject can do with the resource. The permissions are defined in the `permission` property of the access control entry. The permissions are defined as a bit mask. The following table shows the permissions that can be defined in the `permission` property.

| Permission | Value | Notes |
| ---------- | ----- | ----- |
| Create | 1 | Create the resource |
| Read | 2 | Read the resource |
| Write | 4 | Write the resource |
| Delete | 8 | Delete the resource |
| Notify | 16 | Notify the resource |

{{< note >}}
IoTivite lite maps permission Create, Delete to Write permission and Notify is mapped to Read permission.
{{< /note >}}

When resources where the subject has access you can use wildcards `*`/`+`/`-` in the `wildcard` property of the access control resource entry.

| Wildcard | Notes |
| -------- | ----- |
| `*` | Shall match all Non-Configuration Resources |
| `+` | Shall match all Discoverable Non-Configuration Resources which expose at least one Secure OCF Endpoint. |
| `-` | Shall match all Non-Discoverable Non-Configuration Resources which expose at least one Secure OCF Endpoint. |

For device configuration resources you need to specify `href` property in the access control resource entry.

The list of device configuration resources:
| Name | Type | Href | Notes |
| ---- | ---- | ---- | ----- |
| Device configuration | `oic.wk.con` | `/oc/con` | To change device name |
| Device maintenance resource | `oic.wk.mnt` | `/oic/mnt` | For factory reset |
| Device cloud resource | `oic.r.coapcloudconf` | `/CoapCloudConfResURI` | To setup cloud  |
| Device firmware update resource | `oic.r.softwareupdate` | `/oc/swu` | To update device firmware |
| Device secure resources | `/oic/sec/*` | To update device secure resources in provision state |

{{< note >}}
For IoTivity, href is used to match the ACL.
{{< /note >}}

When device is in read for ownership transfer method (RFOTM) state, only device configuration resources and OCF public resources are accessible. ACL is applied only in normal device state. To allow access to the resources in RFOTM state you need to have enabled the feature in IoTivity lite via `cmake -DOC_RESOURCE_ACCESS_IN_RFOTM_ENABLED=ON ...` when building IoTivity lite and mark each resource via function [oc_resource_set_access_in_RFOTM](https://github.com/iotivity/iotivity-lite/blob/ac61ae3b5e6a2fd4cbaab3c8b909209cb8dda982/include/oc_acl.h#L235). The default value is `OFF`.

To update ACL device need to be in provisioning state. To change device state is described in [here](../../tutorials/change-provision-status).

The update of ACL is done via `POST` method on the `/oic/sec/acl2` resource with body:

```jsonc
{
  "aclist2": [
    {
      "permission": 31, // full access
      "resources": [
        {
          "wc": "*", // all resources
          "if": [ "*" ] // all interfaces
        }
      ],
      "subject": { // type of subject is defined by body content
        "uuid": "00000000-0000-0000-0000-000000000000" // device id
      }
    }
  ]
}
```

{{< note >}}
More information about the ACL can be found [here](https://openconnectivity.org/specs/OCF_Security_Specification.pdf) in section 12.
{{< /note >}}
