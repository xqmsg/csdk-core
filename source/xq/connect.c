//
//  connect.c
//  xqc
//
//  Created by Ike E on 10/15/20.
//

#include <stdio.h>
#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <xq/config.h>
#include <xq/connect.h>

/////// START DMUP

#ifdef ENABLE_CURL_DEBUG
struct data {
    char trace_ascii; /* 1 or 0 */
};

static
void dump(const char *text,
          FILE *stream, unsigned char *ptr, size_t size,
          char nohex)
{
    size_t i;
    size_t c;
    
    unsigned int width = 0x10;
    
    if(nohex)
    /* without the hex output, we can fit more on screen */
        width = 0x40;
    
    fprintf(stream, "%s, %10.10lu bytes (0x%8.8lx)\n",
            text, (unsigned long)size, (unsigned long)size);
    
    for(i = 0; i<size; i += width) {
        
        fprintf(stream, "%4.4lx: ", (unsigned long)i);
        
        if(!nohex) {
            /* hex not disabled, show it */
            for(c = 0; c < width; c++)
            if(i + c < size)
                fprintf(stream, "%02x ", ptr[i + c]);
            else
                fputs("   ", stream);
        }
        
        for(c = 0; (c < width) && (i + c < size); c++) {
            /* check for 0D0A; if found, skip past and start a new line of output */
            if(nohex && (i + c + 1 < size) && ptr[i + c] == 0x0D &&
               ptr[i + c + 1] == 0x0A) {
                i += (c + 2 - width);
                break;
            }
            fprintf(stream, "%c",
                    (ptr[i + c] >= 0x20) && (ptr[i + c]<0x80)?ptr[i + c]:'.');
            /* check again for 0D0A, to avoid an extra \n if it's at width */
            if(nohex && (i + c + 2 < size) && ptr[i + c + 1] == 0x0D &&
               ptr[i + c + 2] == 0x0A) {
                i += (c + 3 - width);
                break;
            }
        }
        fputc('\n', stream); /* newline */
    }
    fflush(stream);
}

static
int my_trace(CURL *handle, curl_infotype type,
             char *data, size_t size,
             void *userp)
{
    struct data *config = (struct data *)userp;
    const char *text;
    (void)handle; /* prevent compiler warning */
    
    switch(type) {
        case CURLINFO_TEXT:
            fprintf(stderr, "== Info: %s", data);
            /* FALLTHROUGH */
        default: /* in case a new one is introduced to shock us */
            return 0;
            
        case CURLINFO_HEADER_OUT:
            text = "=> Send header";
            break;
        case CURLINFO_DATA_OUT:
            text = "=> Send data";
            break;
        case CURLINFO_SSL_DATA_OUT:
            text = "=> Send SSL data";
            break;
        case CURLINFO_HEADER_IN:
            text = "<= Recv header";
            break;
        case CURLINFO_DATA_IN:
            text = "<= Recv data";
            break;
        case CURLINFO_SSL_DATA_IN:
            text = "<= Recv SSL data";
            break;
    }
    
    dump(text, stderr, (unsigned char *)data, size, config->trace_ascii);
    return 0;
}

#endif
/// END DMUP

static size_t _write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct xq_response *mem = (struct xq_response *)userp;
    uint8_t *ptr = realloc(mem->content, mem->size + realsize + 1);
    if(ptr == NULL) {
        fprintf( stderr, "not enough memory to continue network write (realloc returned NULL)\n" );
        return 0;
    }
    
    mem->content = ptr;
    memcpy(&(mem->content[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->content[mem->size] = 0;
    return realsize;
}


static inline const char* _server_host_for_product(struct xq_config* config,  enum product_server server) {
    switch ( server ) {
            
        case Server_Sub:  return config->subscription_url;
        case Server_Quantum: return config->quantum_url;
        case Server_Val: return config->validation_url;
        case Server_Saas: return config->saas_url;
        default:
            fprintf(stderr, "An invalid server product was specified.");
            return 0;
        break;
    }
}

static inline const char* _call_method_to_string( enum call_method method ) {
    switch ( method ) {
        case CallMethod_Get:  return "GET";
        case CallMethod_Post: return "POST";
        case CallMethod_Patch: return "PATCH";
        case CallMethod_Put: return "PUT";
        case CallMethod_Delete: return "DELETE";
        default:
            return 0;
    }
}


void xq_destroy_response(struct xq_response* response) {
    
    if (!response || response->size == 0 ) return;
    if (response->content ) free(response->content);
}

struct xq_response xq_call(
             struct xq_config* config,
             enum product_server server,
             enum call_method method,
             const char* serviceName,
             const char* payload,
             _Bool withToken,
             struct xq_multipart_set* files ) {
  
    struct xq_response response = {} ;
    
    CURL *curl;
    CURLcode code;
    curl_mime *form = NULL;
    struct curl_slist *headers = NULL;

    char url[MAX_URL_LENGTH] = {0};
    
    const char* host = _server_host_for_product(config, server);
    
    if (host == 0) {
        fprintf(stderr, "An invalid server product was specified.");
        return response;
    }
    
    /* get a curl handle */
    curl = curl_easy_init();
    
    if (!curl) {
        return response;
    }
    
    // Set the REST verb
    const char* method_str = _call_method_to_string(method );
    if (method_str == 0) {
        fprintf(stderr, "An invalid REST verb was specified.");
        return response;
    }
 
    {
        char* tail = xq_strcat(xq_strcat(xq_strcat(url,(char*) host, MAX_URL_LENGTH), "/", 0),(char*)serviceName, 0);
        if ( method == CallMethod_Get && payload ) {
            xq_strcat( xq_strcat(tail, "?", 0 ) ,(char*) payload, 0 );
        }
    }
    

    // Set the target URL
    code = curl_easy_setopt(curl, CURLOPT_URL, url );
    if (code != CURLE_OK) {
        fprintf(stderr, "Failed to assign the target URL");
        curl_easy_cleanup(curl);
        return response;
    }

    // If this is a request to the saas server, use the dashboard API key ( if set ).
    if ( server == Server_Saas && config->dashboard_api_key ) {
        headers = curl_slist_append(headers, config->dashboard_api_key );
        if (headers == NULL) {
            fprintf(stderr, "Failed to assign dashboard API key.");
            return response;
        }
    }
    
    // Otherwise, use the XQ API key ( if set ).
    else if (config->xq_api_key) {
        headers = curl_slist_append(headers, config->xq_api_key );
        if (headers == NULL) {
            fprintf(stderr, "Failed to assign XQ API key.");
            return response;
        }
    }
    
    if (withToken) {
        if ( server == Server_Saas ) {
            if (config->dashboard_token) {
                headers = curl_slist_append(headers, config->dashboard_token );
                if (headers == NULL) {
                    fprintf(stderr, "Failed to assign dashboard token.");
                    return response;
                }
            }
        }
        else if (config->access_token) {
            headers = curl_slist_append(headers, config->access_token );
            if (headers == NULL) {
                fprintf(stderr, "Failed to assign XQ token.");
                return response;
            }
        }
    }

    /* set our set of headers */
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // If this a payload is specified, add it here.
    if (method == CallMethod_Get) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    }
    else if (payload) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload );
    }
    
    
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method_str );
    
    
    // If any files are specified, add it here.
    if ( files != 0 ) {
        //...
    }
    
    // Setup the function for receiving data.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_memory_callback );
    
    // Setup the data structure that will contain received data.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response );
    
    // Set follow location
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    
#ifdef ENABLE_CURL_DEBUG
    
    struct data dbgcfg;
    
    dbgcfg.trace_ascii = 1; /* enable ascii tracing */
    
    
    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &dbgcfg);
    /* the DEBUGFUNCTION has no effect until we enable VERBOSE */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    
#endif
    
    
    /* Perform the request, code will get the return code */
    code = curl_easy_perform(curl);
    
    if (code != CURLE_OK) {
        response.success = 0;
        //fprintf(stderr, "Connection failed with error code: %i", code );
        response.size = 50;
        response.content = calloc(response.size, 1);
        snprintf((char*)response.content, 100, "Connection failed with error code: %i", code);
        
    }
    
    else {
        /* Fetch the response code */
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.responseCode);
        response.success = (response.responseCode >=200 && response.responseCode <= 204 );
    }
    

    /* then cleanup the form */
    if (form != 0)  curl_mime_free(form);
    /* free slist */
    if (headers != 0)  curl_slist_free_all( headers );
    
    /* always cleanup */
    curl_easy_cleanup(curl);

    return response;
    
}
