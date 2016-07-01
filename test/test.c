/*
 * test.c
 *
 *  Created on: 2016-7-1
 *      Author: will
 */
#include <stdio.h>
#include <stdlib.h>
#include "mix.h"

typedef enum { false, true }bool;

int mix_pcm16le(char *url){
    FILE *fp=fopen(url,"rb+");
    FILE *fp1=fopen("output_l.pcm","wb+");
    FILE *fp2=fopen("output_r.pcm","wb+");

    unsigned char *sample=(unsigned char *)malloc(4);

    while(!feof(fp)){
        fread(sample,1,4,fp);
        //L
        fwrite(sample,1,2,fp1);
        //R
        fwrite(sample+2,1,2,fp2);
    }

    free(sample);
    fclose(fp);
    fclose(fp1);
    fclose(fp2);
    return 0;
}

#define BUFF_SIZE 1024
#define BUFF_BYTES_SIZE BUFF_SIZE*sizeof(int16_t)

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

    int channels = argc - 1;
    char* files[MAX_CHANNELS];
    FILE* fds[MAX_CHANNELS];
    int16_t* buffs[MAX_CHANNELS];
    // numbers of int16_t, not bytes
    int lens[MAX_CHANNELS];
    int ret;

    for (int i=0;i < channels;i++) {
        files[i] = argv[i+1];
        fds[i] = fopen(files[i],"wb+");
        buffs[i] = (int16_t*)malloc(BUFF_SIZE*sizeof(int16_t));
        lens[i] = 0;
    }

    while(true) {
        bool eof = false;
        for (int i=0;i < channels;i++) {
            eof = eof || !feof(fds[i]);
        }

        if (!eof) {
            break;
        }
        // read channel datas
        for (int i=0;i < channels;i++) {
            if (!feof(fds[i])) {
                lens[i] = fread(buffs[i], sizeof(int16_t), BUFF_SIZE,fds[i]);
            }
        }
        int data_index = -1;
        if (mix_audio_pcm_s16le(channels, buffs, lens, &data_index) != 0) {
            break;
            printf("mix error.\n");
        }
        if (data_index < 0 || data_index > channels) {
            printf("get data_index error.\n");
            break;
        }
    }

    for (int i=0;i < channels;i++) {
        fclose(fds[i]);
        free(buffs[i]);
    }

    return 0;
}
