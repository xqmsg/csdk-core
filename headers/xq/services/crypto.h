//
//  encrypt.h
//  xqcsdk
//
//  Created by Ike E on 10/18/20.
//

#ifndef encrypt_h
#define encrypt_h

#define STRONG_HASH EVP_sha1
#define AES_ROUNDS 1
#define WITH_GENERIC_SALT 1

enum algorithm_type {
    /// This should only be used during a decryption phase. Automatically decide which algorithm to used based on its key prefix.
    Algorithm_Autodetect = 0,
    /// OTP (XOR)
    Algorithm_OTP = 1,
    /// Advanced Encryption Standard (AES)
    Algorithm_AES = 2,
    /// 5-Pass Advanced Encryption Standard (AES) with SHA-2 key encoding.
    Algorithm_AES_Strong = 3,
    /// NIST Algorithm
    Algorithm_NIST = 4,
};


/// Check whether an integer is a valid algorithm type or not.
/// @param algo The integer to check.
static inline _Bool is_valid_algorithm(int algo) {
    switch ( algo ) {
        case Algorithm_OTP:
        case Algorithm_AES:
        case Algorithm_AES_Strong:
        case Algorithm_NIST:
            return 1;
        default:
            return 0;
    }
}

/// A  generic message payload structure.
struct xq_message_payload {
    /// The payload content.
    uint8_t* data;
    /// The size of the content in bytes.
    uint32_t length;
    /// The token or key associated with this payload.
    char* token_or_key;
};

/// A generic data  structure for storing hex-encoded quantum key.
struct xq_hex_quantum_key {
    /// The data content.
    char* data;
    /// the size of the content in bytes.
    uint32_t length;
};


/// Encrypts data using the specified algorithm.
///
/// This service will fetch quantum data and use that to generate a secret key. If encryption is successful, it will fill the result object with
/// the newly encrypted data, and the secret key used.
///
/// @param config The XQ configuration instance.
/// @param algorithm The algorithm to use.
/// @param data The data to encrypt.
/// @param data_len The size of the data in bytes.
/// @param entropy_size The amount of quantum entropy to use for the key.
/// @param pool  An optional quantum pool to use  for entropy.
/// @param result The user-provided result instance. The data storage may be preallocated. Otherwise, it will be allocated automatically ( the user must manually clean up after usage).
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_encrypt(   struct xq_config* config,
                    enum algorithm_type algorithm,
                    uint8_t* data, size_t data_len,
                    int entropy_size,
                    struct xq_quantum_pool* pool,
                    struct xq_message_payload* result,
                    struct xq_error_info* error   );



/// Encrypts data using the specified algorithm, and save the key on XQ in return for a token.
///
/// This method will automate a few steps for you. It will :
///
/// 1. Fetch quantum data and use that to generate a secret key.
///
/// 2. Encrypt using the specified algorithm.
///
/// 3. Save the secret key to XQ in return for a token.
///
/// 4. Fill out the result instance with the encrypted content and the token ( not the secret key ).
///
/// @param config The XQ configuration instance.
/// @param algorithm The algorithm to use.
/// @param data The data to encrypt
/// @param data_len The size of the data in bytes.
/// @param entropy_size The amount of quantum entropy to use for the key.
/// @param pool  An optional quantum pool to use  for entropy.
/// @param recipients A list of the recipients.
/// @param recipients_len The number of recipients.
/// @param hours_to_expiration The number of hours before this message should expire.
/// @param delete_on_read Should this message be deleted after it is read?
/// @param result The user-provided result instance. The data storage may be preallocated. Otherwise, it will be allocated automatically ( the user must manually clean up after usage).
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_encrypt_and_store_token(
                                 struct xq_config* config,
                                 enum algorithm_type algorithm,
                                 uint8_t* data, size_t data_len,
                                 int entropy_size,
                                 struct xq_quantum_pool* pool,
                                 const char* recipients,
                                 int hours_to_expiration,
                                 _Bool delete_on_read,
                                 struct xq_message_payload* result,
                                 struct xq_error_info* error   );


/// Decrypts a message using the secret key associated with the provided token.
///
/// This method will attempt to get the secret key using the token. This will only be possible if the user is a valid recipient of the message, and the key is unexpired
/// and unrevoked. Once the key is acquired, the message will be decrypted.
///
/// @param config The XQ configuration instance.
/// @param algorithm The algorithm to use.
/// @param data The raw encrypted data.
/// @param data_len The length of the encrypted data in bytes.
/// @param token The message token. This will be used to retrieve the secret key.
/// @param result The user-provided result instance. The data storage may be preallocated. Otherwise, it will be allocated automatically ( the user must manually clean up after usage).
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_decrypt_with_token(    struct xq_config* config,
                                enum algorithm_type algorithm,
                                uint8_t* data,
                                size_t data_len,
                                char* token,
                                struct xq_message_payload* result,
                                struct xq_error_info* error   );


/// Decrypts a message using the provided secret key.
///
/// @param config The XQ configuration instance.
/// @param algorithm The algorithm to use.
/// @param data The raw encrypted data.
/// @param data_len The length of the encrypted data in bytes.
/// @param key The secret key.
/// @param result The user-provided result instance. The data storage may be preallocated. Otherwise, it will be allocated automatically ( the user must manually clean up after usage).
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_decrypt_with_key(  struct xq_config* config,
                            enum algorithm_type algorithm,
                            uint8_t* data,
                            size_t data_len,
                            char* key,
                            struct xq_message_payload* result,
                            struct xq_error_info* error   );


/// Expands the length of a key.
///
/// In some instances it will be inefficient or impossible to get the number of quantum bits to create a key of a desired length. As an alternative, you may fetch a smaller number of keys, and
/// generate a longer key from those using smart shuffling.
/// @param in_key The original key.
/// @param out_key A pointer to a buffer where the expanded key will be stored.
/// @param new_len The size of the expanded key buffer ( in bytes ). If the method is successful, the buffer will be filled by this number of bytes.
_Bool xq_expand_key( char* in_key, char* out_key, int new_len );


/// Converts a sequence of character bits to bytes.
///
/// @param bits_in The bit sequence
/// @param in_len the length of the bit sequence.
/// @param hex_out A pointer to a buffer that will contain the hex sequence.
/// @param out_len The length of the hex buffer.
void xq_bits_to_hex( char* bits_in, int in_len, char* hex_out, int out_len );




/// Convert a string of character encoded bit key to hex format.
/// @param key_in The key to convert
/// @param hex_out The object where the output will be stored.
/// @param prefix A prefix to attach to the key after conversion. This can be used to store the type of encryption that the key will be used for.
_Bool xq_key_to_hex( struct xq_quantum_key* key_in, struct xq_hex_quantum_key* hex_out, char prefix );


/// Destroy a quantum key object.
/// @param obj The object to destroy.
void xq_destroy_quantum_key(struct xq_quantum_key* obj);


/// Destroy a hex quantum object.
/// @param obj The object to destroy.
void xq_destroy_hex_quantum_key(struct xq_hex_quantum_key* obj);


/// Destroy a message payload  object.
/// @param obj The object to destroy.
void xq_destroy_payload(struct xq_message_payload* obj);


/// Convert a raw payload to a base64 one (usually for display or textual transfer ).
/// @param in The payload to convert.
/// @param out The resulting payload.
_Bool xq_base64_payload(struct xq_message_payload* in, struct xq_message_payload* out);

#endif /* encrypt_h */
