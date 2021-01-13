#ifndef _BASE64_H_
#define _BASE64_H_

#ifdef __cplusplus
extern "C" {
#endif

uint8_t* base64_encode(const uint8_t *input, int len, uint32_t* out_len) ;
uint8_t* base64_decode(const uint8_t *input, int len, uint32_t* out_len ) ;

_Bool base64_encode_with_buffer(const uint8_t *input, int len, uint8_t *output, uint32_t* out_len );
_Bool base64_decode_with_buffer(const uint8_t *input, int len, uint8_t *output, uint32_t* out_len );

#ifdef __cplusplus
}
#endif

#endif //_BASE64_H_
