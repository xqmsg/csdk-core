//
//  aes_decrypt.h
//  xqcsdk
//
//  Created by Ike E on 10/19/20.
//

#ifndef aes_decrypt_h
#define aes_decrypt_h

/// Decrypts the provided data using the specified AES algorithm.  The type of algorithm to use will be automatically detected from the secret key prefix ( either ".A" for normal, or ".D" for strong ).
///  If no prefix is detected, it will attempt to decrypt using Algorithm_AES.
///
/// @param data The data to encrypt.
/// @param data_len The number of bytes of data.
/// @param key The secret encryption key.
/// @param result A reference to the message block where the decryption result will be stored.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_aes_decrypt(   uint8_t* data,
                        size_t data_len,
                        char* key,
                        struct xq_message_payload* result,
                        struct xq_error_info* error   );


#endif /* aes_decrypt_h */
