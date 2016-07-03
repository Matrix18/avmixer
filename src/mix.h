/*
 * mix.h
 *
 *  Created on: 2016-7-1
 *      Author: will
 */

#ifndef MIX_H_
#define MIX_H_

#include <stdint.h>

#define MAX_CHANNELS  4
#define MIX_THRESHOLD 0.6
//threshold numerator
#define MIX_THRESHOLD_NUM  6
// threshold denominator
#define MIX_THRESHOLD_DEM  10
#define MAX_SAMPLE_VALUE 32767
#define MIN_SAMPLE_VALUE -32768
#define NEED_COMPRESS(x)     ((MIX_THRESHOLD_DEM*x > MIX_THRESHOLD_NUM*MAX_SAMPLE_VALUE) \
        || (MIX_THRESHOLD_DEM*x < MIX_THRESHOLD_NUM*MIN_SAMPLE_VALUE))

/*
 * Mix MAX_CHANNELS individual channels' pcm data in 16 bit.
 */
extern int mix_audio_pcm_s16le(float threshold, int channels, int16_t* datas[], int lens[]);

/*
 * Convert pcm_s16 format to pcm_u8 format
 */
extern void s16le2pcm8(int16_t* s16_data, uint8_t* u8_data, int s16_len);

/*
 * split 
 */
extern void split_pcm_s16le_mono2stereo(int16_t *s16_mono_data, int mono_len, int16_t **s16_stereo_data);

#endif /* MIX_H_ */
