//
//  packet.h
//  xqcsdk
//
//  Created by Ike E on 10/20/20.
//

#ifndef packet_h
#define packet_h

/// A data structure containing all the information required to store a new secret key on the server.
struct xq_message_token_request {
    /// The secret key to store.
    char* key;
    /// The length of the secret key.
    int key_len;
    /// The list of recipients who can access the key.
    const char* recipients;
    /// The number of hours before this message expires.
    int expiration;
    /// Should this message be deleted as soon as a recipient reads it?
    _Bool delete_on_read;
    /// A user-defined type tag to specify what type of data is being referred to ( e.g. "msg", "file" ).
    const char* type_tag;
};

/// A structure for the message token.
struct xq_message_token {
    char* data;
    int length;
};

/// A structure for the XQ secret key
struct xq_key {
    char* data;
    int length;
};

/// Stores a new secret key with XQ and receive a token in return.
/// @param config The XQ configuration instance.
/// @param request The request object containing all the data required (key, recipients, etc ).
/// @param result The result object where the key will be stored. The user may preallocate space if desired. Othewise, the library will allocate memory automatically ( which the user must clean up when done ).
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_store_key(
                      struct xq_config* config,
                      struct xq_message_token_request* request,
                      struct xq_message_token* result,
                      struct xq_error_info* error  );


/// Fetches a secret key from XQ using the token.
///
/// A user will only be able to retrieve a secret key if they are listed as one of its recipients. Otherwise this call will fail.
///
/// @param config The XQ configuration instance.
/// @param token The token used to locate the key.
/// @param key_out The key object where the result should be stored. The user may preallocate space if desired. Othewise, the library will allocate memory automatically ( which the user must clean up when done ).
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_get_key(
                       struct xq_config* config,
                       char* token,
                       struct xq_key* key_out,
                       struct xq_error_info* error  );


/// Revokes a key using the token. Once a token has been revoked, ithe key will destroyed, making the message unreadable from that point on.
///
/// A user will only be able to revoke a  key if they are the one that sent it originally. Otherwise this call will fail.
///
/// @param config The XQ configuration instance.
/// @param token The token used to locate the key.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_remove_key(
                     struct xq_config* config,
                     char* token,
                     struct xq_error_info* error  );


#endif /* packet_h */
