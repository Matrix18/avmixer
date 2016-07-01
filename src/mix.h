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
 * mix MAX_CHANNELS individual channels' pcm data in 16 bit.
 */
int mix_audio_pcm_s16le(int channels, int16_t* datas[MAX_CHANNELS],
        int lens[MAX_CHANNELS], int *datas_index);


#endif /* MIX_H_ */
