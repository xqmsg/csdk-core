//
//  contact_id.h
//  xqcsdk
//
//  Created by Ike E on 10/21/20.
//

#ifndef contact_id_h
#define contact_id_h



/// Adds a new  contact to the users management portal.
///
/// @param config The XQ configuration instance.
/// @param identifier Th contact identifier. Must be an email or phone number  for  anything other than an alias role.
/// @param first_name The first name of the contact
/// @param last_name The last name of the contact
/// @param title The business title of the contact
/// @param role The business role of the contact. For numerical aliases, you can use xq_add_contact_alias instead.
/// @param error An optional, user-provided block  to store details of any error that occurs.
/// @returns The internal contact ID if the request succeeds. Otherwise, zero.
long xq_add_contact( struct xq_config* config,
                            const char* identifier,
                            const char* first_name,
                            const char* last_name,
                            const char* title,
                            enum xq_business_role role,
                            enum xq_notification_level notification_level,
                            struct xq_error_info* error );

/// Removes a previously addded external contact.
///
/// @param config The XQ configuration instance.
/// @param team_id The numerical ID of the team to connect to. If set to zero, a random dashboard will be set.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_remove_contact( struct xq_config* config,
                        long internal_contact_id,
                              struct
                        xq_error_info* error );

/// Disables a previously addded external contact.
///
/// @param config The XQ configuration instance.
/// @param team_id The numerical ID of the team to connect to. If set to zero, a random dashboard will be set.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_disable_contact( struct xq_config* config,
                        long internal_contact_id,
                              struct
                        xq_error_info* error );

#endif /* contact_id_h */
