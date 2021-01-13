//
//  combined.h
//  xqcsdk
//
//  Created by Ike E on 10/19/20.
//

#ifndef combined_h
#define combined_h

/// Data structure containing a combined token.
struct xq_combined_token {
    /// The combined token data. May need to be manually deallocated depending on how allocation occurs.
    char* data;
    /// The length of the combined token.
    int length;
    /// The number of tokens that were successfully merged in order to create the combind token.
    short merged;
};


/// Takes a list of valid tokens and creates a single combined one.
///
/// Combining tokens is useful in situations where a user is signed on with multiple accounts ( and so has multiple access tokens), and want to read messags from
/// any one of them without needing to lookup keys for each of them ( in situations where a message could have been sent to any one of their accounts ).
///
/// In this situation they can first combine all their access tokens with this call, and then look up the key using the combined token.
///
/// @param config The XQ configuration instance.
/// @param tokens The list of tokens to combine. These should all be valid tokens - Invalid tokens will be ignored by the server.
/// @param token_count The number of tokens to combine
/// @param result An object instance where results will be stored. The user can preallocate space for the token. If they do not, space will be allocated automatically, and must be cleaned up by the user.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_combine_tokens (   struct xq_config* config,
                                const char** tokens,
                                int token_count,
                                struct xq_combined_token* result,
                                struct xq_error_info* error  );


/// Destroys a combined token. This can be used for tokens where the library allocated space for the token.
/// @param combined The token to be destroyed.
void xq_destroy_combined(struct xq_combined_token* combined);
#endif /* combined_h */
