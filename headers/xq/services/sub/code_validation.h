//
//  code_validation.h
//  xqcsdk
//
//  Created by Ike E on 10/15/20.
//

#ifndef code_validation_h
#define code_validation_h


/// This service should be used to validate an account directly ( instead of using the confirmation link).
///
/// The user simply passes in the PIN number found in their confirmation email, and the library does the rest ( including performing the
/// access token exchange if the PIN is successfully validated ).
///
/// @param config The XQ configuration instance.
/// @param pin The PIN number from the confirmation email or text message.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_code_validation(   struct xq_config* config,
                                const char* pin,
                                struct xq_error_info* error );

#endif /* code_validation_h */
