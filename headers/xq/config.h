//
//  config.h
//  xqc
//
//  Created by Ike E on 10/16/20.
//

#ifndef config_h
#define config_h

#ifndef BEARER_TAG
#define BEARER_TAG "Authorization:Bearer "
#define BEARER_TAG_LEN 24
#endif

#ifndef APIKEY_TAG
#define APIKEY_TAG "Api-Key: "
#endif

#ifndef STATUS_BUFFER_SIZE
#define STATUS_BUFFER_SIZE 512
#endif

#ifndef STATUS_BUFFER_SIZE
#define ACCESS_TOKEN_BUFFER 512
#endif

#ifndef MAX_URL_LENGTH
#define MAX_URL_LENGTH 512
#endif

#ifndef MAX_ERROR_LENGTH
#define MAX_ERROR_LENGTH 512
#endif

#include <stdint.h>

/// The default structure returned from all xq server calls.
struct xq_response {
  /// The raw content returned from the server.
  uint8_t *content;
  /// The size of the content in bytes.
  size_t size;
  /// The HTTP response code that was received.
  long responseCode;
  /// Logs whether the call succeed or not ( i.e. returned a 200 code ).
  _Bool success;
};

/// A simple object to store error response if any.
struct xq_error_info {
  /// The error response content. Must be less than 512 characters.
  char content[512];
  /// The error response code.
  long responseCode;
};

/*! This object contains all the information necessary to communicate properly
 * with an XQ instance.
 */
struct xq_config {
  //! The URL of the subscription server.
  char *subscription_url;
  char *validation_url;
  char *quantum_url;
  char *saas_url;
  char *xq_api_key;
  char *dashboard_api_key;
  int timeout_secs;
  _Bool support_threads;
  char *access_token;
  char *exchange_token;
  char *dashboard_token;
};

/// The different types of supported metadata. This affects the data that will
/// be logged to the XQ dashboard.
enum metadata_type {
  /// Generic metadata. This will not be logged as a communication in the
  /// dashboard.
  Metadata_Generic = 0,
  /// File metadata
  Metadata_File = 1,
  /// Email metadata.
  Metadata_Email = 2,
};

/// The different dashboard user roles
enum xq_business_role {
  role_admin = 1,

  /// Can login to dashboard with restricted permissions ( cannot add other
  /// users ).
  role_user = 2,

  /// Cannot login to dashboard.
  role_customer = 3,

  /// Cannot login to dashboard.
  role_vendor = 4,

  /// Can do everything on an account, including billing.
  role_superuser = 5,

  /// A virtual user is a generated email address that is fully tracked, but
  /// cannot be used to log into the dashboard at all.
  role_device = 6,

  /// An Alias role is similar is an anoymous alias that is trackable by an SMB.
  role_alias = 7
};

/// A data structure containing metadata about the key packet that will be
/// available on the users XQ dashboard.
struct xq_metadata {
  /// The metadata type. 1 = File, 2 = Email, 0 = Generic / Other
  enum metadata_type type;
  /// The total length of the metadata.
  int length;
  /// The stringified JSON metada object.
  char *data;
};

/// Appends one string to the tail end  of another. The user is responsible for
/// ensuring that the target string has enough memory allocated for the append.
/// @param tail The tail-end of the target string.
/// @param src The string to append.
/// @param maxBuf The length of the string to append.
char *xq_strcat(char *tail, char *src, int maxBuf);

/// Set the access token for the provided configuration.
/// @param config The XQ configuration object
/// @param token The access token.
_Bool xq_set_access_token(struct xq_config *config, const char *token);

/// Set the dashboard access token for the provided configuration.
/// @param config The XQ configuration object
/// @param token The access token.
_Bool xq_set_dashboard_token(struct xq_config *config, const char *token);

/// Fetches the value of the current access token for the provided configuration
/// object.
/// @param config The XQ configuration object.
const char *xq_get_access_token(struct xq_config *config);

/// Set the exchange token for the provided configuration object.
/// @param config The XQ configuration object.
/// @param token The exchange token.
/// @param len The length of the exchange token.
_Bool set_exchange_token(struct xq_config *config, const char *token, int len);

/// Returns a url-encoded version of string. The returned string must be freed
/// by the user when no longer needed.
/// @param str The string value to escape.
char *xq_escape(char *str);

/// Creates an error object based on the content of the response. This assumes
/// that the response has already been determined to be an error ( usually based
/// off of the HTTP error code ).
/// @param response The JSON response object containing the error information.
/// @param error The object to be filled with the error information.
_Bool xq_fill_error(struct xq_response *response, struct xq_error_info *error);

/// function to reverse buffer[i..j]
char *reverse(char *buffer, int i, int j);

/// Iterative function to implement itoa() function in C
char *itoa(int value, char *buffer, int base);

#endif /* config_h */
