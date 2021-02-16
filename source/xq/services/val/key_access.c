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
#include <xq/services/val/key_access.h>



_Bool xq_svc_grant_access( struct xq_config* config,
                          char* token,
                          const char** recipients,
                          int recipient_count,
                          struct xq_error_info* error ) {
    
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
    
    if (!recipients ){
        if (error) {
            xq_strcat(error->content, "No recipients to add were provided." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    char serviceUrl[MAX_URL_LENGTH] = {0};
    
    // Create the escaped token.
    char* escaped = xq_escape(token);
    
    // Create the query parameters
    xq_strcat( xq_strcat( serviceUrl, "grant/", MAX_URL_LENGTH ), (char*) escaped , 0 );
    
    free(escaped);
    
    // Next we need to calculate how much space we need for this request.
    int len = 20;
    for ( int x = 0; x < recipient_count; ++x ) {
        len += ((int)strlen(recipients[x])) + 3;
    }
    char* buf = malloc(  len );
    struct jWriteControl jwc;
    jwOpen(&jwc, buf, len, JW_OBJECT);
    jwObj_array(&jwc, "recipients");
    for ( int x = 0; x < recipient_count; ++x ) {
        jwArr_string(&jwc, (char*)recipients[x]);
    }
    jwEnd(&jwc);
    jwClose(&jwc);
    
    struct xq_response response = xq_call( config, Server_Val, CallMethod_Post, serviceUrl, buf , 1,  0 );
    
    free(buf);
    
    if (!response.success && error) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;
    
    return 0;
    
}

_Bool xq_svc_revoke_access(   struct xq_config* config,
                           char* token,
                           const char** recipients,
                           int recipient_count,
                           struct xq_error_info* error ) {
    
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
    
    if (!recipients ){
        if (error) {
            xq_strcat(error->content, "No recipients to revoke were provided." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    char serviceUrl[MAX_URL_LENGTH] = {0};
    
    // Create the escaped token.
    char* escaped = xq_escape(token);
    
    // Create the query parameters
    xq_strcat( xq_strcat( serviceUrl, "revoke/", MAX_URL_LENGTH ), (char*) escaped , 0 );
    
    free(escaped);
    
    // Next we need to calculate how much space we need for this request.
    int len = 20;
    for ( int x = 0; x < recipient_count; ++x ) {
        len += ((int)strlen(recipients[x])) + 3;
    }
    char* buf = malloc(  len );
    struct jWriteControl jwc;
    jwOpen(&jwc, buf, len, JW_OBJECT);
    jwObj_array(&jwc, "recipients");
    for ( int x = 0; x < recipient_count; ++x ) {
        jwArr_string(&jwc, (char*)recipients[x]);
    }
    jwEnd(&jwc);
    jwClose(&jwc);
    
    struct xq_response response = xq_call( config, Server_Val, CallMethod_Patch, serviceUrl, buf , 1,  0 );
    
    free(buf);
    
    if (!response.success && error) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;
    
    return 0;
}

