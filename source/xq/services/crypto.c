//
//  crypto.c
//  xqc
//
//  Created by Ike E on 10/21/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <xq/config.h>
#include <xq/services/quantum/quantum.h>
#include <xq/services/crypto.h>
#include <ext/base64/base64.h>

static int rand_int(int n) {
    int limit = RAND_MAX - RAND_MAX % n;
    int rnd;
    
    do {
        rnd = rand();
    } while (rnd >= limit);
    return rnd % n;
}

void shuffle(char *array, int n) {
    int i, j, tmp;
    for (i = n - 1; i > 0; i--) {
        j = rand_int(i + 1);
        tmp = array[j];
        array[j] = array[i];
        array[i] = tmp;
    }
}


void xq_destroy_quantum_key(struct xq_quantum_key* obj) {
    if (obj && obj->data) {
        free(obj->data);
        obj->data = 0;
    }
}

void xq_destroy_hex_quantum_key(struct xq_hex_quantum_key* obj) {
    if (obj && obj->data) {
        free(obj->data);
        obj->data = 0;
    }
}

void xq_destroy_payload(struct xq_message_payload* obj) {
    if (obj) {
        if (obj->data) {
            free(obj->data);
            obj->data = 0;
        }
        if (obj->token_or_key) {
            free(obj->token_or_key);
            obj->token_or_key = 0;
        }
    }
}

_Bool xq_expand_key( char* in_key, char* out_key, int new_len  ) {
    
    int original_len = (int) strlen(in_key);
    int current_len = original_len;

    // If the key is already a size we need, simply shuffle it.
    if ( current_len > new_len ) {
        memccpy( out_key, in_key, '\0', new_len );
        shuffle( out_key, new_len );
        return 1;
    }
    

    char* c = out_key; // + current_len;
    srand((unsigned int)time(NULL));
    current_len = 0;
    
    while (current_len < new_len)  {
        int remaining = new_len - current_len; // How many bytes are left to fill?
        
        if ( remaining > original_len ) {
            remaining = original_len;
        }
        memccpy(c, in_key, '\0', remaining);
        c += remaining;
        current_len += remaining;
    }
    
    shuffle( out_key , new_len );
    
    return 1;
}


void xq_bits_to_hex( char* bits_in, int in_len, char* hex_out, int out_len ) {
    
    char k[5] = {0};
    
    register int x = 0, p = 0, remaining = 0, value = 0;
    
    for ( x = 0 ; x < out_len; ++x ) {
        p = x * 4;
        if ( p >= in_len ) break;
        remaining = ( ( p + 4 ) >= in_len ) ? ( in_len - p ) : 4;
        memset(k, 0, 5);
        memccpy(k, bits_in + p , '\0', remaining );
        value = (int) strtol(k,NULL, 2 );
        hex_out[x] = ((uint8_t) value) + ((value < 10) ? 48 : 55);
    }
}


_Bool xq_key_to_hex(  struct xq_quantum_key* key_in, struct xq_hex_quantum_key* hex_out, char prefix ) {
    
    if (hex_out == 0 || key_in == 0) return 0;
    
    if (prefix == 0) {
        if (hex_out->data == 0) {
            if ( hex_out->length == 0 ) hex_out->length = (key_in->length / 4);
            hex_out->data = malloc( hex_out->length);
        }
        
        xq_bits_to_hex(key_in->data, key_in->length, hex_out->data, hex_out->length );
    }
    
    else {
        
        if (hex_out->data == 0) {
            if ( hex_out->length == 0 ) hex_out->length = (key_in->length / 4) + 2;
            else hex_out->length += 2;
            hex_out->data = malloc( hex_out->length + 1 );
            hex_out->data[hex_out->length] = 0;
        }

        hex_out->data[0] = '.';
        hex_out->data[1] = prefix;
        xq_bits_to_hex(key_in->data, key_in->length, hex_out->data + 2, hex_out->length - 2 );
    }
    
    return 1;
    
}


_Bool xq_base64_payload(struct xq_message_payload* in, struct xq_message_payload* out) {
    
    if (in == 0 || out == 0 ) return 0;
    
    if ( out->data == 0) {
        out->data = base64_encode( in->data, in->length, &out->length  );
        return (out->data != 0);
    }
    
    else {
        return base64_encode_with_buffer(in->data, in->length, out->data, &out->length );
    }
    
}
