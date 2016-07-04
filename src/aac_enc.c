/*
 * mix.c
 *
 *  Created on: 2016-7-3
 *      Author: will
 */

#include <stdio.h>
#include <stdlib.h>
#include <fdk-aac/aacenc_lib.h>
#include "pcm_enc.h"

//static HANDLE_AACENCODER hAacEncoder = NULL;

struct convert_context{
	HANDLE_AACENCODER hAacEncoder;
};

int init_aac_encoder(convert_context** ctx)
{
	int ret = 0;
	AACENC_ERROR err;
	int vbr = 0;
	HANDLE_AACENCODER hAacEncoder;

	/* encoder handle */
	if (aacEncOpen(&hAacEncoder, 0, 1) != AACENC_OK ) {
		fprintf(stderr, "Unable to open encoder.\n");
		return -1;
	}
	if (aacEncoder_SetParam(hAacEncoder, AACENC_AOT, AOT_AAC_LC) != AACENC_OK) {
		fprintf(stderr, "Unable to set the aot.\n");
		return -1;	
	}
    if (aacEncoder_SetParam(hAacEncoder, AACENC_SAMPLERATE, 44100) != AACENC_OK) {
        fprintf(stderr, "Unable to set the sample rate.\n");
        return -1;
    }
    if (aacEncoder_SetParam(hAacEncoder, AACENC_CHANNELMODE, 1) != AACENC_OK) {
        fprintf(stderr, "Unable to set the channel mode.\n");
        return -1;
    }
    if (aacEncoder_SetParam(hAacEncoder, AACENC_CHANNELORDER, 1) != AACENC_OK) {
        fprintf(stderr, "Unable to set the wav channel order\n");
        return -1;
    }
    if (vbr) {
        if (aacEncoder_SetParam(hAacEncoder, AACENC_BITRATEMODE, vbr) != AACENC_OK) {
            fprintf(stderr, "Unable to set the VBR bitrate mode\n");
            return 1;
        }
    } else {
        if (aacEncoder_SetParam(hAacEncoder, AACENC_BITRATE, 64000) != AACENC_OK) {
            fprintf(stderr, "Unable to set the bitrate\n");
            return 1;
        }
    }
    if (aacEncoder_SetParam(hAacEncoder, AACENC_TRANSMUX, 2) != AACENC_OK) {
        fprintf(stderr, "Unable to set the ADTS transmux\n");
        return -1;
    }
    if (aacEncoder_SetParam(hAacEncoder, AACENC_AFTERBURNER, 0) != AACENC_OK) {
        fprintf(stderr, "Unable to set the afterburner mode\n");
        return -1;
    }
    if ((err = aacEncEncode(hAacEncoder, NULL, NULL, NULL, NULL)) != AACENC_OK) {
        fprintf(stderr, "Unable to initialize the encoder\n");
        return 1;
    }

	convert_context* context = (convert_context*)malloc(sizeof(convert_context));
	context->hAacEncoder = hAacEncoder;
	*ctx = context;

	return ret;
}

int encode_mono(convert_context* ctx, uint8_t* input_buf, int input_size, uint8_t* outbuf, int *outlen)
{
	int ret = 0;
    AACENC_InfoStruct info = {0};
	// Feed input buffer with new audio data and provide input/output arguments to aacEncEncode().
	AACENC_BufDesc in_buf = { 0 }, out_buf = { 0 };
    AACENC_InArgs in_args = { 0 };
    AACENC_OutArgs out_args = { 0 };
    int in_identifier = IN_AUDIO_DATA;
    int in_size, in_elem_size;
    int out_identifier = OUT_BITSTREAM_DATA;
    int out_size, out_elem_size;
    int i;
    int channels = 1;
    void *in_ptr, *out_ptr;
    //uint8_t outbuf[20480];
    AACENC_ERROR err;
    //int input_size;
    //uint8_t* input_buf;
    int16_t* convert_buf;

    if (aacEncInfo(ctx->hAacEncoder, &info) != AACENC_OK) {
        fprintf(stderr, "cannot retrieve encoder info\n");
        return -1;
    }

    //input_size = channels*2*info.frameLength;
    //input_buf = (uint8_t*) malloc(input_size);
    convert_buf = (int16_t*) malloc(input_size);

    for (i = 0; i < input_size / 2; i++) {
        const uint8_t* in = &input_buf[2 * i];
        convert_buf[i] = in[0] | (in[1] << 8);
    }
    if (input_size <= 0) {
        in_args.numInSamples = -1;
    } else {
        in_ptr = convert_buf;
        in_size = input_size;
        in_elem_size = 2;

        in_args.numInSamples = input_size / 2;
        in_buf.numBufs = 1;
        in_buf.bufs = &in_ptr;
        in_buf.bufferIdentifiers = &in_identifier;
        in_buf.bufSizes = &in_size;
        in_buf.bufElSizes = &in_elem_size;
    }
    out_ptr = outbuf;
    out_size = *outlen;
    out_elem_size = 1;
    out_buf.numBufs = 1;
    out_buf.bufs = &out_ptr;
    out_buf.bufferIdentifiers = &out_identifier;
    out_buf.bufSizes = &out_size;
    out_buf.bufElSizes = &out_elem_size;

    if ((err = aacEncEncode(ctx->hAacEncoder, &in_buf, &out_buf, &in_args, &out_args))
            != AACENC_OK) {
        if (err == AACENC_ENCODE_EOF)
            return 1;
        fprintf(stderr, "Encoding failed\n");
        return -1;
    }

	// Write output data to file or audio device.
	*outlen = out_args.numOutBytes;

	return ret;
}

void close_encoder(convert_context* ctx)
{
	if (ctx->hAacEncoder) {
		aacEncClose(&ctx->hAacEncoder);	
	}
}
