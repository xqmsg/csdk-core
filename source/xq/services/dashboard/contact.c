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
#include <xq/services/sub/settings.h>
#include <xq/services/dashboard/contact.h>



long xq_add_contact( struct xq_config* config,
                            const char* identifier,
                            const char* first_name,
                            const char* last_name,
                            const char* title,
                            enum xq_business_role role,
                            enum xq_notification_level notification_level,
                            struct xq_error_info* error ) {
    
    
    if (!config ){
        if (error) {
            xq_strcat(error->content, "No config object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    if (identifier <= 0 ){
        if (error) {
            xq_strcat(error->content, "Identifier cannot be null" , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    if (role == role_alias) notification_level = no_notifications;
    
    char buf[512] = {0};

    struct jWriteControl jwc;
    jwOpen(&jwc, buf, sizeof buf, JW_OBJECT);
    jwObj_string(&jwc, "email", (char*) identifier  );
    jwObj_int(&jwc, "role", role);
    jwObj_int(&jwc, "notifications", notification_level );
    if (first_name != 0 ) jwObj_string(&jwc, "firstName", (char*) first_name );
    if (last_name != 0 )jwObj_string(&jwc, "lastName", (char*) last_name );
    if (title != 0 ) jwObj_string(&jwc, "title", (char*) title );
    jwClose(&jwc);
    
    struct xq_response response = xq_call( config, Server_Saas, CallMethod_Post, "contact", buf , 1,  0 );

    // If something went wrong...
    if (!response.success) {
        if (error) {
            xq_fill_error(&response, error);
        }
        xq_destroy_response(&response);
        return 0;
    }
    
    // Extract the ID from the content.
    struct jReadElement elem;
    long id = jRead_long( (char*) response.content, "{'id'", 0);
    xq_destroy_response(&response);
    return id;
}


_Bool xq_remove_contact( struct xq_config* config,
                        long internal_contact_id,
                              struct
                            xq_error_info* error ) {
    
     if (!config ){
         if (error) {
             xq_strcat(error->content, "No config object has been set." , MAX_ERROR_LENGTH);
             error->responseCode = -1;
         }
         return 0;
     }

     char serviceUrl[64] = {0};
     snprintf(serviceUrl, sizeof serviceUrl, "contact/%li?delete=true", internal_contact_id);
     
     struct xq_response response = xq_call( config, Server_Saas, CallMethod_Delete, serviceUrl, 0 , 1,  0 );

     // If something went wrong...
     if (!response.success) {
         if (error) {
             xq_fill_error(&response, error);
         }
         
     }
    xq_destroy_response(&response);
    return response.success;
    
}


_Bool xq_disable_contact( struct xq_config* config,
                        long internal_contact_id,
                              struct
                            xq_error_info* error ) {
    
     if (!config ){
         if (error) {
             xq_strcat(error->content, "No config object has been set." , MAX_ERROR_LENGTH);
             error->responseCode = -1;
         }
         return 0;
     }

     char serviceUrl[32] = {0};
     snprintf(serviceUrl, sizeof serviceUrl, "contact/%li", internal_contact_id);
     
     struct xq_response response = xq_call( config, Server_Saas, CallMethod_Delete, serviceUrl, 0 , 1,  0 );

     // If something went wrong...
     if (!response.success) {
         if (error) {
             xq_fill_error(&response, error);
         }
         
     }
    xq_destroy_response(&response);
    return response.success;
    
}
