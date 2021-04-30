//
//  group.h
//  xqcsdk
//
//  Created by Ike E on 10/15/20.
//

#ifndef xq_group_h
#define xq_group_h


struct xq_user_group {
    long group_id;
    char* alias;
    char** members;
    int member_count;
};


/// Creates a new mailing group.
///
/// @param config The XQ configuration instance.
/// @param group The mailing group to add
/// @param error An optional, user-provided block  to store details of any error that occurs.
/// @return The new group ID if successful, otherwise zero.
long xq_add_user_group( struct xq_config* config,
                        struct  xq_user_group* group,
                        struct xq_error_info* error );

/// Deletes an existing mailing group.
///
/// @param config The XQ configuration instance.
/// @param group_id The ID of the mailing group to remove.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_remove_user_group( struct xq_config* config,
                        long group_id,
                        struct xq_error_info* error );

/// Changes the details of an existing mailing group.
///
/// @param config The XQ configuration instance.
/// @param group The mailing group to update. The ID must be that of an existing group.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_update_user_group( struct xq_config* config,
                        struct  xq_user_group* group,
                        struct xq_error_info* error );



#endif /* xq_group_h */
