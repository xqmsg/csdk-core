//
//  nist_decrypt.h
//  xqcsdk
//
//  Created by Ike E on 10/22/20.
//

#ifndef nist_decrypt_h
#define nist_decrypt_h


/// Encrypts the provided data using the specified NIST algorithm. The algorithm specified must be available as dynamic library (*.so) to
/// the application before it can be used.
///
/// @param nist_library_name The name of the NIST library to use for encryption.
/// @param data The data to decrypt.
/// @param data_len The number of bytes of data.
/// @param key The secret encryption key.
/// @param result A reference to the message block where the decryption result will be stored.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_nist_decrypt(  char* nist_library_name,
                        uint8_t* data,
                        size_t data_len,
                        char* key,
                        struct xq_message_payload* result,
                        struct xq_error_info* error   );


#endif /* nist_decrypt_h */
