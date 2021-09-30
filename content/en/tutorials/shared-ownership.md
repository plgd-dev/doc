---
title: 'Share devices within user groups'
description: 'Share devices with more than one person identified as owner'
date: '2021-05-13'
lastmod: '2021-05-13'
categories: [tutorials, authorization]
keywords: [bundle, ownership, device group, oauth, auth0]
menu:
  docs:
    parent: tutorials
    weight: 20
toc: true
---

Devices are organized in the identity service by the owner ID retrieved from the JWT token. The plgd API will be based on this value to identify the user and grant him the permissions only to devices he owns. By default, JWT claim `sub` is used as the owner ID. In case you connect the plgd authorization service with the Auth0, each logged-in user can access only his devices. This behaviour can be changed by changing the `OWNER_CLAIM` configuration property and adding custom claim to your Auth0 users.

## How to use custom claim with Auth0

### Assign claim to user

1. Go to **Users & Roles**
1. Find your user and edit his details
1. Extend the **user_metadata** by a custom claim, e.g.

    ```json
    {
        "tenant": "e3e0102d-a45b-5cb2-a22e-3a0410deb8d6"
    }
    ```

### Assign wildcard permission to your service client

1. Go to **Applications**
1. Edit your **Machine to Machine** application
1. Open **Advanced Settings**, switch to **Application Metadata** and add entry:
    - `Key`: `tenant`
    - `Value`: `*`

### Include custom claim to access token

1. Go to **Rules** and create new one
1. Copy and paste the function below, which uses custom claim `https://plgd.dev/tenant`

    ```js
    function addTenantToAccessToken(user, context, callback) {
        var tenantClaim = 'https://plgd.dev/tenant';
        var tenant = (user && user.user_metadata && user.user_metadata.tenant) || (context && context.clientMetadata && context.clientMetadata.tenant) || null;
        if (tenant) {
            context.accessToken[tenantClaim] = tenant;
            context.idToken[tenantClaim] = tenant;
        }
        return callback(null, user, context);
    }
    ```

After the rule is created, Auth0 include into every access tokens custom claim `https://plgd.dev/tenant` used to group users and "their" devices. In case the custom `OWNER_CLAIM` is configured, devices are no more owned by a single user, but in this case, by the **tenant**. Each user who is a member of the tenant A will be able to access all the devices of this tenant.

{{% warning %}}
If the configuration property `OWNER_CLAIM` is changed, each user is required to have this claim present.
{{% /warning %}}
