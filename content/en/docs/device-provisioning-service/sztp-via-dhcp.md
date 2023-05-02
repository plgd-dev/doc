---
title: 'Zero Touch Provisioning via DHCP'
description: 'How is configured Zero Touch Provisioning via DHCP?'
date: '2023-03-03'
categories: [zero-touch, provisioning, dhcp]
keywords: [dhcp, provisioning]
weight: 5
---

DHCP-based zero touch provisioning is a method of configuring devices using DHCP, which is a protocol for automatically assigning IP addresses to devices on a network. This article explains how to configure zero-touch provisioning via DHCP.

## Overview

When provisioning a device, it needs to be configured with the endpoint and, if necessary, the TLS settings. There are three options for securing the TLS connection between the device and the DPS service:

### 1. Use a Root Certificate Authority (mTLS)

The manufacturer preinstalls a root certificate authority (CA) certificate of the DPS service in the device. During firmware updates, the certificates are updated. Each DPS service needs to have a leaf certificate signed by the root preinstalled CA certificate. To obtain the certificate, the manufacturer's customer needs to use a certificate signing request (CSR) and can sign for an intermediate certificate authority or a leaf certificate. If the manufacturer provides an intermediate certificate authority, the customer can sign for another intermediate CA or leaf certificate. To limit sub-intermediate CA, the [pathLenConstraint](https://www.rfc-editor.org/rfc/rfc5280#section-4.2.1.9) field can be used (0 means that intermediate CA can only create a leaf certificate). When the device establishes a TLS connection with the DPS service, all intermediate CA certificates must be provided in the chain because the device only contains the root CA certificate. The manufacturer is responsible for updating the root CA certificate in the device via firmware update and providing the new intermediate CA or leaf certificate to the customer for the rotation of the root CA certificate.

From the device implementation point of view, the root certificate authority (CA) certificates can be stored in a directory, and for each of them, you can read the certificate and add it to the trusted manufacturer certificate store of iotivity-lite using [oc_pki_add_mfg_trust_anchor](http://iotivity.org/iotivity-lite-doxygen/oc__pki_8h.html#ab429d36577356b5b821e919d43320110) during device initialization.

### 2. Use a Fingerprint of the Certificate (mTLS*)

{{< warning >}}
If the device is connected to a network with an **untrustworthy DHCP server**, using the fingerprint to verify the DPS certificate **does not provide any security**. In such a scenario, it is the same as using a skip certificate verification.
{{< /warning >}}

However, the root certificate authority via DHCP is too large. Therefore, the user must provide a fingerprint of any certificate in the chain and the message digest used (e.g., SHA256, SHA384). In this case, the device is configured with the fingerprint of the DPS certificate, which is a unique identifier computed from the certificate. The fingerprint is used to verify the DPS certificate, where the DPS chain must contain one certificate with the same fingerprint as configured via DHCP. The message digest type used to calculate the fingerprint must be configured in the DHCP server so that the device can obtain the necessary [configuration](#how-to-configure-zero-touch-provisioning-via-dhcp) parameters during provisioning. Additionally, the device must be configured to allow the use of the configured message digest algorithm.

### 3. Use a Skip Certificate Verification (TLS)

{{< warning >}}
This is the simplest way to configure the device, but an intruder can steal the device when it is connected to the network with an untrusted DHCP server and replace the DPS endpoint with the intruder's server.
{{< /warning >}}

When using the Skip Certificate Verification (TLS) option, the device trusts any certificates sent by the DPS service, but the DPS service still verifies the device's certificate during the TLS handshake. This option should only be used when the device is connected to a trusted network, since an untrusted network could allow an attacker to steal the device and replace the DPS endpoint with their own server. If you want to skip the certificate verification to the DPS service, you can set the `skip_verify` parameter to true using the `plgd_dps_set_skip_verify` function. More details on this function can be found in the [dps.h](https://docs.plgd.dev/docs/device-provisioning-service/static/dps-release/dps.h).

## How to configure Zero Touch Provisioning via DHCP

### Configuring kea-dhcp-server

To configure kea-dhcp-server to provide zero touch provisioning via DHCP, modify the `/etc/kea/kea-dhcp4.conf` file to include the following lines:

```jsonc
{
  "Dhcp4": {
    "interfaces-config": {
        "interfaces": [ "eth0" ] // your network interfaces where you want to provide DHCP
    },
    "option-data": [
      {
        "space": "dhcp4",
        "name": "domain-name",
        "code": 15,
        "data": "example.org" // your domain name
      },
      {
        "space": "dhcp4",
        "name": "domain-name-servers",
        "code": 6,
        "data": "8.8.8.8" // your DNS server
      },
      {
        "space": "dhcp4",
        "name": "ntp-servers",
        "code": 42,
        "data": "213.81.129.99" // your NTP server
      }
    ],
    "option-def": [
      {
        "space": "MY-COMPANY",
        "name": "dps-endpoint", // endpoint of DPS service
        "code": 200,
        "type": "string"
      },
      {
        "space": "MY-COMPANY",
        "name": "dps-certificate-fingerprint", // fingerprint of any DPS certificate in the chain
        "code": 201,
        "type": "binary"
      },
      {
        "space": "MY-COMPANY",
        "name": "dps-certificate-fingerprint-md-type", // message digest type used to calculate the fingerprint
        "code": 202,
        "type": "string"
      }
    ],
    "client-classes": [
      {
        "name": "MY-COMPANY-CLASS",
        // to match substring of vendor-class-identifier option
        "test": "substring(option[60].hex,0,10) == 'MY-COMPANY-DEVICE'",
        "option-def": [
          {
            "name": "vendor-encapsulated-options",
            "code": 43,
            "type": "empty",
            "encapsulate": "MY-COMPANY"
          }
        ],
        "option-data": [
          {
            "name": "vendor-encapsulated-options",
            "code": 43
          },
          {
            "space": "MY-COMPANY",
            "name": "dps-endpoint",
            "data": "coaps+tcp://dps.mycompany.com:5684"
          },
          {
            "space": "MY-COMPANY",
            "name": "dps-certificate-fingerprint",
            "data": "A1E1C34C3E03178DE47779F992287DFEB4B7702F80EED915DDECD654E4C64FE2"
          },
          {
            "space": "MY-COMPANY",
            "name": "dps-certificate-fingerprint-md-type",
            "data": "SHA256"
          }
        ]
      }
    ],
    "subnet4": [
      {
        "id": 1,
        "subnet": "10.115.115.0/24",
        "pools": [
          {
            "pool": "10.115.115.100 - 10.115.115.200"
          }
        ],
        "option-data": [
          {
            "space": "dhcp4",
            "name": "routers",
            "code": 3,
            "data": "10.115.115.10"
          }
        ]
      }
    ],
    "host-reservation-identifiers": [
      "hw-address"
    ],
    "reservation-mode": "global",
    "reservations": [
 //   # set static ip
      {
        "hostname": "test",
        "hw-address": "08:00:27:49:cb:b2",
        "ip-address": "10.115.115.11"
      }
    ]
  }
}
```

The `dps-certificate-fingerprint` (optional - use for [2 option](#2-use-a-fingerprint-of-the-certificate-mtls)) refers to a certificate fingerprint used only during the TLS handshake with the DPS service at the `dps-endpoint`. It is utilized to verify the DPS certificate, where the DPS chain must contain one certificate with the same fingerprint as configured via DHCP. The fingerprint can be calculated from any certificate of the chain DPS service (intermediate or leaf certificate). The `dps-certificate-fingerprint-md-type` (not needed when `dps-certificate-fingerprint` is not set) indicates the type of hash that was used, such as SHA256 or SHA384.

After configuring the DHCP server, it is necessary to reload the configuration for the changes to take effect. This can be done by restarting the DHCP server using the following command:

```bash

sudo systemctl restart kea-dhcp4-server

```

### Obtaining the DPS service fingerprint (for [2 option](#2-use-a-fingerprint-of-the-certificate-mtls))

The following command returns the certificate chain in PEM format:

```bash

openssl s_client -connect dps.mycompany.com:5684 -showcerts

```

To configure a device for zero touch provisioning using DHCP, you need to obtain the fingerprint of one of the certificates in the chain. A fingerprint is a unique identifier that is computed from the contents of the certificate. In this particular example, the SHA256 message digest is used to compute the fingerprint.

```bash

openssl x509 -noout -fingerprint -sha256 -inform pem <<EOF
-----BEGIN CERTIFICATE-----
MIIBlzCCAT2g...
-----END CERTIFICATE-----
EOF

```

The output of the above command can be used as a fingerprint. The fingerprint and the message digest algorithm (in this case, SHA256) must be configured in the DHCP server so that the device can obtain the necessary configuration parameters during provisioning. Additionally, the device must be configured to allow the use of the configured message digest algorithm.

### Configuring isc-dhcp-client

On the client side, the configuration must be updated as well. If the client is using the ISC DHCP client, the configuration file is typically located at `/etc/dhcp/dhclient.conf`. To configure the client to obtain the necessary parameters for zero touch provisioning via DHCP, add the following lines to the configuration file:

```text

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

This will configure the client to send the vendor-class-identifier option with a value of `MY-COMPANY-DEVICE-SN-123456` to identify itself as a device from your company. It also configures the client to request the necessary parameters for zero touch provisioning from the DHCP server.

After making changes to the configuration files, it is necessary to restart the DHCP client for the changes to take effect or reboot the device.

With these configuration changes in place, the device should be able to obtain the necessary TLS settings and endpoint for zero touch provisioning via DHCP. This can simplify the device provisioning process, particularly for large deployments where manually configuring each device can be time-consuming and error-prone.

### Connect with client library

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

  plgd_dps_dhcp_set_values_t ret = plgd_dps_dhcp_set_values_from_vendor_encapsulated_options(dps_ctx, d.value, d.size);
  switch (ret) {
  case PLGD_DPS_DHCP_SET_VALUES_ERROR:
    DPS_ERR("pull vendor_encapsulated_options: error during update");
    return false;
  case PLGD_DPS_DHCP_SET_VALUES_NOT_CHANGED:
    DPS_DBG("pull vendor_encapsulated_options: not changed");
    break;
  case PLGD_DPS_DHCP_SET_VALUES_UPDATED:
    DPS_DBG("pull vendor_encapsulated_options: updated, but force re-provision is not needed");
    break;
  case PLGD_DPS_DHCP_SET_VALUES_NEED_REPROVISION: {
    DPS_DBG("pull vendor_encapsulated_options: updated, but needed to force re-provision and restart dps manager");
    plgd_dps_force_reprovision(dps_ctx);
    if (plgd_dps_manager_restart(dps_ctx)) {
      DPS_ERR("pull vendor_encapsulated_options: failed to restart dps manager");
      return false;
    }
  } break;
  default:
    DPS_ERR("pull vendor_encapsulated_options: unknown return value %d", ret);
    return false;
  }
  return true;
}

```
