//
//  config.c
//  xqc
//
//  Created by Ike E on 10/16/20.
//

#include <stdio.h>
#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#include <xq/config.h>
#include <xq/connect.h>
#include <ext/inih/ini.h>
#include <ext/jread/jRead.h>
#include <ext/uri_encode/uri_encode.h>



const struct xq_config empty_conf = { 0,0,0,0,0,0,0,0 };


/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *xq_escape(char *str) {
    
    const size_t len = strlen(str);
    char* buffer = calloc( len * 3 + 1 , 1 );
    uri_encode(str, len,  buffer);
    return buffer;

}

char* xq_strcat( char* tail, char* src, int max_buf )
{
    if (max_buf == 0 ) {
        while (*tail) tail++;
        while ((*tail++ = *src++));
        return --tail;
    }
    
    char* dest = memccpy(tail, src,'\0', max_buf );
    return (dest) ? --dest  : NULL;
};


_Bool xq_set_access_token(struct xq_config* config, const char* token ) {

    if (!config)  return 0;
    
    if (config->access_token) {
        free(config->access_token );
        config->access_token = 0;
    }
    
    if (token) {
        config->access_token = calloc(strlen(token) + strlen(BEARER_TAG) + 1, 1 );
        xq_strcat(xq_strcat(config->access_token, BEARER_TAG, 0 ), (char*)token, 0 );
    }
    
    return 1;
}

_Bool set_exchange_token(struct xq_config* config, const char* token, int len ) {
    
    if (!config) return 0;
    
    if (config->exchange_token) {
        free(config->exchange_token );
        config->exchange_token = 0;
    }
    
    if (token) {
        if (len == 0 ) len = (int) strlen(token);
        config->exchange_token = calloc( len + BEARER_TAG_LEN, 1  );
        xq_strcat( xq_strcat(config->exchange_token, BEARER_TAG, BEARER_TAG_LEN ) , (char*) token, len );
    }

    return 1;
}

const char* xq_get_access_token(struct xq_config* config) {
    if (!config) return 0;
    return (const char*)(config->access_token + (int)strlen(BEARER_TAG));
}


static int _ini_handler(void* user, const char* section, const char* name,
                        const char* value)
{
    struct xq_config* pconfig = (struct xq_config*)user;
    
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    
    if (MATCH("Connections", "Sub"))  pconfig->subscription_url = strdup(value);
    else if (MATCH("Connections", "Val"))  pconfig->validation_url = strdup(value);
    else if (MATCH("Connections", "Saas"))  pconfig->saas_url = strdup(value);
    else if (MATCH("Connections", "Quantum"))  pconfig->quantum_url = strdup(value);
    else if (MATCH("ApiKeys", "XQ"))  {
        pconfig->xq_api_key = calloc(strlen(value) + strlen(APIKEY_TAG), 1 );
        char* tail = xq_strcat(pconfig->xq_api_key, APIKEY_TAG , 0);
        xq_strcat( tail, (char*)value,  0);
    }
    else if (MATCH("ApiKeys", "Dashboard")) {
        pconfig->dashboard_api_key = calloc(strlen(value) + strlen(APIKEY_TAG), 1 );
        char* tail = xq_strcat(pconfig->dashboard_api_key, APIKEY_TAG , 0);
        xq_strcat( tail, (char*)value, 0);
    }
    else if (MATCH("Settings", "Timeout"))  pconfig->timeout_secs = atoi(value);
    else if (MATCH("Settings", "Threads"))  pconfig->support_threads = strcmp("true",value) == 0;
    else return 0; //  unknown section/name, error
    return 1;
}

struct xq_config xq_init(const char* configPath) {
    
   //curl_global_init( CURL_GLOBAL_DEFAULT );
    
    struct xq_config new_config = empty_conf;
    
    if (ini_parse(configPath, _ini_handler, &new_config ) < 0 ) {
        fprintf ( stderr, "Unable to locate or open configuration file: %s\n" , configPath );
        return new_config;
    }
    
    return new_config;
    
}

_Bool xq_is_valid_config(struct xq_config* conf) {
    return conf->subscription_url != 0
    && conf->validation_url !=0
    && conf->xq_api_key != 0;
}

void xq_destroy_config(struct xq_config* config) {
    
    if (!config) return;
    if (config->saas_url ) free(config->saas_url);
    if (config->quantum_url ) free(config->quantum_url);
    if (config->subscription_url ) free(config->subscription_url);
    if (config->validation_url ) free(config->validation_url);
    if (config->xq_api_key ) free(config->xq_api_key);
    if (config->dashboard_api_key ) free(config->dashboard_api_key );
    if (config->access_token ) free(config->access_token );
    if (config->exchange_token ) free(config->exchange_token );
    memset(config, 0, sizeof(struct xq_config));
    //curl_global_cleanup();
}

_Bool xq_fill_error(struct xq_response* response, struct xq_error_info* error) {
    if (response == 0 || error == 0 || response->content == 0 ) return 0;
    jRead_string( (char*)response->content, "{'status'", error->content, MAX_ERROR_LENGTH, 0);
    error->responseCode = response->responseCode;
    return 1;
}
