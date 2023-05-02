---
title: 'Securing device with TPM'
description: 'How TPM can enhance security with iotivity-lite?'
date: '2023-04-28'
categories: [features]
keywords: [features, security, tpm]
weight: 10
---

The TPM (Trusted Platform Module) is a microcontroller chip that provides hardware-based security for computer systems. It is used to securely store and manage cryptographic keys, including private keys for various types of certificates such as SSL/TLS certificates and IDevID certificates.

IDevID (Identity Device) certificates are digital certificates used to authenticate IoT devices. They contain a public key and a digital signature and are issued by a trusted Certificate Authority (CA). By storing the private key for IDevID certificates in TPM, the certificate's security and the device it authenticates are safeguarded against unauthorized access and malicious attacks.

## OCF Certificates and TPM in Device

The OCF (Open Connectivity Foundation) uses certificates to ensure secure communication between devices or services via mTLS. When TPM (Trusted Platform Module) is integrated, the private key is stored in the TPM, and the credential resource contains only the public certificate and a reference key to the private key for the TPM.

{{< note >}}

The default message digest algorithm allowed is `SHA256`. If you want to also allow `SHA384`, you can call the following function:

```c
oc_sec_certs_md_set_algorithms_allowed(
    MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA256) |
    MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA384));
```

Similarly, the default elliptic curve group allowed is `secp256r1`. If you want to also allow `secp384r1`, you can call the following function:

```c
oc_sec_certs_ecp_set_group_ids_allowed(
    MBEDTLS_X509_ID_FLAG(MBEDTLS_ECP_DP_SECP256R1) |
    MBEDTLS_X509_ID_FLAG(MBEDTLS_ECP_DP_SECP384R1));
```

{{< /note >}}

### OCF Manufacturer Certificate

The manufacturer certificate is used during ownership transfer, in which the device acts as a server with a manufacturer certificate, and a client certificate is required for mTLS. This certificate can also be used by the device to connect to another device, service, or cloud.

To meet the **requirements** for a manufacturer certificate:

- the extended key usage (EKU) must include `Server authentication` and `Client authentication` without `Any Extended Key Usage`
- the signature algorithm should be either `ecdsa-with-SHA256` or `ecdsa-with-SHA384`
- the elliptic curves must be `secp256r1` or `secp384r1`

### OCF Identity Certificate

The OCF identity certificate serves the purpose of identifying the device in the network and enabling it to connect as a client to other devices, services, or clouds. This certificate is used when the device is already provisioned with a signed identity certificate after ownership transfer, and it also allows the device to act as a server. If the device is factory reset, the certificate is deleted, and a new device UUID is created. To create this certificate, the device resource /oic/sec/csr provides a generated CSR (Certificate Signing Request).

To meet the **requirements** for an identity certificate

- the extended key usage (EKU) must include  `Server authentication` and `Client authentication` and `1.3.6.1.4.1.44924.1.6` without `Any Extended Key Usage`
- the signature algorithm should be either `ecdsa-with-SHA256` or `ecdsa-with-SHA384`
- the elliptic curves must be `secp256r1` or `secp384r1`
- the subject common name must be in the format of `uuid:<GUID>`, where GUID is the device UUID.

{{< note >}}

The default message digest signature algorithm for identity CSR is `SHA256`. However, if you need to use `SHA384`(need to be allowed by `oc_sec_certs_md_set_algorithms_allowed`), you can call the following function:

```c
oc_sec_certs_md_set_signature_algorithm(MBEDTLS_MD_SHA384);
```

Similarly, the default elliptic curve group for identity CSR is `secp256r1`. If you want to use `secp384r1`(need to be allowed by `oc_sec_certs_ecp_set_group_ids_allowed`), you can call the following function:

```c
oc_sec_certs_ecp_set_group_id(MBEDTLS_ECP_DP_SECP384R1);
```

{{< /note >}}

## TPM Integration

In OCF, certificates are typically stored in the credential resource `/oic/sec/cred` using PEM, DER, or RAW formats. If TPM is integrated, the private key is kept within the TPM, and the credential resource only holds the public certificate along with a reference key pointing to the private key in the TPM. This way, all private key-related functions are handled by the TPM, including data signing and encryption. As a **reference key**, either the public key or its hash can be used.

### IDevID Certificate as OCF Manufacturer Certificate

As a manufacturer certificate, an IDevID certificate signed by the certificate authority can be used. This certificate is stored in the credential resource `/oic/sec/cred` in PEM, DER, or RAW format. With TPM, the private key is stored in TPM, and the credential resource contains only the IDevID certificate and a IDevID reference key to the private key for TPM.
To store the IDevID certificate in the credential resource use call `oc_pki_add_mfg_cert` with the reference key instead of the private key.

### Generating OCF Identity Certificate

To generate the OCF identity certificate, a private key must first be generated in the TPM and associated with the reference key. The public key and reference key will then be stored in `/oic/sec/cred`. This private key is used by the TPM to sign the CSR (Certificate Signing Request), which is then sent to the certificate authority to generate the certificate. The CSR can be signed multiple times, for example, for certificate rotation. It is recommended to remove the private key when a factory reset is performed.

### Integration to iotivity-lite

To integrate TPM with iotivity-lite, the mbedtls library is used for cryptography. There are four main functions that need to be implemented:

1. **Generate public and private key**: The `mbedtls_pk_ecp_gen_key` function generates a private key for the CSR of the identity certificate when iotivity-lite doesn't have a private key in the keypair storage. In this function, a private key needs to be generated in the TPM, and the public key `mbedtls_pk_context` needs to be filled.
2. **Free private key in TPM**: The `pk_free_key` function frees the private key in the TPM. This function is called when a factory reset is performed or when the keypair storage does not match the TPM during the generation of the CSR with the reference key.
3. **Store reference key to credential resource**: The `mbedtls_pk_write_key_der` function writes the reference key to a buffer for the credential resource or keypair storage. A reference key associated with the private key in the TPM needs to be created according to `mbedtls_pk_context *pk`.
4. **Load public key with reference key**: The `mbedtls_pk_parse_key` function parses the public key/certificate and reference key from the keypair storage or credential resource. The reference key to the private key in the TPM, as written by `mbedtls_pk_write_key_der`, needs to be used as the key to set the public key `mbedtls_pk_context *pk` by the TPM chip.

To generate reference key from `mbedtls_pk_context *pk` you can use following function:

```c
#include <mbedtls/pk.h>
#include <mbedtls/sha256.h>
#include <stdint.h>

#define MD_SHA256_SIZE (32)

/**
 * @brief Gets the reference key as hash(SHA-256) for a given public key.
 *
 * @param pk Pointer to an mbedtls_pk_context struct containing the public key.
 * @param buf Pointer to the buffer to store the reference key.
 * @param size Size of the buffer.
 * @return The length of the reference key (32 bytes) on success, or -1 on failure.
 */
int get_reference_key(const mbedtls_pk_context *pk, uint8_t *buf, size_t size) {
    if (size < MD_SHA256_SIZE) {
        // buffer is too small
        return -1;
    }
    unsigned char pubkey[120];
    // Marshal pk to pubkey buffer. The marshaled public key is stored at the end of the pubkey buffer.
    int ret = mbedtls_pk_write_pubkey_der(pk, pubkey, sizeof(pubkey));
    if (ret < 0) {
        // error occurs
        return ret;
    }
    mbedtls_sha256(pubkey + size - ret, ret, buf, 0);
    return MD_SHA256_SIZE;
}
```

To make use of these TPM functions in iotivity-lite, you need to set the TPM implementations as `oc_pki_set_pk_functions` before calling `oc_main_init`.

```c

#include <iotivity-lite/oc_pki.h>

// Set TPM functions for PKI to iotivity-lite
oc_pki_pk_functions_t pk_functions = {
    .mbedtls_pk_ecp_gen_key = tpm_mbedtls_pk_ecp_gen_key,
    .pk_free_key = tpm_pk_free_key,
    .mbedtls_pk_parse_key = tpm_mbedtls_pk_parse_key,
    .mbedtls_pk_write_key_der = tpm_mbedtls_pk_write_key_der,
};
if (!oc_pki_set_pk_functions(&pk_functions)) {
  // Failed to set PKI function
  return -1;
}

```

{{< note >}}
An example that demonstrates how to integrate a simulated TPM with iotivity-lite can be found in the [iotivity-lite/cloud_server](https://github.com/iotivity/iotivity-lite/blob/master/apps/cloud_server.c).
{{< /note >}}
