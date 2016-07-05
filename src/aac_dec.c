/*
 * mix.c
 *
 *  Created on: 2016-7-3
 *      Author: will
 */

#include <stdio.h>
#include <stdlib.h>
#include <fdk-aac/aacdecoder_lib.h>
#include "aac_dec.h"

struct decoder_context {
    HANDLE_AACDECODER hAacDecoder;
    int downmix_channels;// number of output channels, MONO:1, STEREO:2
};

/* ASC config binary data */
UCHAR eld_conf[] = { 0xF8, 0xE8, 0x50, 0x00 };
UCHAR *conf[] = { eld_conf };                   //TODO just for aac eld config

int open_decoder(decoder_context** ctx)
{
    HANDLE_AACDECODER hAacDecoder;
    AAC_DECODER_ERROR err;
    CStreamInfo *aac_stream_info = NULL;
    int downmix_channels = 1;
    hAacDecoder = aacDecoder_Open(TT_MP4_RAW, 1);
    if (hAacDecoder == NULL) {
        fprintf(stderr, "Unable to open decoder\n");
        return -1;
    }
    /*if (aacDecoder_ConfigRaw(hAacDecoder, conf, &conf_len) != AAC_DEC_OK) {
        fprintf(stderr, "Unable to set configRaw\n");
        return -1;
    }*/
    if (aacDecoder_SetParam(hAacDecoder, AAC_PCM_OUTPUT_CHANNEL_MAPPING, 1/*wav*/) != AAC_DEC_OK) {
        fprintf(stderr, "aacDecoder_SetParam wav failed!\n");
        return -1;
    }
    if (aacDecoder_SetParam(hAacDecoder, AAC_PCM_MAX_OUTPUT_CHANNELS, downmix_channels) != AAC_DEC_OK) {
        fprintf(stderr, "Unable to set output channels in the decoder\n");
        return -1;
    }
    aac_stream_info = aacDecoder_GetStreamInfo(hAacDecoder);
    if (aac_stream_info == NULL) {
        fprintf(stderr, "aacDecoder_GetStreamInfo failed!\n");
        return -1;
    }

    decoder_context* dec_ctx = (decoder_context*)malloc(sizeof(decoder_context));
    dec_ctx->hAacDecoder = hAacDecoder;
    dec_ctx->downmix_channels = 1;
    *ctx = dec_ctx;

    return 0;
}

/* period size 480 samples */
#define N_SAMPLE 480
static int pcm_pkt_size = 4 * N_SAMPLE;
static int fdk_flags = 0;

int decode_mono(decoder_context* ctx, INT_PCM *output_buf, int *output_size, uint8_t *buffer, int *size)
{
    int ret = 0;
    AAC_DECODER_ERROR err;
    int buff_size = *size;
    UINT valid_size = *size;
    UCHAR *input_buf[1] = {buffer};

    /* step 1. Fill aac_data_buf to decoder's internal buf */
    err = aacDecoder_Fill(ctx->hAacDecoder, input_buf, &buff_size, &valid_size);
    if (err!= AAC_DEC_OK) {
        fprintf(stderr, "Fill failed, err=%x\n", err);
        *output_size  = 0;
        return -1;
    }

    /* step 2. Call decoder function */
    err = aacDecoder_DecodeFrame(ctx->hAacDecoder, output_buf, pcm_pkt_size, fdk_flags);
    if (err == AAC_DEC_NOT_ENOUGH_BITS) {
        fprintf(stderr, "not enough\n");
        *output_size  = 0;
         //if not enough, get more data
        return 0;
    } else if (err != AAC_DEC_OK) {
        fprintf(stderr, "aacDecoder_DecodeFrame : 0x%x\n", err);
        *output_size  = 0;
        return -1;
    }

    *output_size = pcm_pkt_size;
    //return aac decode size
    *size = *size - valid_size;
    return 0;
}

void close_decoder(decoder_context* ctx)
{
        if (ctx->hAacDecoder) {
            aacDecoder_Close(ctx->hAacDecoder);
        }
}
