//
//  quantum.c
//  xqc
//
//  Created by Ike E on 10/18/20.
//

#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <xq/config.h>
#include <xq/connect.h>
#include <ext/jwrite/jWrite.h>
#include <ext/base64/base64.h>
#include <xq/services/quantum/quantum.h>
#include <xq/services/sub/authorize.h>
#include <xq/services/crypto.h>

struct quantum_refill_thread {
    pthread_mutex_t  pool_lock;
    pthread_attr_t attr;
    pthread_t pthread;
    struct xq_config* cfg;
    struct xq_quantum_pool* pool;
    _Bool running;
};

static struct quantum_refill_thread _quantum_refill_thread = { PTHREAD_MUTEX_INITIALIZER, 0, 0, 0, 0, 0 };


void xq_destroy_pool(struct xq_quantum_pool* pool ) {
    if (pool) {
        if (pool->pools[0] ) free( pool->pools[0]  );
        if (pool->pools[1]) free( pool->pools[1]  );
        memset(pool, 0, sizeof(struct xq_quantum_pool));
    }
}


static void* refill_pool(void* args) {
    
    // If the pool is already being refilled, ignore.
    int rc = pthread_mutex_trylock(&_quantum_refill_thread.pool_lock);
    
    struct quantum_refill_thread* params = (struct quantum_refill_thread*) args;
    
    int fill_index = params->pool->active_pool ;
    
    // Switch to the alternate pool before refilling this one.
    params->pool->active_pool = (fill_index + 1) % 2;

    struct xq_quantum_key key_out = { params->pool->pools[fill_index], params->pool->size };
    if ( xq_svc_quantum(params->cfg , &key_out, 0, 0 )) {
        params->pool->used[ fill_index ] = 0;
        
    }
    rc = pthread_mutex_unlock(&_quantum_refill_thread.pool_lock);
    
    return 0;
}

_Bool xq_init_pool( struct xq_config* config, int size, struct xq_quantum_pool* pool,  struct xq_error_info* error ) {
    
    if (config == 0 ) return 0;
    
    if ( pool == 0) {
        if (error) {
            xq_strcat(error->content, "A pool instance is required " , MAX_ERROR_LENGTH);
            error->responseCode = 0;
        }
        return 0; // Fail
    }
    
    // Create thread variables
    if (pthread_attr_init(&_quantum_refill_thread.attr ) != 0 ) {
        perror("pthread_attr_init");
        exit(1);
    }
    
    xq_destroy_pool(pool);

    pool->size = size;
    
    for (int x = 0; x < 2; ++x ) {
        
        pool->pools[x] = malloc(size);
        
        struct xq_quantum_key key_out = { pool->pools[x], size };
        
        if (!xq_svc_quantum(config, &key_out, 0, error )) {
            xq_destroy_pool(pool);
            return 0;
        }
        pool->used[x] = 0;
    }
   
    pool->active_pool = 0;
    
    return 1;
}

_Bool xq_svc_quantum(struct xq_config* config,  struct xq_quantum_key* key_out,  struct xq_quantum_pool* pool,  struct xq_error_info* error ) {
    
    if ( key_out == 0) {
        if (error) {
            xq_strcat(error->content, "A quantum key object is required " , MAX_ERROR_LENGTH);
            error->responseCode = 0;
        }
        return 0; // Fail
    }
    
    if (key_out->length > MAX_QUANTUM_SIZE) {
        if (error) {
            char* tail = xq_strcat(error->content, "Exceeded max quantum size of " , MAX_ERROR_LENGTH);
            itoa(MAX_QUANTUM_SIZE, tail, 10);
            error->responseCode = 0;
        }
        return 0; // Fail
    }
    
    if (key_out->length == 0) key_out->length = 256;
    int required = key_out->length;
    
    if ( pool ) {
        
        // Check whether the current pool has enough entropy for us.
        int available = pool->size - pool->used[ pool->active_pool ];

        // If enough entropy is available, copy it to the key.
        if (available >= required ) {
            
            if (key_out->data == 0) key_out->data = malloc( required );
            memccpy( key_out->data, pool->pools[pool->active_pool] +  pool->used[ pool->active_pool ] ,'\0', required );
            pool->used[pool->active_pool] += required;
            
            // If the pool has been used up, switch to the alternate pool, and refill the empty one.
            if (pool->used[pool->active_pool] >= pool->size ) {
                _quantum_refill_thread.pool = pool;
                _quantum_refill_thread.cfg = config;
                pthread_create(&_quantum_refill_thread.pthread, &_quantum_refill_thread.attr, refill_pool, &_quantum_refill_thread );
            }
            
            return 1;
        }
        
        else if ( available > 0  ) {
            
            int needed = required - available;
            if (key_out->data == 0) key_out->data = malloc( required );
            memccpy( key_out->data, pool->pools[pool->active_pool] +  pool->used[ pool->active_pool ] ,'\0', available );
            pool->used[pool->active_pool] += available;
            
            // If the pool has been used up, switch to the alternate pool, and refill the empty one.
            if (pool->used[pool->active_pool] >= pool->size ) {
                _quantum_refill_thread.pool = pool;
                _quantum_refill_thread.cfg = config;
                pthread_create(&_quantum_refill_thread.pthread, &_quantum_refill_thread.attr, refill_pool, &_quantum_refill_thread );
            }
            
            // Write the email into a json payload.
            char params[16] = {0};
            
            // Create the query parameters
            char* tail = xq_strcat( params, "raw=1&ks=", MAX_URL_LENGTH );
            itoa(needed, tail, 10);
            
            struct xq_response response = xq_call( config, Server_Quantum, CallMethod_Get, "", params, 1,  0 );
            
            if (response.success) {
                // If for some reason we did not get the number of bytes we wanted, fail the operation.
                if ( needed != response.size ) {
                    xq_strcat(error->content, "Failed to get the desired number of bytes " , MAX_ERROR_LENGTH);
                    response.success = 0;
                    error->responseCode = -1;
                }
                else {
                    memccpy(key_out->data + available, response.content,'\0', needed );
                }
            }
            
            xq_destroy_response(&response);
            return response.success;
        }
    }
    
    // Write the email into a json payload.
    char params[16] = {0};
    
    // Create the query parameters
    char* tail = xq_strcat( params, "raw=1&ks=", MAX_URL_LENGTH );
    itoa(required, tail, 10);
    
    struct xq_response response = xq_call( config, Server_Quantum, CallMethod_Get, "", params, 1,  0 );
    
    if (response.success) {
        
        // If for some reason we did not get the number of bytes we wanted, fail the operation.
        if (key_out->length != response.size ) {
            xq_strcat(error->content, (char*) response.content , MAX_ERROR_LENGTH);
            response.success = 0;
            error->responseCode = -1;
        }
        else {
            if (key_out->data == 0) key_out->data = malloc(response.size);
            memccpy(key_out->data, response.content,'\0',response.size );
        }
    }
    
    xq_destroy_response(&response);
    return response.success;
}
