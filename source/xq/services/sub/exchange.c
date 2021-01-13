//
//  exchange.c
//  xqctest
//
//  Created by Ike E on 10/17/20.
//


#include <stdio.h>
#include <string.h>

#include <xq/config.h>
#include <xq/connect.h>
#include <xq/services/sub/exchange.h>


_Bool xq_svc_exchange(struct xq_config* config, struct xq_error_info* error ){

    if (config->exchange_token == 0 ){
        if (error) {
            xq_strcat(error->content, "No exchange code has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    // Clear the current access token. This will deallocate any previously allocated token.
    set_access_token(config, 0);
    
    // Set the access token to the current exchange token without a duplicate allocation.
    config->access_token = config->exchange_token;

    // Make the service call.
    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Get, "exchange", 0 , 1, 0 );
    
    // Clear the pointer to the access token ( so that when the config object is destroyed, the
    // token will not be destroyed twice ).
    config->access_token = 0;
    
    // Check whether we were able to obtain an access token.
    if ( response.success ) {
        
        //  Store the new access token.
        set_access_token( config, (const char*) response.content );
        
        // We've used up the exchange token, so clear it from memory..
        set_exchange_token(config, 0 , 0);
        
    }
    
    // If anything went wrong, save the reason ( if an error object is provided ).
    else if (error) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;

}
