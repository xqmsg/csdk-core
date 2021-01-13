//
//  combined.c
//  xqc
//
//  Created by Ike E on 10/19/20.
//

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

#include <xq/config.h>
#include <xq/connect.h>
#include <ext/jwrite/jWrite.h>
#include <ext/jread/jRead.h>
#include <xq/services/sub/combined.h>


void xq_destroy_combined(struct xq_combined_token* combined) {
    if (combined->data) free(combined->data);
    combined->data = 0;
}

_Bool xq_svc_combine_tokens ( struct xq_config* config, const char** tokens, int token_count, struct xq_combined_token* result, struct xq_error_info* error  ){
   
    if (!config || config->access_token == 0 ){
        if (error) {
            xq_strcat(error->content, "No access token has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    int len = 14;
    for ( int x = 0; x < token_count; ++x ) {
        len += ((int)strlen(tokens[x])) + 3;
    }
    
    // Write the email into a json payload.
    //char bufBacking[1024] = {0};
    char* buf = malloc( len);

    struct jWriteControl jwc;
    jwOpen(&jwc, buf, len, JW_OBJECT);
    jwObj_array(&jwc, "tokens");
    for ( int x = 0; x < token_count; ++x ) {
        jwArr_string(&jwc, (char*)tokens[x]);
    }
    jwEnd(&jwc);
    jwClose(&jwc);
    
    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Post, "combined", buf , 1,  0 );
    
    free(buf);
    
    if (response.success) {
        
        struct jReadElement elem;
        jRead((char*)response.content, "{'token'", &elem);
        result->merged = jRead_int( (char*) response.content, "{'merged'", 0);
        
        if ( result->length == 0 ) {
            result->data = malloc(  elem.bytelen);
            result->length = elem.bytelen;
            jRead_string( (char*) response.content, "{'token'", result->data, elem.bytelen , 0);
        }
        
        else if (  result->length >=elem.bytelen ) {
            result->length = elem.bytelen  ;
            jRead_string( (char*) response.content, "{'token'", result->data, elem.bytelen  , 0);
        }
        
        else {
            response.success = 0;
            xq_strcat(error->content, "Buffer is not large enough to hold combined token" , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
    }
    
    else if (error) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;
    
}
