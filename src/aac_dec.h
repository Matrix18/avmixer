/*
 * pcm_enc.h
 *
 *  Created on: 2016-7-4
 *      Author: will
 */

#ifndef PCM_DEC_H_
#define PCM_DEC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

typedef enum {
	PCM_UNKNOWN = 0,
	PCM_S16_LE,
	PCM_U8
} PCM_TYPE;

typedef struct decoder_context decoder_context;

/*
 * open aac encoder 
 */
extern int open_decoder(decoder_context** ctx);

/*
 * decode aac to pcm
 */
extern int decode_mono(decoder_context* ctx, INT_PCM *output_buf, int *output_size, uint8_t *buffer, int *size);

/*
 * close aac encoder 
 */
extern void close_decoder(decoder_context* ctx);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PCM_DEC_H_ */
