/*
 * mix.c
 *
 *  Created on: 2016-7-3
 *      Author: will
 */

#include <fdk-aac/aacenc_lib.h>

static HANDLE_AACENCODER hAacEncoder = NULL;


typedef struct {
	HANDLE_AACENCODER hAacEncoder;
} convert_context;


int open_aac_encoder(convert_context** ctx)
{
	int ret = 0;
	AACENC_ERROR ErrorStatus;
	HANDLE_AACENCODER hAacEncoder;
	
	/* encoder handle */
	if ( (ErrorStatus = aacEncOpen(&hAacEncoder, 0, 0)) != AACENC_OK ) {
		printf("aacEncOpen error.\n");
		return -1;
	}

	ErrorStatus = aacEncoder_SetParam(hAacEncoder, AACENC_AOT, AOT_AAC_LC);
	if (ErrorStatus != AACENC_OK) {
		printf("acc encoder set param error.\n");
		return -1;	
	}

	convert_context* context = (convert_context*)malloc(sizeof(convert_context));
	context->hAacEncoder = hAacEncoder;
	*ctx = context;

	return ret;
}

int encode_mono(convert_context* ctx, int16_t* pcm_s16_data, int inlen, uint8_t* outdata, int *outlen)
{
	int ret = 0;
	AACENC_ERROR ErrorStatus;
	
	//AACENC_InfoStruct encInfo;
	//ErrorStatus = aacEncInfo(hAacEncoder, &encInfo);
	// Feed input buffer with new audio data and provide input/output arguments to aacEncEncode().
	AACENC_BufDesc inBufDesc, outBufDesc;
	void* inbufs[] = {(void*)pcm_s16_data};
	INT inidf = IN_AUDIO_DATA;
	INT inbufSize = inlen;
	INT inbufElSize = sizeof(int16_t);
	inBufDesc.numBufs = 1;
	inBufDesc->bufs = inbufs;
	inBufDesc->bufferIdentifiers = &inidf;
	inBufDesc->bufSizes = &inbufSize;
	inBufDesc->bufElSizes = &inbufElSize;

	void* outbufs[] = {(void*)outdata};
	INT outidf = OUT_BITSTREAM_DATA;
	INT outbufSize = *outlen;
	INT outbufElSize = sizeof(int8_t);

	outBufDesc.numBufs = 1;
	outBufDesc->bufs = outbufs;
	outBufDesc->bufferIdentifiers = &outidf;
	outBufDesc->bufSizes = &outbufSize;
	outBufDesc->bufElSizes = &outbufElSize;

	AACENC_InArgs inargs;
	inargs.numInSamples = 1;
	inargs.numAncBytes = 0;

	AACENC_OutArgs outargs;
	outargs.numInSamples = 1;
	outargs.numAncBytes = 0;
	// bytes of encoded data
	//outargs.numOutBytes 
	ErrorStatus = aacEncEncode(ctx->hAacEncoder, 
								&inBufDesc,
								&outBufDesc,
								&inargs,
								&outargs);
	if (ErrorStatus == AACENC_OK) {
		printf("aacEncEncode err:%d\n", ErrorStatus);
		return -1;	
	}
	// Write output data to file or audio device.
	outlen = outargs.numOutBytes;

	return ret;
}

void close_encoder(convert_context* ctx)
{
	if (ctx->hAacEncoder) {
		aacEncClose(&ctx->hAacEncoder);	
	}
}
