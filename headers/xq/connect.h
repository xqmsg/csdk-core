//
//  connect.h
//  xqcsdk
//
//  Created by Ike E on 10/15/20.
//
// Declares functionality for connecting to

#ifndef connect_h
#define connect_h


#include <curl/curl.h>

struct xq_multipart {
    const char* name;
    FILE* value;
};

struct xq_multipart_set
{
    struct xq_multipart* part;
    unsigned count;
};

/*! Specifies an XQ backend product. Generally used to denote which server URL a call should be directed to.
 */
enum product_server {
    //! The XQ Subscription server, where XQ subscribers are managed.
    Server_Sub = 1,
    //! The XQ validation server, where message keys are managed.
    Server_Val = 2,
    //! The XQ quantum server.
    Server_Quantum = 3,
    //! The XQ dashboard server for workspace administration of XQ users.
    Server_Saas = 4
};

/// HTTP methods.
enum call_method {
    /// GET
    CallMethod_Get = 1,
    /// POST
    CallMethod_Post = 2,
    /// PATCH
    CallMethod_Patch = 3,
    /// PUT
    CallMethod_Put = 4,
    /// DELETE
    CallMethod_Delete = 5
};


/// Checks whether an xq config object is valid or not.
/// @param conf The configuration object to test.
_Bool xq_is_valid_config(struct xq_config* conf);

/// Internal XQ method that is responsible for making remote calls to XQ servers.
/// @param config The XQ configuration object.
/// @param server The type of server product to call (sub, val, quantum or saas ).
/// @param method The HTTP REST method type (Get, Post, etc).
/// @param serviceName The name of the service to call.
/// @param payload The data to be passed as the request body.
/// @param withToken Should an access token be sent ( if one is available ) ? This will be true in most cases.
/// @param multipart Any additional files to be passed into the request. If this object is defined, the request type will automatically be set to multipart ( curl_mime
struct xq_response xq_call( struct xq_config* config,
             enum product_server server,
             enum call_method method,
             const char* serviceName,
             const char* payload,
             _Bool withToken,
             struct xq_multipart_set* multipart
             );


/// Initialize XQ with a configuration file.
/// @param configPath The configuration file.
struct xq_config xq_init(const char* configPath);


/// Destroys an XQ configuration file.
///
/// This should be called to clean up any initialized xq_config object.
/// @param config The configuration object to clean up.
void xq_destroy_config(struct xq_config* config);


/// Destroy a generated response object.
///
/// Any and all response objects generate from xq_call should always be destroyed using this method, once all their data has been processed.
/// @param response The response object to destroy.
void xq_destroy_response(struct xq_response* response);

#endif /* connect_h */
