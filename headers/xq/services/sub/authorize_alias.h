//
//  authorize_now.h
//  xqcsdk
//
//  Created by Ike E on 10/15/20.
//

#ifndef authorize_now_h
#define authorize_now_h

/// Authorizes a tracked XQ user without 2-Factor authentication.
///
/// The difference between this and the standard authorization service is that only predetermined TLD's ( including .local ) will be allowed to authenticate this way.
/// As long as one of those TLD's is used, it will always return a usable token.\n
///
/// Warning: This should **only** used in situations where encryption is the only goal, and validating a sender/recipient
/// is completely unimportant.
///
/// @param config The XQ configuration instance.
/// @param identifier The contact identifier ( e.g. name, external ID, etc ).
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_authorize_alias(   struct xq_config* config,
                                const char* identifier,
                                struct xq_error_info* error);



#endif /* authorize_now_h */
