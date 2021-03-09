#include <stdio.h>
#include <stdlib.h>
#include <xq/config.h>
#include <xq/connect.h>
#include <xq/services/quantum/quantum.h>
#include <xq/services/sub/packet.h>
#include <xq/services/crypto.h>
#include <ext/jwrite/jWrite.h>
#include <xq/services/sub/authorize.h>
#include <xq/algorithms/aes/aes_decrypt.h>
#include <ext/base64/base64.h>
#include <ext/jread/jRead.h>
#include <memory.h>

_Bool xq_svc_authorize_trusted(
                               struct xq_config* config,
                               const char* email_or_phone,
                               int team_id,
                               const char* security_key,
                               const char* device_name,
                               struct xq_error_info* error ) {
 
    if (!config ){
        if (error) {
            xq_strcat(error->content, "No config object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    

    // Create the request path.
    char path[24] = {0};
    char* tail = xq_strcat( path, "authorize/trusted/", 24 );
    itoa(team_id, tail, 6);

    struct xq_response response = xq_call( config, Server_Sub, CallMethod_Post, path, "" , 1,  0 );
    
    // Ensure the authorization JSON was written properly.
    if ( response.success ) {
          
        // Decode the base64 payload.
        uint32_t decoded_len = 0;
        uint8_t* decoded_base64 = base64_decode((const uint8_t*)response.content, response.size, &decoded_len);
        
        // If a security key was provided, attempt to decrypt the response.
        struct xq_message_payload result = {0,0};
        
        char key[70] = {0};
        if (security_key && security_key[0] != '.') {
            xq_strcat(xq_strcat(key,".A", 70), (char*)security_key, 70);
        }
        else {
            memcpy(key, security_key, strlen(security_key));
        }

        if (!xq_aes_decrypt(decoded_base64, decoded_len, key, &result, error)) {
            // If the decryption failed, abort immediately.
            xq_destroy_payload(&result);
            xq_destroy_response(&response);
            return 0;
        }

        struct xq_message_payload pl = {0,0};

        char token[400] = {0};
        
        size_t token_len = jRead_string( (char*)result.data, "{'access_token'",  token, sizeof(token)-1, 0);
        if ( token_len == 0 ) {
            xq_strcat(error->content, "Failed to read access token", MAX_ERROR_LENGTH);
            xq_destroy_response(&response);
            return 0;
        }
        

        if ( token_len > 0) {
            
            xq_set_access_token(config, token);
            
            char buf[STATUS_BUFFER_SIZE] = {0};
            
            struct jWriteControl jwc;
            jwOpen(&jwc, buf, STATUS_BUFFER_SIZE, JW_OBJECT);
            jwObj_string(&jwc, "afirst", (char*) device_name);
            jwObj_string(&jwc, "alast", "");
            jwClose(&jwc);
            
            // Unlike every other dashboard call, the /announce endpoint allows us to use
            // our XQ API key instead. So if one is not provided, use that instead.
            _Bool noDashboardKey = config->dashboard_api_key == 0;
            if (noDashboardKey) config->dashboard_api_key = config->xq_api_key;
            
            char* existingToken  = config->dashboard_token;
            config->dashboard_token = config->access_token;
            
            struct xq_response announce_response = xq_call(config, Server_Saas, CallMethod_Post, "trusted/announce", buf, 1, 0);
            
            // Clear the API key.
            if (noDashboardKey) config->dashboard_api_key = 0;
            
            if (!announce_response.success) {
                fprintf(stderr, "[trusted/announce] %li: %s\n", announce_response.responseCode, announce_response.content );
            }
            xq_destroy_response(&announce_response);
            config->dashboard_token = existingToken;

        }
        
        else {
            xq_strcat(error->content, "Failed to decode access token", MAX_ERROR_LENGTH);
            xq_destroy_response(&response);
            return 0;
        }
        xq_destroy_payload(&result);
    }
    
    else if (error) {
        xq_fill_error(&response, error);
    }
    
    xq_destroy_response(&response);
    return response.success;

}
