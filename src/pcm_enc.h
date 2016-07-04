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

typedef unsigned char byte;

typedef struct convert_context convert_context;

/*
 * open aac encoder 
 */
extern int init_aac_encoder(convert_context** ctx);

/*
 * encode pcm to aac 
 */
extern int encode_mono(convert_context* ctx, uint8_t* input_buf, int input_size, byte* outdata, int *outlen);

/*
 * close aac encoder 
 */
extern void close_encoder(convert_context* ctx);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PCM_ENC_H_ */
