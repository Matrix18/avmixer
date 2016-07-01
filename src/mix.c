/*
 * mix.c
 *
 *  Created on: 2016-7-1
 *      Author: will
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mix.h"

/*
threshold   alpha
    0                   2.51
    0.05                2.67
    0.1                 2.84
    0.15                3.04
    0.2                 3.26
    0.25                3.52
    0.3                 3.82
    0.35                4.17
    0.4                 4.59
    0.45                5.09
    0.5                 5.71
    0.55                6.49
    0.6                 7.48
    0.65                8.81
    0.7                 10.63
    0.75                13.3
    0.8                 17.51
    0.85                24.97
    0.9                 41.15
    0.95                96.09

f(x) = t + (1-t)*ln(1+α*(x-t)/(2-t))/ln(1+α)
when t=0.6, α=7.48

f(x) = 0.6 +0.4*ln(5.342857*x - 2.205714)/ln(8.48)
*/

static int16_t pcm_compress(int16_t input)
{
    return 0.6 +0.4*log(5.342857*input - 2.205714)/log(8.48);
}

int mix_audio_pcm_s16le(int channels, int16_t* datas[], int lens[])
{
    if (channels > MAX_CHANNELS) {
        return -1;
    }

    int ret = 0;
    // max numbers of int16_t
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
            sum = sum>0? MAX_SAMPLE_VALUE * pcm_compress(sum): MIN_SAMPLE_VALUE * pcm_compress(sum);
        }
        datas[0][j] = (int16_t)sum;
    }

    lens[0] = max_data_len;

    return ret;
}
