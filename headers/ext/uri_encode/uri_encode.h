#ifndef _URI_ENCODE_H_
#define _URI_ENCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

size_t uri_encode (const char *src, const size_t len, char *dst);

size_t uri_decode (const char *src, const size_t len, char *dst);


#ifdef __cplusplus
}
#endif

#endif //_URI_ENCODE_H_
