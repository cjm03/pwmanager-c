// pwgen.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/core.h>
#include <openssl/rand.h>
#include <openssl/provider.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/bio.h>


char genCharacter(void)
{
    const char all[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$_-";
    int first = rand() % (sizeof(all) - 1);
    return all[first];
}

char genCharacterForDashed(void)
{
    const char all[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$_";
    int first = rand() % (sizeof(all) - 1);
    return all[first];
}

char* genSimplePassword(int len)
{
    char* pwd = malloc(len * sizeof(char) + 1);
    if (!pwd) {
        fprintf(stderr, "error: could not allocate for simple pwd\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < len; i++) {
        pwd[i] = genCharacter();
    }
    return pwd;
}

char* genDashedPassword(int len)
{
    int cut = len - (len % 6);
    int dashes = (cut / 6) - 1;
    char* pwd = malloc((cut + dashes) * sizeof(char) + 1);
    if (!pwd) {
        fprintf(stderr, "error: could not allocate for simple pwd\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < cut + dashes; i++) {
        if ((i + 1) % 7 == 0) {
            pwd[i] = '-';
        } else {
            pwd[i] = genCharacterForDashed();
        }
    }
    return pwd;
}

int genSalt(unsigned char* buffer, int bytes)
{
    int ret = RAND_priv_bytes(buffer, bytes);
    if (ret != 1) {
        fprintf(stderr, "RAND_priv_bytes: %lu\n", ERR_get_error());
        memset(buffer, 0x00, bytes);
        return -1;
    }
    return ret;
}

int sha256(char* pwd)
{
    EVP_MD_CTX* ctx = NULL;
    const EVP_MD* sha256 = NULL;
    unsigned int len = 0;
    unsigned char outdigest[EVP_MAX_MD_SIZE];
    sha256 = EVP_get_digestbyname("SHA3-256");
    if (sha256 == NULL) exit(EXIT_FAILURE);
    ctx = EVP_MD_CTX_new();
    if (ctx == NULL) exit(EXIT_FAILURE);
    EVP_DigestInit_ex2(ctx, sha256, NULL);
    EVP_DigestUpdate(ctx, pwd, strlen(pwd));
    EVP_DigestFinal_ex(ctx, outdigest, &len);
    EVP_MD_CTX_free(ctx);
    for (unsigned int i = 0; i < len; i++) {
        printf("%02x", outdigest[i]);
    }
    printf("\n");
    return 0;
}
// {
//     unsigned char* hash = malloc(SHA256_DIGEST_LENGTH * sizeof(unsigned char));
//
//     SHA256_CTX sha256;
//     SHA256_Init(&sha256);
//     SHA256_Update(&sha256, pwd, strlen(pwd));
//     SHA256_Final(hash, &sha256);
//
//     return hash;
// }
