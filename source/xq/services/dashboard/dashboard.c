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
#include <xq/services/dashboard/dashboard.h>


_Bool xq_connect_dashboard( struct xq_config* config,
                        long team_id, struct
                           xq_error_info* error ) {
    
    if (!config ){
        if (error) {
            xq_strcat(error->content, "No config object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    if ( config->access_token == 0 ) {
        if (error) {
            xq_strcat(error->content, "No access token available." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    
    if ( config->dashboard_api_key == 0 ) {
        if (error) {
            xq_strcat(error->content, "No dashboard API key available." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    if ( config->dashboard_token != 0 ) {
        xq_set_dashboard_token(config, 0);
    }

    config->dashboard_token = config->access_token;
    
    char serviceUrl[140] = {0};
    snprintf(serviceUrl, sizeof serviceUrl, "login/verify?request=sub&bid=%li", team_id);
    
    
    struct xq_response response = xq_call(config, Server_Saas, CallMethod_Get, serviceUrl, 0, 1, 0);
    
    if (!response.success)
    {
        if (error) {
            xq_fill_error(&response, error);
        }
    }
    else {
        config->dashboard_token  = 0;
        xq_set_dashboard_token(config,  (const char*) response.content );
    }
    
    
    xq_destroy_response(&response);
    return response.success;
    
}
