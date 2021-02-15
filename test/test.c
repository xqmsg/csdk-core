//  A slightly simplified version of the starter tutorial for testing encryption functionality.
//  Unlike the starter tutorial, this runs without needing any user input.
//  test.c
//  test
//
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xq/xq.h>


_Bool testEncryption(struct xq_config *cfg, const char* recipients, const char* message, int algorithm) {
    
    struct xq_message_payload result = { 0,0 };
    struct xq_error_info err = {0};
    
    if (!xq_encrypt_and_store_token(
                                    cfg, // XQ Configuration object
                                    algorithm, // The algorithm to use for encryption
                                    (uint8_t*)message,  // The message to encrypt.
                                    strlen(message), // The length of the message ( in bytes )
                                    256,  // The number entropy bytes to use.
                                    0, // Entropy pool to use ( 0 if none ).
                                    recipients, // The accounts that will be able to read this message.
                                    24, // The number of hours this message will be available
                                    0, // Prevent this message from being read more than once?
                                    &result,
                                    &err)) {
        fprintf(stderr, "[xq_encrypt_and_store_token] %li: %s\n", err.responseCode, err.content );
        return 0;
    }
    
    struct xq_message_payload encoded = { 0, 0 };
    xq_base64_payload(&result, &encoded);
    // Display the encrypted message.
    printf( "-- Encrypted Message: %s\n", encoded.data );
    // Display the XQ locator token.
    printf( "-- Token: %s\n", result.token_or_key);
    xq_destroy_payload(&encoded);
    
    // The encrypted message should be exactly the same as
    // the one originally generated.
    struct xq_message_payload decrypted = { 0,0 };
    
    if (!xq_decrypt_with_token(
                               cfg,
                               Algorithm_Autodetect,
                               result.data,  // The encrypted payload
                               result.length,  // The length of the encrypted payload
                               result.token_or_key, // The XQ locator token
                               &decrypted,
                               &err)){
        fprintf(stderr, "[xq_decrypt_with_token] %li: %s\n", err.responseCode, err.content );
        xq_destroy_payload(&result);
        return 0;
    }
    
    // Success. The message has been successfully encrypted.
    printf( "-- Decrypted:%s\n", decrypted.data );
    
    xq_destroy_payload(&decrypted);
    xq_destroy_payload(&result);
    return 1;
}


int main(int argc, const char * argv[]) {
    
    
    // 1. SDK Initialization
    const char *config_file = argc > 1 ? argv[1] : "xq.ini";
    struct xq_config cfg = xq_init( config_file );
    if (!xq_is_valid_config(&cfg) ) {
        // If something went wrong, call this to clean up
        // any memory that was possibly allocated.
        xq_destroy_config(&cfg);
        exit(EXIT_FAILURE);
    }
    
    // 2. Create Quantum Pool
    struct xq_error_info err = {0};
    
    // 3. Authenticate a user.
    const char* email_address = argv[2];
    
    // If a real email address was set.
    if  (!xq_svc_authorize_alias( &cfg, email_address, &err )) {
        fprintf(stderr, "[xq_svc_authorize_alias] %li : %s", err.responseCode, err.content );
        xq_destroy_config(&cfg);
        exit(EXIT_FAILURE);
    }
    
    printf( "Alias Account authorized.\n");
    
    // Retrieving your access token
    const char* access_token = xq_get_access_token(&cfg);
    if ( !access_token ){
        fprintf(stderr, "[xq_get_access_token] Failed to get access token.\n");
        xq_destroy_config(&cfg);
        exit(EXIT_FAILURE);
    }
    
    char* token = strdup(access_token);
    
    if (!xq_set_access_token(&cfg, token)) {
        fprintf(stderr, "[xq_set_access_token] Failed to reset access token.\n");
        free(token);
        xq_destroy_config(&cfg);
        exit(EXIT_FAILURE);
    }
    
    printf( "Current Access Token: %s\n", token );
    
    free(token);
    
    // Retrieve information about this user.
    struct xq_subscriber_info info = {0};
    if (!xq_svc_get_subscriber(&cfg, &info, &err)) {
        fprintf(stderr, "[xq_svc_get_subscriber] %li: %s\n", err.responseCode, err.content );
        xq_destroy_config(&cfg);
        exit(EXIT_FAILURE);
    }
    
    // 6. Test OTP a new message
    const char* message = "Hello World From John Doe";
    printf( "Encrypting message: %s...\n", message);
    _Bool res = testEncryption( &cfg, info.mailOrPhone, message, Algorithm_OTP );
    printf("OTP Encryption: %s.\n", res ? "OK" : "Failed" );
    res = testEncryption( &cfg, info.mailOrPhone, message, Algorithm_AES );
    printf("AES Encryption (SHA 256): %s.\n", res ? "OK" : "Failed" );
    res = testEncryption( &cfg, info.mailOrPhone, message, Algorithm_AES_Strong );
    printf("AES Encryption (SHA 512, 100K Rounds): %s.\n", res ? "OK" : "Failed" );
    
    // Cleanup
    xq_destroy_config(&cfg);
    
    printf("Finished.\n");
    
    return 0;
    
}
