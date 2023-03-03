---
title: 'Zero Touch Provisioning via DHCP'
description: 'How is configured Zero Touch Provisioning via DHCP?'
date: '2022-06-28'
categories: [zero-touch, provisioning, dhcp]
keywords: [dhcp, provisioning]
weight: 5
---

To enable Secure Zero Touch Provisioning via DHCP, DHCP can be used to configure the necessary network settings and endpoints on a device. To do this, the DHCP server must be configured with two specific options: Option 143 for IPv4 (defined in [RFC-8572](https://www.rfc-editor.org/rfc/rfc8572.html#section-8.1)) and Option 136 for IPv6 (defined in [RFC-8572](https://www.rfc-editor.org/rfc/rfc8572.html#section-8.2)). These two DHCP options, Option 143 for IPv4 and Option 136 for IPv6, provide Zero Touch Provisioning (ZTP) endpoints. Other network settings such as IP address, gateway, NTP, and DNS are acquired by the device from the DHCP server.


## Configure isc-dhcp-server

To configure isc-dhcp-server to provide Zero Touch Provisioning via DHCP, modify the /etc/dhcp/dhcpd.conf file to include the following lines:

```config
# option definitions common to all supported networks...
option domain-name "example.org";
# dns servers
option domain-name-servers 8.8.8.8; 
# ntp servers
option ntp-servers pool.ntp.org;        

default-lease-time 600;
max-lease-time 7200;

# zero touch provisioning
# define zero touch provisioning option
option rfc8572-v4-sztp-redirect code 143 = text;
# set zero touch provisioning server
option rfc8572-v4-sztp-redirect "coaps+tcp://try.plgd.cloud:15684"; # sztp server

subnet 10.115.115.0 netmask 255.255.255.0 {
  range 10.115.115.100 10.115.115.200;
  # gateway address
  option routers 10.115.115.1;
  
  # set static ip based on mac address
  host test {
    hardware ethernet 08:00:27:49:cb:b2;
    fixed-address 10.115.115.11;
  }
}
```

After configuring the DHCP server, it is necessary to reload the configuration for the changes to take effect.

```bash
sudo systemctl restart isc-dhcp-server
```

## Configure isc-dhcp-client

For the client-side configuration, modify the `/etc/dhcp/dhclient.conf` file and add the following lines:

```config
# define zero touch provisioning options
option rfc8572-v4-sztp-redirect code 143 = text;
option rfc8572-v6-sztp-redirect code 136 = text;


send host-name = gethostname();

# list of requested options from DHCP server
request subnet-mask, broadcast-address, time-offset, routers,
        domain-name, domain-name-servers, domain-search, host-name,
        dhcp6.name-servers, dhcp6.domain-search, dhcp6.fqdn, dhcp6.sntp-servers,
        netbios-name-servers, netbios-scope, interface-mtu,
        ntp-servers,
        rfc8572-v4-sztp-redirect, rfc8572-v6-sztp-redirect;
```

Once the DHCP client has obtained the configuration from the DHCP server, the setup will be stored in the `/var/lib/dhcp/dhclient*.leases` file. The last lease configuration in this file will be the one currently in use.

```file
default-duid "\000\001\000\001+\222\0344\010\000'I\313\262";
lease {
  interface "enp0s3";
  fixed-address 10.115.115.11;
  option subnet-mask 255.255.255.0;
  option rfc8572-v4-sztp-redirect "coaps+tcp://try.plgd.cloud:15684";
  option routers 10.115.115.1;
  option dhcp-lease-time 600;
  option dhcp-message-type 5;
  option domain-name-servers 8.8.8.8;
  option dhcp-server-identifier 10.115.115.10;
  option ntp-servers 212.55.254.189,185.242.56.5,213.160.182.226,213.81.129.99;
  option domain-name "example.org";
  renew 3 2023/03/01 14:27:19;
  rebind 3 2023/03/01 14:31:30;
  expire 3 2023/03/01 14:32:45;
}
```

## Connect with client library

When the DHCP client leases file is updated with a new Zero Touch Provisioning (ZTP) endpoint, the client library needs to be notified of the update. To accomplish this on Linux, `inotify` can be used to watch the leases file and notify the client library about the new configuration, or the client library can periodically pull the configuration from the leases file.

Once the new ZTP endpoint is parsed from the leases file, it can be set as the new endpoint to the client library.
Similar function need to be called for the changes to take effect:

```c
static int
plgd_dps_update_endpoint(struct plgd_dps_context_t *dps_ctx, const char *endpoint)
{
  char old_endpoint[256];
  int n = plgd_dps_get_endpoint(dps_ctx, old_endpoint, sizeof(old_endpoint) -1);
  if (n < 0) {
    DPS_ERR("update dps endpoint: cannot get old endpoint");
    return -1;
  }
  old_endpoint[n] = '\0';
  if (strcmp(old_endpoint, endpoint) == 0) {
    DPS_DBG("update dps endpoint: %s is not change", endpoint);
    return 0;
  }
  DPS_DBG("update dps endpoint: %s is changed to %s", old_endpoint, endpoint);
  plgd_dps_set_endpoint(dps_ctx, endpoint);
  plgd_dps_force_reprovision(dps_ctx);
  if (plgd_dps_manager_restart(dps_ctx)) {
    DPS_ERR("update dps endpoint: failed to restart dps manager");
    return -1;
  }
  return 0;
}
```
