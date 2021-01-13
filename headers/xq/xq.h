//
//  xq.h
//  xqcsdk
//
//  Created by Ike E on 10/15/20.
//
// A general header that will automatically include all other headers required
// for a basic XQ project.

#ifndef xq_h
#define xq_h

#include <xq/config.h>
#include <xq/connect.h>
#include <ext/jread/jRead.h>
#include <ext/jwrite/jWrite.h>
#include <ext/http-codes/http-codes.h>
#include <xq/services/sub/authorize.h>
#include <xq/services/sub/authorize_alias.h>
#include <xq/services/sub/code_validation.h>
#include <xq/services/sub/settings.h>
#include <xq/services/sub/subscriber.h>
#include <ext/base64/base64.h>
#include <xq/services/quantum/quantum.h>
#include <xq/services/crypto.h>
#include <xq/services/sub/exchange.h>
#include <xq/algorithms/otp/otp_encrypt.h>
#include <xq/algorithms/otp/otp_decrypt.h>
#include <xq/algorithms/aes/aes_encrypt.h>
#include <xq/algorithms/aes/aes_decrypt.h>
#include <xq/algorithms/nist/nist_encrypt.h>
#include <xq/algorithms/nist/nist_decrypt.h>
#include <xq/services/sub/combined.h>
#include <xq/services/sub/delegate.h>
#include <xq/services/sub/packet.h>
#include <xq/services/val/expiration.h>

#endif /* xq_h */
