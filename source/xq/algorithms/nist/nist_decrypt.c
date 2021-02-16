//
//  nist_decrypt.c
//  xqc
//
//  Created by Ike E on 10/22/20.
//

#include <stdio.h>
#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <xq/config.h>
#include <xq/services/quantum/quantum.h>
#include <xq/services/sub/packet.h>
#include <xq/services/crypto.h>
#include <xq/algorithms/nist/nist_decrypt.h>


_Bool xq_nist_decrypt(  char* nist_library_name,
                      uint8_t* data,
                      size_t data_len,
                      char* key,
                      struct xq_message_payload* result,
                      struct xq_error_info* error   ) {

    fprintf(stderr,"NOT IMPLEMENTED\n");
    return 0;
    
}
