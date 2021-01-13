//
//  Header.h
//  xqcsdk
//
//  Created by Ike E on 10/19/20.
//

#ifndef Header_h
#define Header_h


/// Encrypts the provided data using the specified AES algorithm.  At the moment, this must be either Algorithm_AES (allowing intercompatibility with other XQ products),
///  or Algorithm_AES_Stong ( no intercompatibility currently, but using the more secure SHA-2 for key hashing ).
///
/// @param data The data to encrypt.
/// @param data_len The number of bytes of data.
/// @param key The secret encryption key.
/// @param result A reference to the message block where the encryption result will be stored.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_aes_encrypt(
                        uint8_t* data, size_t data_len,
                        char* key,
                        struct xq_message_payload* result,
                        struct xq_error_info* error   );

#endif /* Header_h */
