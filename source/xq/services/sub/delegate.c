//
//  delegate.c
//  xqc
//
//  Created by Ike E on 10/20/20.
//

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <xq/config.h>
#include <xq/connect.h>
#include <ext/jwrite/jWrite.h>
#include <ext/jread/jRead.h>
#include <xq/services/sub/delegate.h>


void xq_destroy_delegate(struct xq_delegate_token* delegated) {
    if (delegated->data) free(delegated->data);
    delegated->data = 0;
}


_Bool xq_svc_delegate_token  ( struct xq_config* config, struct xq_delegate_token* result, struct xq_error_info* error  ) {
    
    if (!config || config->access_token == 0 ){
        if (error) {
            xq_strcat(error->content, "No access token has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Get, "delegate", 0 , 1,  0 );
    
    
    if (response.success) {
        
        struct jReadElement elem;
        jRead((char*)response.content, "{'token'", &elem);

        if ( result->length == 0 ) {
            result->data = malloc(  response.size);
            result->length = (int) response.size;
            memcpy(result->data, response.content, response.size);
        }
        else if (  result->length >= response.size ) {
            result->length = (int) response.size;
            memcpy(result->data, response.content, response.size);
        }
        else {
            response.success = 0;
            xq_strcat(error->content, "Buffer is not large enough to hold delegate token" , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
    }
    
    else if (error) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;
    
}

