//
//  aes_encrypt.c
//  xqc
//
//  Created by Ike E on 10/21/20.
//

#include <stdio.h>
#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <xq/config.h>
#include <xq/services/quantum/quantum.h>
#include <xq/services/crypto.h>
#include <xq/algorithms/aes/aes_encrypt.h>

_Bool xq_aes_encrypt(

                     uint8_t* data, size_t data_len,
                     char* key,
                     struct xq_message_payload* result,
                     struct xq_error_info* error   ) {
    
    /* "opaque" encryption, decryption ctx structures that libcrypto uses to record
     status of enc/dec operations */
    EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
    if (!en) {
        ERR_print_errors_fp(stderr);
        return 0;
    }
    
    

    
    /* 8 bytes to salt the key_data during key generation. This is an example of
     compiled in salt. We just read the bit pattern created by these two 4 byte
     integers on the stack as 64 bits of contigous salt material -
     ofcourse this only works if sizeof(int) >= 4 */
    int i;
    
    uint8_t salt[8] = {0};
    for (  i = 0; i < 8 ; ++i ) {
        salt[i] = 48 + ((uint8_t) rand() % 74);
    }

    
    _Bool compat = (key[0] == '.' && key[1] == 'A' );
    if (key[0] == '.') key += 2;
    
    int key_data_len = (int)strlen(key);
    int len = (int) data_len + AES_BLOCK_SIZE;
    unsigned char gen_key[32] = {0}, gen_iv[32] = {0};
    int padding = (!compat)? AES_PADDING : 0;
    
    if (!compat) {
        EVP_CIPHER_CTX_set_padding(en, AES_PADDING);
    }

    
    /*
     * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
     * nrounds is the number of times the we hash the material. More rounds are more secure but
     * slower.
     */
    i = EVP_BytesToKey(EVP_aes_256_cbc(), (compat) ? EVP_md5() : STRONG_HASH() , salt, (unsigned char*)key, key_data_len, (compat) ? 1 : AES_ROUNDS, gen_key, gen_iv);
   
    if (i != 32) {
        fprintf(stderr, "Key size is %d bits - should be 256 bits\n", i);
        EVP_CIPHER_CTX_free(en);
        return -1;
    }
    
    
    if (!EVP_EncryptInit_ex(en, EVP_aes_256_cbc(), NULL, gen_key, gen_iv)){
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(en);
        return -1;
    }
    

    /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
    int c_len = len;
    int max_buf_len = (((len + 16)/16) * 16);
    len = 0;
     
    result->data = calloc( 1, max_buf_len + 16 + padding );
    uint8_t* pos = result->data + 16;
    
    /* update ciphertext, c_len is filled with the length of ciphertext generated,
     *len is the size of plaintext in bytes */
    if (!EVP_EncryptUpdate(en, pos , &len, data, c_len )){
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(en);
        return -1;
    }
    c_len = len;
    
    
    int f_len = 0;
    
    if(padding > 0 && !EVP_EncryptFinal_ex(en,  pos + c_len,   &f_len)){
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(en);
        return -1;
    }
    
    memccpy(result->data, "Salted__", '\0', 8);
    memccpy(result->data + 8, salt , '\0', 8);
    result->length = f_len + c_len + 16;

    EVP_CIPHER_CTX_free(en);

    return 1;
    
}
