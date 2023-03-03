---
title: 'Zero Touch Provisioning via DHCP'
description: 'How is configured Zero Touch Provisioning via DHCP?'
date: '2023-03-03'
categories: [zero-touch, provisioning, dhcp]
keywords: [dhcp, provisioning]
weight: 5
---

DHCP-based zero touch provisioning is a method of configuring devices using DHCP, which is a protocol for automatically assigning IP addresses to devices on a network. The text explains that in order to provision a device, it is necessary to configure the device with the necessary TLS settings and endpoint. However, the root certificate authority via DHCP is too large, so instead, the user must provide a fingerprint of any certificate in the chain and the used message digest (eg SHA256,SHA384).

## Get fingerprint from the certificate

The command `openssl s_client -connect dps.mycompany.com:5684 -showcerts` returns the certificate chain in PEM format. The certificate chain is a sequence of certificates that verifies the identity of the server.

To configure a device for zero touch provisioning using DHCP, you need to obtain the fingerprint of one of the certificates in the chain. A fingerprint is a unique identifier that is computed from the contents of the certificate.

```bash
openssl s_client -connect dps.mycompany.com:5684 -showcerts
...
Certificate chain
 0 s:CN = 0.dps.mycompany.com
   i:CN = IntermediateCA
   a:PKEY: id-ecPublicKey, 256 (bit); sigalg: ecdsa-with-SHA256
   v:NotBefore: Jun  7 05:50:08 2022 GMT; NotAfter: Jun  7 05:50:38 2023 GMT
-----BEGIN CERTIFICATE-----
MIIB2jCCAX+...
-----END CERTIFICATE-----
 1 s:CN = IntermediateCA
   i:CN = RootCA
   a:PKEY: id-ecPublicKey, 256 (bit); sigalg: ecdsa-with-SHA256
   v:NotBefore: Oct 14 07:43:25 2020 GMT; NotAfter: Jan  1 00:00:00 2025 GMT
-----BEGIN CERTIFICATE-----
MIIBlzCCAT2g...
-----END CERTIFICATE-----
```

In this particular example, the SHA256 message digest is used to compute the fingerprint. The SHA256 message digest is a cryptographic hash function that produces a fixed-length output. This output of command `openssl x509 -noout -fingerprint -sha256 -inform pem <<EOF...` can be used as a fingerprint.

```bash
openssl x509 -noout -fingerprint -sha256 -inform pem <<EOF
-----BEGIN CERTIFICATE-----
MIIBlzCCAT2g...
-----END CERTIFICATE-----
EOF
sha256 Fingerprint=63:25:73:b8:f9:e:b9:24:54:e5:4:c4:79:61:3b:34:21:c5:f7:20:fe:fa:65:f6:f:51:6c:1e:72:5c:ec:73
```

The fingerprint and the message digest algorithm (in this case, SHA256) must be configured in the DHCP server so that the device can obtain the necessary configuration parameters during provisioning. Additionally, the device must be configured to allow the use of the configured message digest algorithm.

## Configure isc-dhcp-server

To configure isc-dhcp-server to provide Zero Touch Provisioning via DHCP, modify the /etc/dhcp/dhcpd.conf file to include the following lines:

```text
# dns servers
option domain-name-servers 8.8.8.8;
# ntp servers
option ntp-servers pool.ntp.org;
option space MY-COMPANY;
option MY-COMPANY.dps-endpoint code 200 = text;
option MY-COMPANY.dps-certificate-fingerprint code 201 = string;
option MY-COMPANY.dps-certificate-fingerprint-md-type code 202 = text;
class "MY-COMPANY-CLASS" {
  # to match substring of vendor-class-identifier option
  match if substring (option vendor-class-identifier,0,17) = MY-COMPANY-DEVICE;
  vendor-option-space MY-COMPANY;
  option MY-COMPANY.dps-endpoint "coaps+tcp://dps.mycompany.com:5684";
  # Fingerprint calculated from certificate
  option MY-COMPANY.dps-certificate-fingerprint 63:25:73:b8:f9:e:b9:24:54:e5:4:c4:79:61:3b:34:21:c5:f7:20:fe:fa:65:f6:f:51:6c:1e:72:5c:ec:73;
  # Message digest type used to calculate fingerprint
  option MY-COMPANY.dps-certificate-fingerprint-md-type SHA256;
}
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

After configuring the DHCP server, it is necessary to reload the configuration for the changes to take effect.

```bash

sudo systemctl restart isc-dhcp-server

```

## Configure isc-dhcp-client

For the client-side configuration, modify the `/etc/dhcp/dhclient.conf` file and add the following lines:

```config

option space MY-COMPANY;
option MY-COMPANY.dps-endpoint code 200 = text;
option MY-COMPANY.dps-certificate-fingerprint code 201 = string;
option MY-COMPANY.dps-certificate-fingerprint-md-type code 202 = text;
vendor option space MY-COMPANY;
# device identifier with prefix as in dhcpd.conf and SN
send vendor-class-identifier MY-COMPANY-DEVICE-SN-123456;
send host-name = gethostname();
# list of requested options from DHCP server
request subnet-mask, broadcast-address, time-offset, routers,
        domain-name, domain-name-servers, domain-search, host-name,
        dhcp6.name-servers, dhcp6.domain-search, dhcp6.fqdn, dhcp6.sntp-servers,
        netbios-name-servers, netbios-scope, interface-mtu,
        ntp-servers,
        vendor-encapsulated-options;

```

Once the DHCP client has obtained the configuration from the DHCP server, the setup will be stored in the `/var/lib/dhcp/dhclient*.leases` file. The last lease configuration in this file will be the one currently in use.

To acquire configuration from the DHCP server, the device network interface must be restarted. (eg reboot device)

```text

lease {
  interface "enp0s3";
  fixed-address 10.115.115.11;
  option subnet-mask 255.255.255.0;
  option routers 10.115.115.10;
  option dhcp-lease-time 20;
  option dhcp-message-type 5;
  option domain-name-servers 10.115.115.10;
  option dhcp-server-identifier 10.115.115.10;
  option ntp-servers 62.168.65.36,213.81.129.99,213.81.129.98,185.242.56.5;
  option vendor-encapsulated-options c8:20:63:6f:61:70:73:2b:74:63:70:3a:2f:2f:74:72:79:2e:70:6c:67:64:2e:63:6c:6f:75:64:3a:32:35:36:38:34:c9:20:63:25:73:b8:f9:e:b9:24:54:e5:4:c4:79:61:3b:34:21:c5:f7:20:fe:fa:65:f6:f:51:6c:1e:72:5c:ec:73:ca:6:53:48:41:32:35:36;
  option infinera.dps-endpoint "coaps+tcp://dps.mycompany.com:5684";
  option infinera.dps-certificate-fingerprint 63:25:73:b8:f9:e:b9:24:54:e5:4:c4:79:61:3b:34:21:c5:f7:20:fe:fa:65:f6:f:51:6c:1e:72:5c:ec:73;
  option infinera.dps-certificate-fingerprint-md-type "SHA256";
  renew 2 2023/03/14 13:39:46;
  rebind 2 2023/03/14 13:39:52;
  expire 2 2023/03/14 13:39:55;
}

```

## Connect with client library

When the DHCP client leases file is updated with a new vendor-encapsulated-options, the [client library](../client-library) needs to be notified of the update. To accomplish this on Linux, `inotify` can be used to watch the leases file and notify the client library about the new configuration, or the client library can periodically pull the configuration from the leases file.

Once the new vendor-encapsulated-options parsed from the leases file, it can be set to the client library.
Similar function need to be called for the changes to take effect:

```c

static bool
update_from_vendor_encapsulated_options(struct plgd_dps_context_t *dps_ctx, const char *vendor_encapsulated_options)
{
  // decode hex string to binary data
  char binary_vendor_encapsulated_options[4096];
  ssize_t len = plgd_dps_hex_string_to_bytes(vendor_encapsulated_options, strlen(vendor_encapsulated_options), binary_vendor_encapsulated_options, sizeof(binary_vendor_encapsulated_options));
  if (len < 0) {
    DPS_ERR("invalid character in vendor encapsulated options");
    return false;
  }

 switch (plgd_dps_dhcp_set_values_from_vendor_encapsulated_options(dps_ctx, binary_vendor_encapsulated_options, (size_t)len)) {
  case -1:
    DPS_ERR("pull vendor_encapsulated_options: error during update");
    break;
  case 0:
    DPS_DBG("pull vendor_encapsulated_options: no change");
    break;
  case 1:
    DPS_DBG("pull vendor_encapsulated_options: updated but force re-provision is not needed");
    break;
  case 2: {
    DPS_DBG("pull vendor_encapsulated_options: updated but needed force re-provision and restart dps manager");
    plgd_dps_force_reprovision(dps_ctx);
    if (plgd_dps_manager_restart(dps_ctx)) {
      DPS_ERR("pull vendor_encapsulated_options: failed to restart dps manager");
    }
    } break;
  }
  return true;
}

```
