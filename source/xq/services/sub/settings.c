//
//  settings.c
//  xqc
//
//  Created by Ike E on 10/18/20.
//

#include <stdio.h>
#include <xq/config.h>
#include <xq/connect.h>
#include <ext/jwrite/jWrite.h>
#include <ext/jread/jRead.h>
#include <xq/services/sub/settings.h>


_Bool xq_svc_get_settings( struct xq_config* config, struct xq_settings_info* settings, struct xq_error_info* error  ) {
    
    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Get, "settings", 0 , 1,  0 );

    if ( response.success ) {
        if (settings ) {
            settings->notifications = jRead_int( (char*) response.content, "{'notifications'", 0 );
            settings->newsletter = jRead_int( (char*) response.content, "{'newsletter'", 0 );
        }
    }
    
    else if ( error ) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;
}


_Bool  xq_svc_save_settings( struct xq_config* config, struct xq_settings_info* settings, struct xq_error_info* error ) {
    
    if (settings == 0) {
        if (error) {
            xq_strcat(error->content, "No settings object was provided" , MAX_ERROR_LENGTH);
        }
        return 0;
    }
    
    // We'll store our results in this buffer.
    char buf[40] = {0};
    struct jWriteControl jwc;
    jwOpen(&jwc, buf, 40, JW_OBJECT);
    jwObj_int(&jwc, "notifications",  (int)settings->notifications );
    jwObj_bool(&jwc, "newsletter",  settings->newsletter );
    jwClose(&jwc);
    
    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Patch, "settings", buf , 1,  0 );
    
    if (!response.success && error ) {
        xq_fill_error(&response, error);
    }
    xq_destroy_response(&response);
    return response.success;
    
    
}
