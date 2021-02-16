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
/// @param metadata Additional metadata used for categorizing this key in the dashboard.
/// @param result The result object where the key will be stored. The user may preallocate space if desired. Othewise, the library will allocate memory automatically ( which the user must clean up when done ).
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_store_key(
                       struct xq_config* config,
                       struct xq_message_token_request* request,
                       struct xq_metadata* metadata,
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


/// Creates a new e-mail metadata object. Entries will appear in the dashboard communication section.
/// @param subject The email subject line.
struct xq_metadata xq_new_email_metadata( const char* subject );



/// Creates a new file metadata object. Entries will appear in the dashboard communication section.
/// @param filename The file name
/// @param mimeType The mime type of the file
/// @param filesize The size of the file in bytes
struct xq_metadata xq_new_file_metadata( const char* filename,
                           const char* mimeType,
                           long filesize);



/// Aliases a data metadata object. This should be used for entries that the user does not want to appear in the
/// dashboard communication section ( event logs will still be available ).
/// @param type The metadata type .
/// @param content The raw json meta content.
struct xq_metadata xq_use_metadata( enum metadata_type type, const char* content );



/// Destroys a metadata object. This should always be called for metadata objects created with
/// one of the xq_new metadata calls ( this does not include xq_use_metadata, which has its lifecycle fully controlled by the user  ).
/// @param metadata The metadata object to destroy.
void xq_destroy_metadata(struct xq_metadata* metadata);


#endif /* packet_h */
