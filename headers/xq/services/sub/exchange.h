//
//  exchange.h
//  xqcsdk
//
//  Created by Ike E on 10/16/20.
//

#ifndef exchange_h
#define exchange_h

/// Exchanges the provided token with a valid access token.
/// This function should only be called after a user has validated their account, either by clicking the confirmation message in their
/// email, or by using the **code validation** endpoint. The access token can then be read using xq_get_access_token, or a normal JSON reader.
///
/// @param config The XQ configuration object.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_exchange(  struct xq_config* config,
                        struct xq_error_info* error
                      );

#endif /* exchange_h */
