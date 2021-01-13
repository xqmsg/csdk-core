//
//  authorize.h
//  xqcsdk
//
//  Created by Ike E on 10/15/20.
//

#ifndef authorize_h
#define authorize_h


/// Authorizes an XQ user with 2-Factor authentication. After making this call, a confirmation link and code will be sent to the user.
/// They can then confirm their account using one of the following two methods:
/// 1. Call the xq_svc_codevalidation service, and provide the PIN code from the email.
/// 2. Click on the link the email, and then call the xq_svc_exchange service.
///  Both services ( if successful ) will yield an access token that the SDK will use to make authenticated calls from that point on.
///
/// @param config The XQ configuration instance.
/// @param email_or_phone The email address  (or phone) of the user to authorize.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_authorize( struct xq_config* config,
                        const char* email_or_phone, struct
                        xq_error_info* error );


/// Deauthorizes a user. This clears the authorization for the current user on both the backend and the SDK. As a result, the user will
/// be automatically be logged out, and will need to authorize again in order to make authenticated calls.
///
/// @param config The XQ configuration instance.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_deauthorize(   struct xq_config* config,
                            struct xq_error_info* error );

#endif /* authorize_h */
