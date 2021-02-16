//
//  packet.c
//  xqc
//
//  Created by Ike E on 10/20/20.
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
#include <xq/services/sub/packet.h>


_Bool xq_svc_store_key(
                      struct xq_config* config,
                      struct xq_message_token_request* request,
                      struct xq_message_token* result,
                      struct xq_error_info* error  ) {
    
    if (!config ){
        if (error) {
            xq_strcat(error->content, "No config object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    if (!request ){
        if (error) {
            xq_strcat(error->content, "No request object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    // Next we need to calculate how much space we need for this request.
    int len = 70 + request->key_len;
    int idx = 0;
    if (request->type_tag) len += (int) strlen(request->type_tag);
    len += strlen(request->recipients);
    char* buf = malloc(  len + 3  );
    struct jWriteControl jwc;
    jwOpen(&jwc, buf, len, JW_OBJECT);
    jwObj_string_max(&jwc, "key", request->key, request->key_len );
    jwObj_bool(&jwc, "dor", request->delete_on_read );
    jwObj_int(&jwc, "expires", request->expiration );
    if (request->type_tag) jwObj_string(&jwc, "type", (char*)request->type_tag );
    jwObj_string(&jwc, "recipients", (char*) request->recipients );

  //  jwEnd(&jwc);
    jwClose(&jwc);
    
    struct xq_response sub_response = xq_call( config, Server_Sub, CallMethod_Post, "packet", buf , 1,  0 );
    
    free(buf);

    // If something went wrong...
    if (!sub_response.success) {

        if (error) {
            xq_fill_error(&sub_response, error);
        }
        xq_destroy_response(&sub_response);
        return 0;
    }
    
    // If there were no issues, we should have a new encrypted packet. Send the packet to the validation server.
    struct xq_response response = xq_call( config, Server_Val, CallMethod_Post, "packet", (char*) sub_response.content , 1,  0 );
    
    
    xq_destroy_response(&sub_response);
    
    if (response.success) {
        
        if (result->data == 0) {
            result->data = malloc(response.size);
            memcpy(result->data, response.content, response.size);
            result->length = (int) response.size;
        }
        else if (result->length >= response.size) {
            memcpy(result->data, response.content, response.size);
            result->length = (int) response.size;
        }
        else {
            response.success = 0;
            xq_strcat(error->content, "Buffer is not large enough to hold result" , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
    }
    
    else if (error) {
        xq_fill_error(&sub_response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;
    
}


_Bool xq_svc_get_key(
                     struct xq_config* config,
                     char* token,
                     struct xq_key* result,
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
    
    if (!result ){
        if (error) {
            xq_strcat(error->content, "No object was provided to store resulting key." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    // Write the email into a json payload.
    char serviceUrl[MAX_URL_LENGTH] = {0};
    
    // Create the escaped token.
    char* escaped = xq_escape(token);
    
    // Create the query parameters
    xq_strcat(xq_strcat( serviceUrl, "key/", MAX_URL_LENGTH ), (char*) escaped , 0 );
    
    free(escaped);
    
    struct xq_response response = xq_call( config, Server_Val, CallMethod_Get, serviceUrl, 0 , 1,  0 );
    
    
    if (response.success) {
        
        if ( result->data != 0 && result->length == 0 ) {
            result->length = (int) response.size;
            memcpy(result->data, response.content, response.size);
        }
        else if ( result->data == 0 ) {
            result->data = malloc(  response.size + 1 );
            result->length = (int) response.size;
            memcpy(result->data, response.content, response.size);
            result->data[response.size] = '\0';
        }
        else if (  result->length >= response.size ) {
            result->length = (int) response.size;
            memcpy(result->data, response.content, response.size);
        }
        else {
            response.success = 0;
            xq_strcat(error->content, "Buffer is not large enough to hold secret key" , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
    }
    
    else if (error) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;
}


_Bool xq_svc_remove_key(
                        struct xq_config* config,
                        char* token,
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
    
    // Write the email into a json payload.
    char serviceUrl[MAX_URL_LENGTH] = {0};
    
    // Create the escaped token.
    char* escaped = xq_escape(token);
    
    // Create the query parameters
    xq_strcat(xq_strcat( serviceUrl, "key/", MAX_URL_LENGTH ), (char*) escaped , 0 );
    
    free(escaped);
    
    struct xq_response response = xq_call( config, Server_Val, CallMethod_Delete, serviceUrl, 0 , 1,  0 );

    if (!response.success && error ) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;
    
}
