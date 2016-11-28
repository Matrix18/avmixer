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
#include <fdk-aac/aacdecoder_lib.h>

#define DMX_ANC_BUFFSIZE       128
#define DECODER_MAX_CHANNELS     6
#define DECODER_BUFFSIZE      2048 * sizeof(INT_PCM)

typedef struct decoder_context decoder_context;

/*
 * open aac decoder
 */
extern int open_decoder(decoder_context** ctx);

/*
 * decode aac to pcm
 */
extern int decode_mono(decoder_context* ctx, uint8_t *buffer, int *size, INT_PCM *output_buf, int *output_size);

/*
 * close aac decoder
 */
extern void close_decoder(decoder_context* ctx);

/*
 * flush aac decoder
 */
extern void flush_decoder(decoder_context *ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PCM_DEC_H_ */
