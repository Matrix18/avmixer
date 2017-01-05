/*
 * mix.c
 *
 *  Created on: 2016-7-1
 *      Author: will
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "audio_mix.h"

/*
 * this algorithm is based on
 * http://www.voegler.eu/pub/audio/digital-audio-mixing-and-normalization.html
 *
threshold   	alpha
    0           2.51
    0.05        2.67
    0.1         2.84
    0.15        3.04
    0.2         3.26
    0.25        3.52
    0.3         3.82
    0.35        4.17
    0.4         4.59
    0.45        5.09
    0.5         5.71
    0.55        6.49
    0.6         7.48
    0.65        8.81
    0.7         10.63
    0.75        13.3
    0.8         17.51
    0.85        24.97
    0.9         41.15
    0.95        96.09

		 	 x-t
		 ln(1+α* ---)
			 c-t
f(x) = t + (1-t)*------------
		   ln(1+α)
or
f(x) = t + (1-t)*ln(1+α*(x-t)/(c-t))/ln(1+α)
In the formula above,
	1. t is the threshold you want to set.
	2. c is the number of channels that are being merged.
    3. x is the sum input value each of which is normalized to [0,1], 
	   that is, when c=3, x has the range [0, 3];
    4. α is an assistant parameter which is associated with t.

when t=0.6, α=7.48
f(x) = 0.6 + 0.4*ln(1+7.48*(x-0.6)/(c-0.6))/ln(8.48), ln(8.48) = 2.137710
f(x) = 0.6 + 0.187116120*ln(1+7.48*(x-0.6)/(c-0.6))

Especially, when c=2
f(x) = 0.6 + 0.187116120*ln(5.342857143*x - 3.205714286)
*/

typedef struct {
	float threshold;
	float alpha;
}threshold_alpha;

static const threshold_alpha ta_table[20] = {
	{0, 2.51},
	{0.05, 2.67},
	{0.1, 2.84},
	{0.15, 3.04},
	{0.2, 3.26},
	{0.25, 3.52},
	{0.3, 3.82},
	{0.35, 4.17},
	{0.4, 4.59},
	{0.45, 5.09},
	{0.5, 5.71},
	{0.55, 6.49},
	{0.6, 7.48},
	{0.65, 8.81},
	{0.7, 10.63},
	{0.75, 13.3},
	{0.8, 17.51},
	{0.85, 24.97},
	{0.9, 41.15},
	{0.95, 96.09}
};


/*
 * calculate compression ratio
 * @sum  the sum of samples
 * @c    the number of channels
 */
static float pcm_compress(int t, int c, int sum)
{
	float x = 1;
	// x has range [0, c]
	if (sum >= 0) {
		x = (float)sum / MAX_SAMPLE_VALUE;
	} else {
		x = (float)sum / MIN_SAMPLE_VALUE;
	}
	float alpha = -1;
	for (int i = 0;i < 20; i++) {
		if (ta_table[i].threshold == t) {
			alpha = ta_table[i].alpha;
			break;
		}
	}

	if (alpha < 0) {
		return -1;
	}
	return (float)(t + (1-t)*log(1 + alpha*(x-t)/(c-t))/log(1+alpha));
}

int mix_audio_pcm_s16le(float threshold, int channels, int16_t* datas[], int lens[])
{
    if (channels > MAX_CHANNELS) {
        return -1;
    }

    int ret = 0;
    // max number of int16_t
    int max_data_len = 0;
    int i;
    for (i =0;i < channels; i++) {
        if (lens[i] > max_data_len) {
            max_data_len = lens[i];
        }
    }

    for (int j = 0; j < max_data_len;j++ ) {
        int sum = 0;
        for (i =0;i < channels; i++) {
            sum += datas[i][j];
        }
        // bigger than threshold or smaller than -threshold,
        // if sum is in range of threshold, directly use the it.
        if (MIX_THRESHOLD_DEM*sum > MIX_THRESHOLD_NUM*MAX_SAMPLE_VALUE ||
              MIX_THRESHOLD_DEM*sum < MIX_THRESHOLD_NUM*MIN_SAMPLE_VALUE) {
			float comp_ratio = pcm_compress(threshold, channels, sum);
			printf("comp_ratio:%.2f\n", comp_ratio);
			if (comp_ratio < 0) {
				printf("wrong threshold.\n");
				return -1;
			}
            sum = sum > 0 ? MAX_SAMPLE_VALUE * comp_ratio: MIN_SAMPLE_VALUE * comp_ratio;
        }
        datas[0][j] = (int16_t)sum;
    }

    lens[0] = max_data_len;

    return ret;
}

/*
 * pcm_s16 sample value range is [-32768, 32767]
 * pcm_u8  sample value range is [0, 255]
 */
void s16le2pcm8(int16_t* s16_data, uint8_t* u8_data, int s16_len)
{
	for (int i = 0; i < s16_len; i++) {
		// add +128 make [-128, 127] fit into [0, 255]
		u8_data[i] = (int8_t)(s16_data[i] >> 8) + 128;
	}
}

/*
 * Convert mono channel to stereo(L&R) channels
 * the conversion should obey the formula M*M = L*L + R*R, L=R.
 * where M means mono sample value, L means Left channel value 
 * and R means Right channel value. So L = R = M*sqrt(2)/2.
 * Stereo channel data layout:
 * | int16_t(L) | int16_t(R) | int16_t(L) | int16_t(R) | ...
 * sqrt(2)/2 = 0.707106781
 */
void pcm_s16le_mono2stereo(int16_t *s16_mono_data, int mono_len, int16_t **s16_stereo_data)
{
	int16_t* stereo_data = (int16_t*)malloc(sizeof(int16_t) * mono_len * 2);
	for (int i = 0; i < mono_len; i++) {
		int16_t lr_value = (int16_t)(0.707106781*s16_mono_data[i]);
		stereo_data[i] = lr_value;
		stereo_data[i+1] = lr_value;
	}
	*s16_stereo_data = stereo_data;
}

