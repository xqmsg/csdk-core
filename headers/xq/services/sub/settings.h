//
//  settings.h
//  xqcsdk
//
//  Created by Ike E on 10/17/20.
//

#ifndef settings_h
#define settings_h

/// Enumeration of notification receipt types.
enum xq_notification_level {
    /// The user will not receive any usage emails or tutorials.
    no_notifications = 0,
    /// The user will only receive emails related to their usage.
    usage_notifications = 1,
    /// The user will only receive tutorial emails on how to use XQ.
    tutorial_notifications = 2,
    /// The user will receive all notifications.
    all_notifications = 3
};


/// Specifies whether an integer is a valid notification level or not.
/// @param level Thei integer to test.
static inline _Bool is_valid_notification(int level) {
    switch ( level ) {
        case no_notifications:
        case usage_notifications:
        case tutorial_notifications:
        case all_notifications:
            return 1;
        default:
            return 0;
    }
}

/// Used for storing settings data fetched from the server.
struct xq_settings_info {
    /// The notification level for the current user.
    enum xq_notification_level notifications;
    /// Specifies whether the user wants to subscribe to other newsletters or not.
    _Bool newsletter;
};


/// Fetches the notification and newsletter settings for the currently signed-in user.
/// @param config The XQ configuration object.
/// @param settings The settings info instance where the results will be stored.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_get_settings(  struct xq_config* config,
                            struct xq_settings_info* settings,
                            struct xq_error_info* error  );



/// Updates the notification and newsletter settings for the currently signed-in user.
/// @param config The XQ configuration object.
/// /// @param settings The settings info instance containing the updated values.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool  xq_svc_save_settings(    struct xq_config* config,
                                struct xq_settings_info* settings,
                                struct xq_error_info* error );

#endif /* settings_h */
