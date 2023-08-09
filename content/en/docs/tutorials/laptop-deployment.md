---
title: 'Setting Up Plgd Hub and Plgd DPS in a Dynamic IP Environment (DHCP)'
description: 'How to setting up Plgd Hub and Plgd DPS in a Dynamic IP Environment (DHCP)'
docsOthersDisplay: true
date: '2023-08-09'
categories: [tutorials, deployment, advanced]
keywords: [tutorials, dhcp, laptop]
weight: 1
---

In scenarios where IP addresses can change, such as with DHCP, you'll need to configure plgd hub and plgd dps (Device Provisioning Service) using both a dynamic and a static IP address.

1. **Configuring the System:**
  To accommodate both dynamic and static IP addresses, follow these steps:

   - Add an Additional IP (Secondary IP) to the Ubuntu System. This can be achieved by referring to guides such as:
     [How To Add Additional IP (Secondary IP) In Ubuntu System](https://www.2daygeek.com/add-secondary-ip-address-on-network-interface-linux/)
     [Ask Ubuntu - Does netplan support DHCP and static addresses on one interface?](https://askubuntu.com/questions/1183406/does-netplan-support-dhcp-and-static-addresses-on-one-interface)
   - Assign a static IP address, for example: `10.254.254.1/32`
   - Update the `/etc/hosts` file with domains pointing to the static IP address:

     ```plaintext
     10.254.254.1 demo.mycompany.com auth.demo.mycompany.com api.demo.mycompany.com
     ```

2. **Installing and Configuring DNS Server:**

    Install a DNS server, such as `dnsmasq`, to serve domains to MicroK8s. Make this configuration permanent for reboots. Allow the server to listen on `10.254.254.1`.

3. **Configuring MicroK8s to Use Custom DNS:**

    Configure MicroK8s to utilize the custom DNS server with IP `10.254.254.1` by following the MicroK8s documentation for adding custom DNS as an add-on.

4. **Updating Helm Values for PLGD Services:**

    Adjust the Helm values YAML for PLGD services to utilize the domain and the DHCP assigned IP address. Modify the `values.yaml` with the following settings:

    ```yaml
    global:
      domain: demo.mycompany.com
    coapgateway:
      apis:
        coap:
          externalAddress: <DHCP_IP:PORT>
    resourcedirectory:
      publicConfiguration:
        coapGateway: coaps+tcp://<DHCP_IP:PORT>
    deviceProvisioningService:
      enrollmentGroups:
        - id:
            hub:
              coapGateway: <DHCP_IP:PORT>
    ```

    Replace `<DHCP_IP:PORT>` with the actual DHCP-assigned IP address and port.

5. **Sustaining IP Address Changes:**

    After changing the IP address, you only need to update the `values.yaml` in Helm for the changes to take effect. Then the coap-gateway, resource-directory, and device-provisioning-service pods will be restarted to load configuration.

6. **Accessing the Services:**

    Ensure that any laptop attempting to access `https://demo.mycompany.com` has the updated `/etc/hosts` file pointing to the new DHCP-assigned IP address.
