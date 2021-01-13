//
//  expiration.h
//  xqcsdk
//
//  Created by Ike E on 10/21/20.
//

#ifndef expiration_h
#define expiration_h


/// Check whether a particular key has expired or not.
///
/// @param config The XQ configuration instance.
/// @param token The token used to locate the key.
/// @param secondsLeft A pointer to a user-defined long instance. If provided, it will be filled with the remaining seconds that the key has before expiration.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_check_expiration(  struct xq_config* config,
                                char* token,
                                long* secondsLeft,
                                struct xq_error_info* error  );

#endif /* expiration_h */
