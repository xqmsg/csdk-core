//
//  quantum.h
//  xqcsdk
//
//  Created by Ike E on 10/18/20.
//

#ifndef quantum_h
#define quantum_h

#ifndef MAX_QUANTUM_SIZE
#define MAX_QUANTUM_SIZE 4096
#endif

#ifndef QUANTUM_POOL_SIZE
#define QUANTUM_POOL_SIZE 4096
#endif


/// Used for storing quantum data fetched from the server.
struct xq_quantum_key {
    /// The raw bit data in string format will be stored here ( e.g. 100100111 ).
    char* data;
    /// The number of bits stored.
    int length;
};

/// Manages a quantum pool.
/// This allows an application to pre-cache quantum data, so a server call does not need to be made every time quantum data is required.
struct xq_quantum_pool {
    
    /// The data pools. Two are always available - When one is exhausted, the application will switch to the other while replenishing the previous one for future use.
    char* pools[2];
    
    /// The size of each pool in bytes. This is also the number of bytes that will be pulled each time a pool is refilled.
    uint32_t size;
    
    /// The index to the currently active pool (either 0 or 1 ).
    short active_pool;
    
    /// The number of bytes that have been used in each pool ( by index ).
    uint32_t used[2];
};


/// Initializes a quantum pool. Not that this method itself is NOT thread-safe. Ideally, it should be called at the start of the application, or when  the application is guaranteed not
/// to be using the pool that is provided.
///
/// @param config The XQ configuration instance.
/// @param size The size of the pools to initialize. Cannot exceed the value that MAX_QUANTUM_SIZE is set to.
/// @param pool The pool to initialize. This pool must not be in use when this method is called.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_init_pool( struct xq_config* config,
                    int size,
                    struct xq_quantum_pool* pool,
                    struct xq_error_info* error );



/// Destroys an existing quantum pool.
/// @param pool The pool to destroy. This pool should not be in use when this method is called.
void xq_destroy_pool(struct xq_quantum_pool* pool );


/// Fetches quantum entropy from the XQ servers ( or an existing entropy pool if provided ).
/// @param config The XQ configuration instance.
/// @param key_out The required user-provided instance where the resulting key will be saved.
/// @param pool The entropy pool to use to fulfill this request. If not provided, entropy will be fetched directly from the server.
/// @param error An optional, user-provided block  to store details of any error that occurs.
_Bool xq_svc_quantum(   struct xq_config* config,
                        struct xq_quantum_key* key_out,
                        struct xq_quantum_pool* pool,
                        struct xq_error_info* error );


#endif /* quantum_h */
