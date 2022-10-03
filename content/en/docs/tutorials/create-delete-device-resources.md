---
title: 'Create & Delete device resources'
description: 'Understand how to create and delete resources hosted by the device dynamically'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [tutorials, device library]
keywords: [device, iotivity, create resource, delete resource]
menu:
  docs:
    parent: tutorials
    weight: 50
toc: true
---

## Creating resources

Device with a collection resource allows dynamic creation of resources. The created resource can only be of a well defined type (see call to `oc_collections_add_rt_factory` in [Guide](#define-constructor-and-destructor)) and all created resources are contained within the collection.

### How to create a resource

To develop your own device you can check the example in [cloud_server](https://github.com/iotivity/iotivity-lite/blob/master/apps/cloud_server.c). Lets examine the code to identify the necessary steps that allow a device to dynamically create resources in a collection.

#### Create a collection resource

```C
oc_resource_t* col = oc_new_collection(NULL, "/switches", 1, 0);
oc_resource_bind_resource_type(col, "oic.wk.col");
```

For precise description of the arguments of the given functions please refer to the iotivity-lite documentation.

#### Determine which resource types can populate the collection

```C
oc_collection_add_supported_rt(col, "oic.r.switch.binary");
```

Supported resource types are visible through `oic.if.baseline` interface in the `rts` property of the collection.

#### Enable create operation on the collection resource

```C
oc_resource_bind_resource_interface(col, OC_IF_CREATE);
```

Supporting creating of resources is visible through `oic.if.baseline` interface in the `if` property of the collection. To support creation of resources the property must contain the `oic.if.create` interface.

#### Define constructor and destructor

```C
oc_collections_add_rt_factory("oic.r.switch.binary", new_switch_instance, free_switch_instance);
```

```C
typedef struct oc_switch_t
{
  struct oc_switch_t *next;
  oc_resource_t *resource;
  bool state;
} oc_switch_t;
OC_MEMB(switch_s, oc_switch_t, 1);
OC_LIST(switches);

static oc_resource_t*
new_switch_instance(const char* href, oc_string_array_t *types,
                    oc_resource_properties_t bm, oc_interface_mask_t iface_mask,
                    size_t device)
{
  oc_switch_t *cswitch = (oc_switch_t *)oc_memb_alloc(&switch_s);
  if (cswitch) {
    cswitch->resource = oc_new_resource(
      NULL, href, oc_string_array_get_allocated_size(*types), device);
    if (cswitch->resource) {
      size_t i;
      for (i = 0; i < oc_string_array_get_allocated_size(*types); i++) {
        const char *rt = oc_string_array_get_item(*types, i);
        oc_resource_bind_resource_type(cswitch->resource, rt);
      }
      oc_resource_bind_resource_interface(cswitch->resource, iface_mask);
      cswitch->resource->properties = bm;
      oc_resource_set_default_interface(cswitch->resource, OC_IF_A);
      oc_resource_set_request_handler(cswitch->resource, OC_GET, get_cswitch,
                                      cswitch);
      oc_resource_set_request_handler(cswitch->resource, OC_DELETE, delete_cswitch,
                                      cswitch);
      oc_resource_set_request_handler(cswitch->resource, OC_POST, post_cswitch,
                                      cswitch);
      oc_resource_set_properties_cbs(cswitch->resource, get_switch_properties,
                                     cswitch, set_switch_properties, cswitch);
      oc_add_resource(cswitch->resource);
      oc_set_delayed_callback(cswitch->resource, register_to_cloud, 0);

      oc_list_insert(switches, prev, cswitch);
      return cswitch->resource;
    }
    oc_memb_free(&switch_s, cswitch);
  }
  return NULL;
}

static void
free_switch_instance(oc_resource_t *resource)
{
  oc_switch_t *cswitch = (oc_switch_t *)oc_list_head(switches);
  while (cswitch) {
    if (cswitch->resource == resource) {
      oc_delete_resource(resource);
      oc_list_remove(switches, cswitch);
      oc_memb_free(&switch_s, cswitch);
      return;
    }
    cswitch = oc_list_item_next(cswitch);
  }
}
```

#### Compile and link

To enable create operation in iotivity-lite library compile with CREATE=1 option.

```make
make cloud_server CLOUD=1 SECURE=0 CREATE=1
```

#### Create a resource

When you have a hub backend and a cloud_server binary running, you can use a hub client to create a resource.

#### Create by go grpc client

Go [grpc client](https://github.com/plgd-dev/hub/tree/main/bundle/client/grpc) is a simple tool that supports several useful commands we can combine to create a resource.

- Use the get command to identify the collection device

The get command retrieves data of all available devices. To correctly call the create command the device `id` and `href` properties are necessary. In the output find the item with type `oic.wk.col`.

```bash
// retrieves all resources of all devices
./grpc -get
```

Output:

```json
...
{
    "content": {
        "content_type": "application/vnd.ocf+cbor",
        "data": "n/8="
    },
    "resource_id": {
        "device_id": "2b9ed3ed-ddf3-4c9c-4d21-9ec1f6ba6b03",
        "href": "/switches"
    },
    "status": 1,
    "types": [
        "oic.wk.col"
    ]
}
...
```

- Create a binary switch resource in the collection

```bash
./grpc -create -deviceid 2b9ed3ed-ddf3-4c9c-4d21-9ec1f6ba6b03 -href /switches <<EOF
{
    "rt": [
        "oic.r.switch.binary"
    ],
    "if": [
        "oic.if.a",
        "oic.if.baseline"
    ],
    "rep": {
        "value": false
    },
    "p": {
        "bm": 3
    }
}
EOF
```

The command creates a binary switch (`oic.r.switch.binary`), which supports actuator interface (`oic.if.a`) and has two possible states ("value": `true`/`false`). The switch is set to be discoverable and observable (`bm`: 3; mask value 3 equals `OC_DISCOVERABLE | OC_OBSERVABLE`).

- Use the get command again to examine the newly created switch

```bash
// retrieves all resources of all devices
./grpc -get
```

Output:

```json
...
{
    "content": {
        "content_type": "application/vnd.ocf+cbor",
        "data": "v2V2YWx1ZfT/"
    },
    "resource_id": {
        "device_id": "2b9ed3ed-ddf3-4c9c-4d21-9ec1f6ba6b03",
        "href": "/4rLN4BlwJmFmbbMJblChB2kyT2zJEP"
    },
    "status": 1,
    "types": [
        "oic.r.switch.binary"
    ]
},
...
```

## Deleting resources

Device with collection resource allows to delete dynamically created resource. To delete resource you need to set delete handler during creating resource.

### How to delete a resource

To develop your own device check the example in [cloud_server](https://github.com/iotivity/iotivity-lite/blob/master/apps/cloud_server.c). Lets examine the code to identify the necessary steps that allow a device to delete a dynamically created resource from a collection.

#### Set delete handler at constructor

```C
// delete handler of switch
static void
delete_cswitch(oc_request_t *request, oc_interface_mask_t iface_mask,
            void *user_data)
{
  OC_DBG("%s", __func__);
  (void)request;
  (void)iface_mask;
  oc_switch_t *cswitch = (oc_switch_t *)user_data;

  // cleanup resource data
  ...

  // we cannot delete resource immediately via oc_delete_resource because
  // it is used during invocation delete handler. So we plan deleting via oc_delayed_delete_resource call.
  oc_delayed_delete_resource(cswitch->resource);
}
```

```C
static oc_resource_t*
new_switch_instance(const char* href, oc_string_array_t *types,
                    oc_resource_properties_t bm, oc_interface_mask_t iface_mask,
                    size_t device)
{
  ...
      oc_resource_set_request_handler(cswitch->resource, OC_GET, get_cswitch,
                                      cswitch);

      // set delete handler to the created resource
      oc_resource_set_request_handler(cswitch->resource, OC_DELETE, delete_cswitch,
                                      cswitch);
      oc_resource_set_request_handler(cswitch->resource, OC_POST, post_cswitch,
                                      cswitch);
  ...
}
```

#### Delete by go grpc client

Delete a created binary switch resource from the collection.

```bash
./grpc -delete -deviceid 2b9ed3ed-ddf3-4c9c-4d21-9ec1f6ba6b03 -href /4rLN4BlwJmFmbbMJblChB2kyT2zJEP
```
