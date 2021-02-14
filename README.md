**XQ C Library **

This is the XQ SDK for native applications and embedded devices.

## Installation

1.  Install Prerequisites (Certain distros will have some of these installed already):
   **Linux**

    ```shell
    sudo apt install openssl curl libcurl4-openssl-dev
    ```
   **MacOS**
   Mac users will need to make sure that they have the following installed:

- Xcode 10.x +
- cmake: `brew install cmake`
- OpenSSL 1.1: `brew install openssl@1.1`

2. You will need to add your XQ API key to the `xq.ini` file in the `config` folder. [Visit your dashboard](https://manage.xqmsg.com) to generate new keys.
3. Build the XQ library itself:

```shell
mkdir build
cd build
cmake .. && make
```

## Basic Usage

### 1. SDK Initialization

The SDK must be initialized with a valid configuration before it can be used. A basic `xq.ini` configuration file is provided in the `config` folder. You will need to add your XQ API key to this file. [Visit your dashboard](https://manage.xqmsg.com) to generate new keys.

```c
#include <xq/xq.h>

struct xq_config cfg = xq_init( "path/to/xq.ini"  );
if (!xq_is_valid_config(&cfg) ) {
	// If something went wrong, call this to clean up
	// any allocated memory.
	xq_destroy_config(&cfg);
	exit(EXIT_FAILURE);
}
// Success - Configuration should be safe to use from this point.
// Delete config objects once they are no longer needed.
xq_destroy_config(&cfg);
```

### 2. Creating a Quantum Entropy Pool

A user can optionally create a quantum pool in order to improve performance. When a quantum pool is available, entropy will be pulled from that pool each time data is encrypted instead of needing to make a server request. The pool will be automatically replenished when necessary.

```c
#include <xq/xq.h>

struct xq_quantum_pool pool = {0};
struct xq_error_info err = {0};

// Initialize a 2048 byte quantum pool.
if (!xq_init_pool(&cfg, 2048, &pool, &err) ) {
	fprintf(stderr, "%li, %s", err.responseCode, err.content );
	exit(EXIT_FAILURE);
}

// Success - Pool is ready for use.
```

### 3. Destroying a Quantum Entropy Pool

Entropy pools should always be destroyed when they are no longer of use.

```c
	xq_destroy_pool(&pool);
```

### 4. Authenticating a User 

Before data can be encrypted or decrypted, a user must be have a valid access token. 

```c
// 1. Requesting access to XQ:
const char* email_address = "john@email.com";
if  (!xq_svc_authorize( &cfg, email_address, &err )) {
	fprintf(stderr, "%li, %s", err.responseCode, err.content );
	exit(EXIT_FAILURE);
}
// Success - A preauthorization token will be stored in memory.
// This will be replaced with a real authorization token once the user
// has successfully validated their email address by clicking on the link  // they received.
```

An email with an authorization link and a PIN code will be sent to the email address provided in the request. From this point on, a user has two ways of obtaining a valid access token:

  **i. Clicking Confirmation Link**
After clicking on the confirmation link in the email, the user can then exchange their preauthorization token ( received in the previous step ) for a valid access token:

```c
// 2. Exchange a preauthorization token for a valid access token:
if  (!xq_svc_exchange( &cfg, &err )) {
	fprintf(stderr, "%li, %s", err.responseCode, err.content );
	exit(EXIT_FAILURE);
}
// Success - Account is authorized.
```

  **ii. Entering the PIN code**
Alternatively, the PIN code received can be used to validate account ownership. **Note that this method will fail if the authorization link has already been clicked**. The access token will automatically be saved once the PIN has been validated:

```c
// Exchanging your PIN for a valid access token:
const char* pin = "123456";
if ( !xq_svc_code_validation( &cfg, pin , &err ) ) {
	fprintf(stderr, "%li, %s", err.responseCode, err.content );
	exit(EXIT_FAILURE);
}
// Success - Account is now authorized. Future attempts to authorize this PIN will fail.
```


### 5. Viewing your Access Token
An access token can be stored and reused for as long as it is valid. If this is done, ensure that it is stored in a manner that only authorized users have access to it.
```c

// Retrieving your access token
const char* access_token = xq_get_access_token(&cfg);
if ( !access_token ){
	exit(EXIT_FAILURE);
}

// Loading an access token
if (!xq_set_access_token(&cfg, access_token)){
	exit(EXIT_FAILURE);
}

```


### 6. Encrypting a Message

The most straightforward way to encrypt a mesage is by using the `xq_encrypt_and_store_token` method:

```c
// Encrypting a message

const char* message = "Hello World";
const char* recipients = "john@email.com,jane@email.com";

struct xq_message_payload result = { 0,0 };

if (!xq_encrypt_and_store_token( 
	&cfg, // XQ Configuration object
	Algorithm_OTP, // The algorithm to use for encryption
	(uint8_t*)message,  // The message to encrypt.
	strlen(message), // The length of the message ( in bytes )
	64,  // The number entropy bytes to use.
	recipients, // The accounts that will be able to read this message.
	24, // The number of hours this message will be available
	0, // Prevent this message from being read more than once?
	&result,
	&err)) {
		fprintf(stderr, "%li, %s", err.responseCode, err.content );
		exit(EXIT_FAILURE);
	}

// Success. The message has been successfully encrypted. The 
// encrypted message will be stored in "result.data". 
// Here, we will convert the result to base64 in order to display onscreen.
struct xq_message_payload encoded = { 0, 0 };
xq_base64_payload(&result, &encoded);
// Display the encrypted message.
fprintf(stdout, "Encrypted Message ( Base64 ):%s\n", encoded.data );
// Display the XQ locator token. 
fprintf(stdout, "Token: %s\n", result.token_or_key);

// Cleanup
xq_destroy_payload(&encoded);
xq_destroy_payload(&result);
```

### 7. Decrypting a Message
Encrypted data can be decrypted via `xq_decrypt_with_token`:

```c

// The encrypted message should be exactly the same as
// the one originally generated.
const uint8_t* encrypted = ".....";
size_t encrypted_len = LENGTH_IN BYTES;
struct xq_message_payload decrypted = { 0,0 };

if (!xq_decrypt_with_token(
  &cfg, 
  Algorithm_Autodetect, // The original algorithm (or autodetect)
  encrypted,  // The encrypted payload
  encrypted_len,  // The length of the encrypted payload
  token, // The XQ locator token
  &decrypted, 
  &err)){
	fprintf(stderr, "%li, %s", err.responseCode, err.content );
	exit(EXIT_FAILURE);
}

// Success - The message has been successfully encrypted.
fprintf(stdout, "Decrypted Message:%s\n", decrypted.data );
fprintf(stdout, "Decrypted Length:%i\n", decrypted.length );
xq_destroy_payload(&decrypted);
```

### 8. Revoking a Message
All valid messages can be revoked, provided that you are the original sender, and you have a valid token:

```c

struct xq_error_info err;
struct xq_message_payload result = { 0,0 };
if (!xq_svc_remove_key(cfg, token, &err)) {
  fprintf(stderr, "%li, %s", err.responseCode, err.content );
  exit(EXIT_FAILURE);
}
xq_destroy_payload(&result);

```