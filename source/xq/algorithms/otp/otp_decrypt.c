//
//  otp_decrypt.c
//  xqc
//
//  Created by Ike E on 10/21/20.
//

#include <stdio.h>
#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <xq/config.h>
#include <xq/services/quantum/quantum.h>
#include <xq/services/crypto.h>
#include <xq/algorithms/otp/otp_decrypt.h>

_Bool xq_otp_decrypt(
                     uint8_t* data, size_t data_len,
                     char* key,
                     struct xq_message_payload* result,
                     struct xq_error_info* error   ) {
    

    if ( result == 0) {
        if (error) {
            xq_strcat(error->content, "No object was provided to store results" , MAX_ERROR_LENGTH);
            error->responseCode = 0;
        }
        return 0; // Fail
    }
    
    int key_offset =  (key[0] == '.') ? 2 : 0;
    int key_length = ((int)strlen(key)) - key_offset;
    if ( key_length <= 0 ) return 0;
    
    size_t max_length = data_len;// > key_length ? data_length : key_length;
    
    if ( result->length == 0 ) {
        result->data = malloc(  max_length + 1 );
    }
    
    else if (result->length < max_length ) {
        if (error) {
            xq_strcat(error->content, "The provided buffer is not large enough to hold result" , MAX_ERROR_LENGTH);
            error->responseCode = 0;
        }
        return 0; // Fail
    }
    
    
    
    register int m_idx = 0,k_idx = 0;
    do {
        k_idx = key_offset  + (m_idx % key_length);
        result->data[m_idx] = ((uint8_t)data[m_idx]) ^ ((uint8_t)key[k_idx]);
    } while (++m_idx < data_len );
    result->data[m_idx] = '\0';
    result->length = m_idx;
    return 1;
    
}
