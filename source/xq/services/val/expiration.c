//
//  expiration.c
//  xqc
//
//  Created by Ike E on 10/21/20.
//

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <xq/config.h>
#include <xq/connect.h>
#include <ext/jwrite/jWrite.h>
#include <ext/jread/jRead.h>
#include <xq/services/val/expiration.h>


_Bool xq_svc_check_expiration(
                     struct xq_config* config,
                     char* token,
                     long* secondsLeft,
                     struct xq_error_info* error  ) {
    
    
    if (!config ){
        if (error) {
            xq_strcat(error->content, "No config object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    if (!token ){
        if (error) {
            xq_strcat(error->content, "No token has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    if (!secondsLeft ){
        if (error) {
            xq_strcat(error->content, "No object was provided to store seconds." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    char serviceUrl[MAX_URL_LENGTH] = {0};
    
    // Create the escaped token.
    char* escaped = xq_escape(token);
    
    // Create the query parameters
    xq_strcat( xq_strcat( serviceUrl, "expiration/", MAX_URL_LENGTH ), (char*) escaped , 0 );
    
    free(escaped);
    
    struct xq_response response = xq_call( config, Server_Val, CallMethod_Get, serviceUrl, 0 , 1,  0 );
    
    if (response.success) {
        *secondsLeft = jRead_int( (char*)response.content, "{'expiresOn'", 0);
    }
    
    else if (error) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;
}
