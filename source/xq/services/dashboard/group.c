//
//  exchange.c
//  xqctest
//
//  Created by Ike E on 10/17/20.
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
#include <xq/services/dashboard/group.h>



long xq_add_user_group( struct xq_config* config,
                        struct  xq_user_group* group,
                          struct xq_error_info* error ) {
   
    if (!config ){
        if (error) {
            xq_strcat(error->content, "No config object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    if (!group ){
        if (error) {
            xq_strcat(error->content, "No group object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    int idx;

    // Next we need to calculate how much space we need for this request.
    int len = 25 + strlen(group->alias);
    for ( idx = 0; idx < group->member_count; ++idx) len += (strlen(group->members[idx]) + 2);

    char* buf = calloc(  1,  len + 1  );
 
    struct jWriteControl jwc;
    jwOpen(&jwc, buf, len, JW_OBJECT);
    jwObj_string(&jwc, "name", group->alias );
    jwObj_array(&jwc, "members");
    for ( idx = 0; idx < group->member_count; ++idx ) {
        jwArr_string(&jwc, (char*)group->members[idx]);
    }
    jwEnd(&jwc);
    jwClose(&jwc);
    
    struct xq_response response = xq_call( config, Server_Saas, CallMethod_Post, "usergroup", buf , 1,  0 );
    
    free(buf);

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



_Bool xq_remove_user_group( struct xq_config* config, long group_id, struct xq_error_info* error ) {
    
   
    if (!config ){
        if (error) {
            xq_strcat(error->content, "No config object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }

    char serviceUrl[32] = {0};
    snprintf(serviceUrl, sizeof serviceUrl, "usergroup/%li", group_id);
    
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


_Bool xq_update_user_group( struct xq_config* config,
                        struct  xq_user_group* group,
                              struct xq_error_info* error ) {
    if (!config ){
        if (error) {
            xq_strcat(error->content, "No config object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }
    
    if (!group ){
        if (error) {
            xq_strcat(error->content, "No group object has been set." , MAX_ERROR_LENGTH);
            error->responseCode = -1;
        }
        return 0;
    }

    // Next we need to calculate how much space we need for this request.
    int len = 25 + strlen(group->alias);
    for ( int x = 0; x < group->member_count; ++x) len += (strlen(group->members[x]) + 2);

    char* buf = calloc(  1, len + 1  );
    struct jWriteControl jwc;
    jwOpen(&jwc, buf, len, JW_OBJECT);
    jwObj_string(&jwc, "name", group->alias );
    jwObj_array(&jwc, "members");
    for ( int x = 0; x < group->member_count; ++x ) {
        jwArr_string(&jwc, (char*)group->members[x]);
    }
    jwEnd(&jwc);
    jwClose(&jwc);
    
    
    char serviceUrl[32] = {0};
    snprintf(serviceUrl, sizeof serviceUrl, "usergroup/%li", group->group_id);
    
    printf("Command for update mailing group: %s\n", buf);
    
    struct xq_response response = xq_call( config, Server_Saas, CallMethod_Patch, serviceUrl , buf , 1,  0 );
    
    free(buf);

    // If something went wrong...
    if (!response.success) {
        if (error) {
            xq_fill_error(&response, error);
        }
    }
    xq_destroy_response(&response);
    return response.success;
}
