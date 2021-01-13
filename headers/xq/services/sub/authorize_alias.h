//
//  authorize_now.h
//  xqcsdk
//
//  Created by Ike E on 10/15/20.
//

#ifndef authorize_now_h
#define authorize_now_h

/// Authorizes an XQ user without 2-Factor authentication.
///
/// The difference between this and the standard authorization service is that only predetermined TLD's ( including .local ) will be allowed to authenticate this way.
/// As long as one of those TLD's is used, it will always return a usable token.\n
///
/// Warning: This should **only** used in situations where encryption is the only goal, and validating a sender/recipient
/// is completely unimportant.
///
/// @param config The XQ configuration instance.
/// @param email The email address off the user to authorize. This must have a supported TLD. If this is just the user part of the email, the server may fill in a TLD if one is set by the API key.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_authorize_alias(   struct xq_config* config,
                                const char* email,
                                struct xq_error_info* error);

#endif /* authorize_now_h */
