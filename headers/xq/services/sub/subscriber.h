//
//  subscriber.h
//  xqcsdk
//
//  Created by Ike E on 10/17/20.
//

#ifndef subscriber_h
#define subscriber_h

/// Enumeration of all possible subscriber status values.
enum subscription_status {
    Status_Unregistered = 0,
    ///The user has been validated..
    Status_Validated = 1,
    /// The user's subscription has expired.
    Status_Expired = 3,
    ///  The user's account needs validation.
    Status_NewAccountValidationRequired = 4,
    /// The user's account needs to have a password set ( usually after revalidation).
    Status_PasswordRequired = 5,
    /// The user's account needs validation.
    Status_OldAccountValidationRequired = 6
    
};

/// Gets a string representation of a subscriber status value.
/// @param stat The subscriber status to check.
static inline const char* xq_status_string(enum subscription_status stat ){
    switch (stat) {
        case Status_Unregistered: return "Unregistered";
        case Status_Validated: return "Validated";
        case Status_Expired: return "Expired";
        case Status_NewAccountValidationRequired: return "New Account Needs Validation";
        case Status_PasswordRequired: return "Password Change Required";
        case Status_OldAccountValidationRequired: return "Unvalidated Account";
        default: return "Unknown";
    }
}


/// Manages subscriber information that is fetched from the server.
struct xq_subscriber_info {
    /// The subscribers first name.
    char firstName[40];
    /// The subscribers last name
    char lastName[40];
    /// The registered email or phone number.
    char mailOrPhone[100];
    /// The current status of the subscriber
    enum subscription_status status;
    /// The subscribers join date ( in milliseconds ).
    long startedOn;
    /// The date when the subscription will end ( in milliseconds ).
    long endsOn;
};


/// Fetches subscription information about the current signed-in user.
/// @param config The XQ configuration instance.
/// @param info A subscriber_info instance where the results will be stored.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_get_subscriber(    struct xq_config* config,
                                struct xq_subscriber_info* info,
                                struct xq_error_info* error  );



/// Removes the currently authenticated user from XQ.
///
/// This will completely remove the current user from XQ ( meaning any other device that use the same account will no longer work ).
/// To simply deauthorize the current device, use xq_svc_deauthorize instead.
///
/// @param config The XQ configuration instance.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool  xq_svc_delete_subscriber( struct xq_config* config, struct xq_error_info* error  );


#endif /* subscriber_h */
