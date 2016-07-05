/*
 * test.c
 *
 *  Created on: 2016-7-1
 *      Author: will
 */
#include <stdio.h>
#include <stdlib.h>
#include "mix.h"
#include <string.h>
#include <errno.h>
#include "aac_enc.h"

typedef enum { 
    false,
    true
} bool;

#define THRESHOLD  0.6

#define BUFF_SIZE 1024
#define BUFF_BYTES_SIZE BUFF_SIZE*sizeof(int16_t)

int mix_pcm_s16le_test(char** filenames, int file_num)
{
	int channels = file_num;
    char* files[MAX_CHANNELS];
    FILE* fds[MAX_CHANNELS];
    int16_t* buffs[MAX_CHANNELS];
    // numbers of int16_t, not bytes
    int lens[MAX_CHANNELS];
    int ret;

    FILE *fd=fopen("mixtest_out.pcm","wb+");

    for (int i=0;i < channels;i++) {
        files[i] = filenames[i];
        fds[i] = fopen(files[i],"rb+");
        if (fds[i] == NULL) {
            printf("open file %s failed.\n", files[i]);
            ret = -1;
			goto end;
        }
        buffs[i] = (int16_t*)malloc(BUFF_SIZE*sizeof(int16_t));
        lens[i] = 0;
    }

    while(true) {
        // reset
        bool eof = false;
        for (int i=0;i < channels;i++) {
            eof = eof || !feof(fds[i]);
            memset(buffs[i], 0, BUFF_BYTES_SIZE);
            lens[i] = 0;
        }

        if (!eof) {
            break;
        }

        // read channel datas
        for (int i=0;i < channels;i++) {
            if (!feof(fds[i])) {
                lens[i] = fread(buffs[i], sizeof(int16_t), BUFF_SIZE, fds[i]);
            }
        }
        // calculate result
        if (mix_audio_pcm_s16le(THRESHOLD, channels, buffs, lens) != 0) {
            printf("mix error.\n");
			ret = -1;
            break;
        }
        // write to output
        if (lens[0] != fwrite(buffs[0], sizeof(int16_t), lens[0], fd)) {
            printf("write output error.\n");
			ret = -1;
            break;
        }
    }

end:
    for (int i=0;i < channels;i++) {
        fclose(fds[i]);
        free(buffs[i]);
    }

    fclose(fd);
	return ret;
}

int pcms16_mono2aac_test(const char* inputfile, const char* outputfile)
{
	int ret = 0;
	FILE *inputfp, *outputfp;
	inputfp = fopen(inputfile, "rb+");
	if (inputfp == NULL) {
        fprintf(stderr, "Open file %s failed.\n", inputfile);
        return -1;
    }
	outputfp = fopen(outputfile, "wb+");
	if (outputfp == NULL) {
        fprintf(stderr, "Open file %s failed.\n", outputfile);
        return -1;
    }

	encoder_context *ctx = NULL;
	ret = init_aac_encoder(&ctx);
	if (ret != 0) {
		fprintf(stderr, "Open aac encoder error.\n");
		return -1;	
	}
	
    uint8_t* inbuf = (uint8_t*)malloc(BUFF_BYTES_SIZE);
    uint8_t* outbuf = (uint8_t*)malloc(BUFF_BYTES_SIZE);
	
	while(!feof(inputfp)) {
		// number of int16_t
		int len = fread(inbuf, sizeof(int16_t), BUFF_SIZE, inputfp);
		if (len > 0) {
			// outbuf must be less than inbuf since aac is to compress the pcm.
			int outlen = BUFF_BYTES_SIZE; // bytes number
			ret = encode_mono(ctx, inbuf, 2*len, outbuf, &outlen);
			if (ret < 0) {
				fprintf(stderr, "Encode_mono error.\n");
				return -1;
			} else if (ret == 1) {
			    // reach the file end.
			    break;
			}
			int out = fwrite(outbuf, 1, outlen, outputfp);
			printf("out=%d, outlen=%d\n", out, outlen);
		}
	}

	fclose(inputfp);
	fclose(outputfp);
	free(inbuf);
	free(outbuf);
	close_encoder(ctx);
	free(ctx);
	
	return ret;
}


int main(int argc, char** argv)
{
    if (argc == 1) {
        printf("\n");
        printf("Usage:\n");
        printf("        ./mixtest <file1.pcm> <file1.pcm> ... \n");
        printf("        the max file num is now 4 for test propose.\n");
        printf("\n");
        return 0;
    }

    /*if (mix_pcm_s16le_test(&argv[1], argc - 1) != 0) {
		fprintf(stderr, "ERROR: mix pcm s16 files failed.\n");
	}*/
    if (pcms16_mono2aac_test("/home/will/lsm/input.pcm", "/home/will/lsm/test.aac") < 0) {
        fprintf(stderr, "ERROR: encode pcm s16 file test failed.\n");
    }

}
