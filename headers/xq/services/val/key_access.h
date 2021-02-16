//
//  key_access_h
//  xqcsdk
//
//  Created by Ike E on 10/15/20.
//

#ifndef key_access_h
#define key_access_h

/// Grants a user acces to an encryption key that was previously generated. The user making this
/// request must be the same one that generated the key.
///
/// @param config The XQ configuration instance.
/// @param token The encryption key token.
/// @param recipients A list of user accounts (email address/phone) to grant access.
/// @param recipient_count The number of recipients to add.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_grant_access( struct xq_config* config,
                          char* token,
                          const char** recipients,
                          int recipient_count,
                          struct xq_error_info* error );


/// Revokes a user access to an encryption key. The user making this
/// request must be the same one that generated the key.
///
/// @param config The XQ configuration instance.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_revoke_access(   struct xq_config* config,
                           char* token,
                           const char** recipients,
                           int recipient_count,
                           struct xq_error_info* error );

#endif /* key_access_h */
