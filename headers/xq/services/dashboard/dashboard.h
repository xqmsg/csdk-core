//
//  dashboard.h
//  xqcsdk
//
//  Created by Ike E on 10/21/20.
//

#ifndef dashboard_h
#define dashboard_h


/// Connects a currenlty authorized XQ user to their management account.
///
/// @param config The XQ configuration instance.
/// @param team_id The numerical ID of the team to connect to. If set to zero, a random dashboard will be set.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_connect_dashboard( struct xq_config* config,
                        long team_id, struct
                        xq_error_info* error );

#endif /* dashboard_h */
