//
//  authorize.c
//  xqc
//
//  Created by Ike E on 10/16/20.
//

#include <stdio.h>
#include <xq/config.h>
#include <xq/connect.h>
#include <ext/jwrite/jWrite.h>
#include <xq/services/sub/authorize.h>

//#define AUTH_TEST_MODE

#ifdef AUTH_TEST_MODE
#include <ext/jread/jRead.h>
#endif

_Bool xq_svc_authorize(struct xq_config* config, const char* email_or_phone, struct xq_error_info* error ) {
 
    if (!config ){
        if (error) {
            xq_strcat(error->content, "No config object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    // Write the email into a json payload.
    char buf[STATUS_BUFFER_SIZE] = {0};

    struct jWriteControl jwc;
    jwOpen(&jwc, buf, STATUS_BUFFER_SIZE, JW_OBJECT);
    jwObj_string(&jwc, "user", (char*) email_or_phone );
#ifdef AUTH_TEST_MODE
    jwObj_bool(&jwc, "quiet", 1 );
#endif
    jwClose(&jwc);
    
    printf("\nAuthorization request: %s\n\n", buf);
    
    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Post, "authorize", buf , 1,  0 );
    
    // Automatically store the exchange token in the configuration object.
    printf("\n\nAuthorization Response: %s\n\n", (char*)response.content );
    
    
    // Ensure the authorization JSON was written properly.
    if ( response.success ) {
        
     
#ifdef AUTH_TEST_MODE
        
        struct jReadElement elem;
        if ( jRead( (char*) response.content , "{'exchange'", &elem) ) {
            set_exchange_token( config, (const char*) elem.pValue, elem.bytelen );
        }
#else
        set_exchange_token( config, (const char*) response.content, 0 );
#endif
    }
    
    else if (error) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;

}

_Bool xq_svc_deauthorize(struct xq_config* config, struct xq_error_info* error ) {
    
    if (!config || config->access_token == 0 ){
        if (error) {
            xq_strcat(error->content, "No exchange code has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Delete, "authorization", 0 , 1,  0 );
    
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
