/*
 * pcm_enc.h
 *
 *  Created on: 2016-7-3
 *      Author: will
 */

#ifndef PCM_ENC_H_
#define PCM_ENC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

typedef enum {
	PCM_UNKNOWN = 0,
	PCM_S16_LE,
	PCM_U8
} PCM_TYPE;

typedef struct convert_context convert_context;

/*
 * open aac encoder 
 */
extern int open_aac_encoder(convert_context** ctx);

/*
 * encode pcm to aac 
 */
int encode_mono(convert_context* ctx, byte* pcm_s16_data, int inlen, byte* outdata, int* outlen);

/*
 * close aac encoder 
 */
void close_encoder(convert_context* ctx);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PCM_ENC_H_ */
