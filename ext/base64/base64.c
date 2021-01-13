#include <string.h>
#include <openssl/evp.h>
#include "base64.h"


uint8_t* base64_encode(const uint8_t *input, int len, uint32_t* out_len) {
    const int pl = ( ( len + 2 ) / 3 ) << 2;
    uint8_t *output = calloc( pl, 1 );
    const int ol = EVP_EncodeBlock(output, input, len );
    if (ol != pl) {
        free(output);
        fprintf(stderr, "Encode error:  %d ( expected ) != %d (actual) \n", pl, ol);
        return 0;
    }
    if (out_len) *out_len = pl;
    return output;
}

_Bool base64_encode_with_buffer(const uint8_t *input, int len, uint8_t *output, uint32_t* out_len ) {
    
    const int pl = ( ( len + 2 ) / 3 ) << 2;
    
    if ( pl > *out_len ) {
        fprintf(stderr, "Not enough memory was provided in the base64 buffer (%d < %d)\n", *out_len, pl);
        return 0;
    }
    
    const int ol = EVP_EncodeBlock(output, input, len);
    if (ol != pl) {
        fprintf(stderr, "An invalid number of bytes were encoded ( %d != %d)\n", pl, ol);
        return 0;
    }
    return 1;
    
}

uint8_t* base64_decode(const uint8_t *input, int len, uint32_t* out_len) {
    const int pl = (3 * len) >> 2 ;
    unsigned char *output = calloc(1,pl+1);
    const int ol = EVP_DecodeBlock(  output, input, len);
    if (pl != ol) {
        free(output);
        fprintf(stderr, "An invalid number of bytes were decoded ( %d != %d)\n", pl, ol);
        return 0;
    }
    if (out_len) *out_len = pl;
    return output;
}

_Bool base64_decode_with_buffer(const uint8_t *input, int len, uint8_t *output, uint32_t* out_len ) {
    
    const int pl = (3 * len) >> 2 ;
    
    if ( (pl+1) > *out_len ) {
        fprintf(stderr, "Not enough memory was provided in the base64 buffer (%d < %d)\n", *out_len, (pl + 1));
        return 0;
    }
    
    const int ol = EVP_DecodeBlock(  output, input, len);
    if (pl != ol) {
        fprintf(stderr, "An invalid number of bytes were decoded ( %d != %d)\n", pl, ol);
        return 0;
    }
    return 1;
}

