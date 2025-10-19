/*
 * ZenithOS SDK - Header File
 *
 * Copyright (C) 2025 ne5link
 *
 * Licensed under the GNU General Public License v3.0 (GPLv3).
 * See <https://www.gnu.org/licenses/> for details.
 *
 * Made by ne5link <3
 */

#ifndef SAPI_H
#define SAPI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bn.h>
#include <unistd.h>

// === ZenithOS Secure API (SAPI) ===
//  SDK 13.0 • OpenSSL-based certificate generator
//  Created by ne5link 💜

#define SAPI_OK 0
#define SAPI_ERR 1

#define C_PURPLE  "\033[1;35m"
#define C_GREEN   "\033[1;32m"
#define C_RED     "\033[1;31m"
#define C_RESET   "\033[0m"

static void sapi_log(const char* msg) {
    FILE* log = fopen("cert.log", "a");
    if (log) {
        time_t t = time(NULL);
        fprintf(log, "[%ld] %s\n", t, msg);
        fclose(log);
    }
}

static EVP_PKEY* sapi_gen_rsa_key() {
    EVP_PKEY* pkey = EVP_PKEY_new();
    RSA* rsa = RSA_new();
    BIGNUM* bn = BN_new();
    BN_set_word(bn, RSA_F4);
    RSA_generate_key_ex(rsa, 2048, bn, NULL);
    EVP_PKEY_assign_RSA(pkey, rsa);
    BN_free(bn);
    sapi_log("RSA key generated (2048-bit)");
    return pkey;
}

static X509* sapi_gen_x509(EVP_PKEY* pkey, int days_valid, 
                           const char* country, 
                           const char* organization, 
                           const char* common_name) {
    X509* x509 = X509_new();
    ASN1_INTEGER_set(X509_get_serialNumber(x509), rand());
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 60L * 60 * 24 * days_valid);
    X509_set_pubkey(x509, pkey);

    X509_NAME* name = X509_get_subject_name(x509);
    if (country && strlen(country))
        X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char*)country, -1, -1, 0);
    if (organization && strlen(organization))
        X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char*)organization, -1, -1, 0);
    if (common_name && strlen(common_name))
        X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char*)common_name, -1, -1, 0);

    X509_set_issuer_name(x509, name);
    X509_sign(x509, pkey, EVP_sha256());
    sapi_log("X509 certificate signed (SHA256)");
    return x509;
}

static int sapi_generate_certificate(const char* filename,
                                     const char* country,
                                     const char* organization,
                                     const char* common_name,
                                     int days_valid,
                                     int quiet) {
    if (!quiet) printf(C_PURPLE "=== ZenithOS SAPI Certificate Generator ===\n" C_RESET);

    if (access(filename, F_OK) == 0) {
        if (!quiet) printf(C_RED "[!] File already exists: %s\n" C_RESET, filename);
        sapi_log("Aborted: file exists");
        return SAPI_ERR;
    }

    EVP_PKEY* pkey = sapi_gen_rsa_key();
    X509* x509 = sapi_gen_x509(pkey, days_valid, country, organization, common_name);

    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        EVP_PKEY_free(pkey);
        X509_free(x509);
        sapi_log("Error: failed to open file for writing");
        return SAPI_ERR;
    }

    PEM_write_PrivateKey(fp, pkey, NULL, NULL, 0, NULL, NULL);
    PEM_write_X509(fp, x509);
    fclose(fp);

    if (!quiet) {
        printf(C_GREEN "[+] Certificate and key saved to: %s\n" C_RESET, filename);
        printf("Valid for %d days.\n", days_valid);
    }

    sapi_log("Certificate successfully written");
    EVP_PKEY_free(pkey);
    X509_free(x509);
    return SAPI_OK;
}

#endif // SAPI_H

