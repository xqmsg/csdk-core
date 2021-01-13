//
//  authorize_now.c
//  xqc
//
//  Created by Ike E on 10/16/20.
//


#include <stdio.h>

#include <xq/config.h>
#include <xq/connect.h>
#include <ext/jwrite/jWrite.h>
#include <xq/services/sub/authorize_alias.h>


_Bool xq_svc_authorize_alias(struct xq_config* config, const char* email, struct xq_error_info* error ) {
    
    // Write the email into a json payload.
    char buf[STATUS_BUFFER_SIZE] = {0};
    
    struct jWriteControl jwc;
    jwOpen(&jwc, buf, STATUS_BUFFER_SIZE, JW_OBJECT);
    jwObj_string(&jwc, "user", (char*) email);
    jwClose(&jwc);
    
    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Post, "authorizealias", buf , 1, 0 );
    
    // Ensure the authorization JSON was written properly.
    if ( response.success) {
        // Automatically store the exchange token in the configuration object.
        set_access_token( config, (const char*) response.content );
    }
    else if (error) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;
}
