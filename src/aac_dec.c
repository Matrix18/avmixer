/*
 * mix.c
 *
 *  Created on: 2016-7-3
 *      Author: will
 */

#include <stdio.h>
#include <stdlib.h>
#include "aac_dec.h"

struct decoder_context {
    HANDLE_AACDECODER hAacDecoder;
    int downmix_channels;// number of output channels, MONO:1, STEREO:2
};

/*static const int aac_sample_rates[] = {
    96000, 88200, 64000, 48000, 44100, 32000,
    24000, 22050, 16000, 12000, 11025, 8000, 0
};*/

int open_decoder(decoder_context** ctx)
{
    HANDLE_AACDECODER hAacDecoder;
    AAC_DECODER_ERROR err;
    int downmix_channels = 1;
    //Set up the Audio Specific Config
    // layout:
    //| aot(5 bits)   |  samplingFrequencyIndex(4 bits)  | channelConfiguration(4 bits)  |  frameLengthFlag(1 bit)  |  DependsOnCoreCoder(1bit)    |  extensionFlag(1bit)|
    //        2(LC)                                     2(64000)                                                            1                                                           0(1024)
    //

    UCHAR asc[2]; // 0001 0|001 1| 001 0|0|0|0
    asc[ 0 ] = (UCHAR) 0x11 ; // 0001 0|001 1| 001 0|0|0|0
    asc[ 1 ] = (UCHAR) 0x88 ;
    UCHAR * conf[] = { asc };
    UINT conf_len = sizeof(asc);

    hAacDecoder = aacDecoder_Open(TT_MP4_ADTS, 1);
    if (hAacDecoder == NULL) {
        fprintf(stderr, "Unable to open decoder\n");
        return -1;
    }
    if ((err = aacDecoder_ConfigRaw(hAacDecoder, conf, &conf_len)) != AAC_DEC_OK) {
        fprintf(stderr, "Unable to set configRaw, err=%d\n", err);
        return -1;
    }
    if (aacDecoder_SetParam(hAacDecoder, AAC_PCM_OUTPUT_CHANNEL_MAPPING, 1/*wav*/) != AAC_DEC_OK) {
        fprintf(stderr, "aacDecoder_SetParam wav failed!\n");
        return -1;
    }
    if (aacDecoder_SetParam(hAacDecoder, AAC_PCM_MAX_OUTPUT_CHANNELS, downmix_channels) != AAC_DEC_OK) {
        fprintf(stderr, "Unable to set output channels in the decoder\n");
        return -1;
    }

    decoder_context* dec_ctx = (decoder_context*)malloc(sizeof(decoder_context));
    dec_ctx->hAacDecoder = hAacDecoder;
    dec_ctx->downmix_channels = 1;
    *ctx = dec_ctx;

    return 0;
}

/* period size 480 samples */
//#define N_SAMPLE 480
//static int pcm_pkt_size = 4 * N_SAMPLE;

int decode_mono(decoder_context* ctx, uint8_t *buffer, int *size, INT_PCM *output_buf, int *output_size)
{
    int ret = 0;
    AAC_DECODER_ERROR err;
    int buff_size = *size;
    UINT valid_size = *size;
    uint8_t *input_buf[1] = {buffer};
    CStreamInfo *aac_stream_info = NULL;

    aac_stream_info = aacDecoder_GetStreamInfo(ctx->hAacDecoder);
    if (aac_stream_info == NULL) {
        fprintf(stderr, "aacDecoder_GetStreamInfo failed!\n");
        return -1;
    }

    /* step 1. Fill aac_data_buf to decoder's internal buf */
    err = aacDecoder_Fill(ctx->hAacDecoder, input_buf, &buff_size, &valid_size);
    if (err!= AAC_DEC_OK) {
        fprintf(stderr, "Fill failed, err=%x\n", err);
        *output_size  = 0;
        return -1;
    }
    //return aac decoded size
    *size = *size - valid_size;

    /* step 2. Call decoder function */
    err = aacDecoder_DecodeFrame(ctx->hAacDecoder, output_buf, *output_size, 0);
    if (err == AAC_DEC_NOT_ENOUGH_BITS) {
        fprintf(stderr, "not enough data\n");
        *output_size  = 0;
         //if not enough, get more data
        return 1;
    }
    if (err != AAC_DEC_OK) {
        fprintf(stderr, "aacDecoder_DecodeFrame : 0x%x\n", err);
        *output_size  = 0;
        return -1;
    }
    *output_size = aac_stream_info->frameSize * sizeof(INT_PCM) * aac_stream_info->numChannels;

    return 0;
}

void close_decoder(decoder_context* ctx)
{
        if (ctx->hAacDecoder) {
            aacDecoder_Close(ctx->hAacDecoder);
        }
}

void flush_decoder(decoder_context *ctx)
{
    if (!ctx->hAacDecoder)
        return;

    if (aacDecoder_SetParam(ctx->hAacDecoder, AAC_TPDEC_CLEAR_BUFFER, 1) != AAC_DEC_OK) {
        fprintf(stderr, "failed to clear buffer when flushing\n");
    }
}
