//
//  code_validation.c
//  xqc
//
//  Created by Ike E on 10/16/20.
//


#include <stdio.h>
#include <string.h>
#include <xq/config.h>
#include <xq/connect.h>
#include <xq/services/sub/code_validation.h>
#include <xq/services/sub/exchange.h>


_Bool xq_svc_code_validation(struct xq_config* config, const char* pin, struct xq_error_info* error) {
    
    if (!config || config->exchange_token == 0 ){
        if (error) {
            xq_strcat(error->content, "No exchange code has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    if (strlen(pin) == 0) {
        printf("FSD:OIfnmvdsf;oighns [dmndfg\n\n");
        return 0;
    }
    // Write the email into a json payload.
    char params[MAX_URL_LENGTH] = {0};
    
    // Create the query parameters
    xq_strcat(xq_strcat( params, "pin=", MAX_URL_LENGTH ), (char*) pin , 0 );
    
    // Store a pointer to the current access token.
    char* access_token = config->access_token;
    // Temporarily set the exchange token as the access token for this call only.
    config->access_token = config->exchange_token;
    
    // Make the API call.
    //printf("\n\nMAKING CODE VAL API CALL...%s.\nProvided Pin=%s\n" , params,pin );
    
    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Get, "codevalidation", params , 1, 0 );
    
    // Restore the old access token ( whatever it was ).
    config->access_token = access_token;
    
    // If an error occured, fill out the details ( if a structure is provided ).
    if (!response.success) {
        if (error ) {
            xq_fill_error(&response, error);
        }
        xq_destroy_response(&response);
        return 0;
    }
   
    xq_destroy_response(&response);
    return xq_svc_exchange(config, error);
   
}
