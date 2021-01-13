//
//  decrypt.c
//  xqc
//
//  Created by Ike E on 10/21/20.
//

#include <stdio.h>

#include <time.h>
#include <stdlib.h>

#include <xq/config.h>
#include <xq/connect.h>
#include <xq/services/quantum/quantum.h>
#include <xq/services/sub/packet.h>
#include <xq/services/crypto.h>
#include <xq/algorithms/otp/otp_decrypt.h>
#include <xq/algorithms/aes/aes_decrypt.h>
#include <xq/algorithms/nist/nist_decrypt.h>


_Bool xq_decrypt_with_token( struct xq_config* config, enum algorithm_type algorithm,  uint8_t* data,  size_t data_len, char* token, struct xq_message_payload* result, struct xq_error_info* error   ) {
    
    // 1st. fetch the key from the validation server using the provided token.
    char keyBits[MAX_QUANTUM_SIZE] = {0};
    struct xq_key key = { keyBits ,0};
    _Bool success = xq_svc_get_key(config, token, &key, error);
    
    if ( !success ) {
        printf("Failed to get key.\n");
        return 0;
    }
    
    success = xq_decrypt_with_key(config, algorithm, data, data_len, key.data, result, error);
    return success;
    
}

_Bool xq_decrypt_with_key( struct xq_config* config, enum algorithm_type algorithm,  uint8_t* data,  size_t data_len, char* key, struct xq_message_payload* result, struct xq_error_info* error   ) {
    
    _Bool success = 0;
    
   
    if (algorithm == Algorithm_Autodetect) {
        
        printf("Attempting to autodetect algorithm...\n");
        
        // If the first character is a period, the next should indicate the algorithm type.
        if (key[0] == '.' ) {
            // Compatibility and strong AES prefixes
            if (key[1] == 'A' || key[1] == 'D'  ) algorithm = Algorithm_AES;
            // NIST prefix
            else if  (key[1] == 'N') algorithm = Algorithm_NIST;
            // OTP by default.
            else algorithm = Algorithm_OTP;
        }
        // OTP by default.
        else algorithm = Algorithm_OTP;
    }
    
    switch (algorithm) {
            
        case Algorithm_OTP: {
            success = xq_otp_decrypt(data, data_len, key, result, error);
        }
        break;
            
        case Algorithm_AES: {
            success = xq_aes_decrypt(data, data_len, key, result, error);
        }
        break;
            
        case Algorithm_NIST: {
            success = xq_nist_decrypt( NULL , data, data_len, key, result, error);
        }
        break;
            
        default: break;
    }
    
    return success;
    
}
