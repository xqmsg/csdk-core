//
//  subscriber.c
//  xqc
//
//  Created by Ike E on 10/18/20.
//

#include <stdio.h>
#include <xq/config.h>
#include <xq/connect.h>
#include <ext/jwrite/jWrite.h>
#include <ext/jread/jRead.h>
#include <xq/services/sub/subscriber.h>


_Bool xq_svc_get_subscriber( struct xq_config* config, struct xq_subscriber_info* info, struct xq_error_info* error  ) {
    
    
    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Get, "subscriber", 0 , 1,  0 );
    
    if ( response.success ) {
        if (info ) {
            
            char* c = (char*) response.content;
            
            jRead_string( c , "{'firstName'",   info->firstName, sizeof(info->firstName)-1, 0);
            jRead_string( c, "{'lastName'",     info->lastName, sizeof(info->lastName)-1, 0);
            jRead_string( c, "{'user'",         info->mailOrPhone, sizeof(info->mailOrPhone)-1, 0);
            
            info->status    = jRead_int(c, "{'_sub'", 0 );
            info->startedOn = jRead_long(c,  "{'starts'", 0);
            info->endsOn    = jRead_long(c,  "{'ends'", 0);

        }
    }
    
    else if ( error ) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;

}

_Bool  xq_svc_delete_subscriber( struct xq_config* config, struct xq_error_info* error  ) {
    
    
    if (!config || config->access_token == 0 ){
        if (error) {
            xq_strcat(error->content, "No exchange code has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Delete, "subscriber", 0 , 1,  0 );
    
    if (response.success) {
        // Since the access token is no longer valid, remove it from the configuration
        // object completely.
        set_access_token(config, 0);
    }
    
    else if (error) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;
    
}

