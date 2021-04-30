# XQ C SDK

[![version](https://img.shields.io/badge/version-0.1-green.svg)](https://semver.org)

This is the XQ SDK for native applications and embedded devices.

## 1. Installation

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

	In order to run tests, macOS users may also need to update the OpenSSL path in the `CMakeList.txt` file (currently set `1.1.1k` ) to match their installed version.

2. You will need to add your XQ API key to the `xq.ini` file in the `config` folder. [Visit your dashboard](https://manage.xqmsg.com) to generate new keys.
3. Build the XQ library itself:

```shell
mkdir build
cd build
cmake .. && make
```

4. Build the tests if desired. 

```shell
make test
cd bin
./test
```

## 2. Basic Usage

### SDK Initialization

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

### Creating a Quantum Entropy Pool

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

### Destroying a Quantum Entropy Pool

Entropy pools should always be destroyed when they are no longer of use.

```c
	xq_destroy_pool(&pool);
```

### Authenticating a User 

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


### Viewing your Access Token
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


### Encrypting a Message

The most straightforward way to encrypt a mesage is by using the `xq_encrypt_and_store_token` method:

```c
// Encrypting a message

const char* message = "Hello World";
const char* recipients = "john@email.com,jane@email.com";

// The metadata be used to identify this communication on the dashboard.
// This may also be created without heap allocation via xq_use_metadata.
struct xq_metadata meta = xq_new_email_metadata("Hello!");

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
  &meta, // The message metadata. 
	&result,
	&err)) {
		fprintf(stderr, "%li, %s", err.responseCode, err.content );
 		// Since our metadata was allocated on the heap, we will need to destroy it.
  	xq_destroy_metadata(&meta);
		exit(EXIT_FAILURE);
	}

// Success. The message has been successfully encrypted. The 
// encrypted message will be stored in "result.data". 
// Here, we will convert the result to base64 in order to display onscreen.
struct xq_message_payload encoded = { 0, 0 };
xq_base64_payload(&result, &encoded);
// Display the encrypted message.
printf("Encrypted Message ( Base64 ):%s\n", encoded.data );
// Display the XQ locator token. 
printf("Token: %s\n", result.token_or_key);

// Cleanup
xq_destroy_metadata(&meta);
xq_destroy_payload(&encoded);
xq_destroy_payload(&result);
```

### Decrypting a Message
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
printf("Decrypted Message:%s\n", decrypted.data );
printf("Decrypted Length:%i\n", decrypted.length );
xq_destroy_payload(&decrypted);
```

### Granting and Revoking User Access

There may be cases where a user needs to grant or revoke access to a previously sent message. This can be achieved via `xq_svc_grant_access` and xq_svc_revoke_access respectively:

```c

// The existing message token.
char* token = "MY_MESSAGE_TOKEN";

// Add Jim and Joe to the recipient list.
const char *emailsToAdd[] = {"jim@email.com","joe@email.com" };

// Grant access to jim@email.com. This will only work if the currently
// active profile is the same one that sent the original message.

if ( !xq_svc_grant_access(cfg, token, emailsToAdd, 2, &err)) {
        fprintf(stderr, "%li: %s\n", err.responseCode, err.content );
        exit(EXIT_FAILURE);
    }

// Success - Jim and Joe will now be able to read the original message.

// Revoke access for Joe alone.
const char* emailsToRemove = { "joe@email.com" };

if ( !xq_svc_revoke_access(cfg, token, emailsToRemove, 1, &err)) {
  fprintf(stderr, "%li: %s\n", err.responseCode, err.content );
  exit(EXIT_FAILURE);
}

// Success - Joe's access to this message has been revoked.
```

### Revoking a Message
Access to an entire message can also be revoked. When this occurs, both the sender and all the recipients will lose all access to it. **Note that this action is not reversible**:

```c

struct xq_error_info err;

if (!xq_svc_remove_key(cfg, token, &err)) {
  fprintf(stderr, "%li, %s", err.responseCode, err.content );
  exit(EXIT_FAILURE);
}

```

### Connecting to the Dashboard
The SDK provides limited functionality for dashboard administration. Before using any dashboard-specific features, a user would perform the following **after** signing into XQ with an authorized email account associated with the management portal:

```c

struct xq_error_info err;

// The ID of the dashboard team.
// The authenticated email account must be associated with this team.
long team_id = 1; 

if (!xq_connect_dashboard(&cfg, team_id, &err)) {
            fprintf(stderr, "%li : %s\n", err.responseCode, err.content );
            xq_destroy_config(&cfg);
            exit(EXIT_FAILURE);
        }

// Success - The dashboard account is authorized.
```

### Admin: Managing a User Group
Users may group a number of emails accounts under a single alias. Doing this makes it possible to add all of the associated email accounts to an outgoing message by adding that alias as a message recipient instead. Note that changing the group members does not affect the access rights of messages that have previously been sent.

```c


struct xq_error_info err;

// Creating the group
char* emails_in_group = { "test1@email.com", "test2@email.com" };
struct xq_user_group new_group = {
                0, // Group ID will be not be used here,
                "My List", // An alias for identifying this group on the dashboard
                emails_in_group,  // The list of email addresses to add
                2 // The number of emails in the group
            };

long group_id = xq_add_user_group(&cfg, &new_group, &err);

if (group_id == 0 ){
	fprintf(stderr, "%li : %s\n", err.responseCode, err.content );
	exit(EXIT_FAILURE);
}

// Success - Adding the recipient <group_id>@group.local will now make all of the group members valid recipients of the message.

```

### Admin: Creating a Dashboard Contact 
The SDK allows a user to create an additional contact on their dashboard, provided their role on the dashboard allows them to do so. All roles besides **Device** and **Alias** will require the the contact to accept an invitation before they will be fully trackable: 

```c

struct xq_error_info err;

// Attempt to create a contact.
const char* mail = "john@email.com";
long contact_id = xq_add_contact(
	&cfg, 
	mail,  // User Identifier
	"John", // First Name 
	"Doe",  // Last Name
	"Employee",  // Title
	role_user,  // Business Role
	all_notifications,  // Notification Level
	&err
	);
        
// A zero contact ID means the request failed.
if ( contact_id == 0 ) {
	fprintf(stderr, "[xq_add_ext_contact] %li : %s\n", err.responseCode, err.content );
	exit(EXIT_FAILURE);
}

// Success - the dashboard contact was created.

```

### Admin: Using an external ID-based contact for tracking
In situations where a user may want to associate an external account with an XQ account for the purposes of encryption and tracking , they can choose to create an account with an **Alias** role.

These type of accounts will allow user authorization using only an account ID. However, these accounts have similar restrictions to anonymous accounts: They will be incapable of account management, and also have no access to the dashboard. However - unlike basic anonymous accounts - they can be fully tracked in a dashboard portal.
```c

struct xq_error_info err;

const char* external_id = "1000101";

long contact_id = xq_add_contact(
	&cfg, 
	external_id, 
	"John", 
	"Doe", 
	"External", 
	role_alias, // Make sure to set the Alias role 
	no_notifications,  // Aliases cannot receive notifications.
	&err
	);
        
if ( contact_id == 0 ) {
	fprintf(stderr, "[xq_add_ext_contact] %li : %s\n", err.responseCode, err.content );
	exit(EXIT_FAILURE);
}

// Success - the dashboard contact was created.
```

Afterwards, if a user signs in with that external ID via **authorize_alias**, their activity can be fully tracked on the dashboard:

```c

// Sign in using the external ID as the alias.

struct xq_error_info err;

const char* external_id = "1000101";
if  (!xq_svc_authorize_alias( &cfg, external_id, &err )) {
	fprintf(stderr, "[xq_svc_authorize_alias] %li : %s", err.responseCode, err.content );
            xq_destroy_config(&cfg);
            exit(EXIT_FAILURE);
}

// Success - This user now has a restricted access token capable of encryption and decryption. Any actions performed will be logged on the associated dashboard.
        
```

