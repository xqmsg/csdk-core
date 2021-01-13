//
//  delegate.h
//  xqcsdk
//
//  Created by Ike E on 10/20/20.
//

#ifndef delegate_h
#define delegate_h

/// A structure to contain delegate data.
struct xq_delegate_token {
    /// The delegate token data.
    char* data;
    /// The size of the delegate data in byes.
    int length;
};


/// Create a short-lived token for accessing resources.
///
/// @param config The XQ configuration instance.
/// @param result An object instance where results will be stored. The user can preallocate space for the token. If they do not, space will be allocated automatically, and must be cleaned up by the user.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_delegate_token (   struct xq_config* config,
                                struct xq_delegate_token* result,
                                struct xq_error_info* error  );


/// Destroys a delegate token. This can be used for tokens where the library allocated space for the token.
/// @param delegated The token to be destroyed.
void xq_destroy_delegate(struct xq_delegate_token* delegated);

#endif /* delegate_h */
