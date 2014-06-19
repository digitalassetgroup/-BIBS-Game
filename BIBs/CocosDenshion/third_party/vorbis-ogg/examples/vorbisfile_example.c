/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: simple example decoder using vorbisfile
 last mod: $Id: vorbisfile_example.c 16328 2009-07-24 01:51:10Z xiphmont $

 ********************************************************************/

/* Takes a vorbis bitstream from stdin and writes raw stereo PCM to
   stdout using vorbisfile. Using vorbisfile is much simpler than
   dealing with libvorbis. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <string.h>

#ifdef _WIN32 /* We need the following two to set stdin/stdout to binary */
#include <io.h>
#include <fcntl.h>
#endif

char pcmout[4096]; /* take 4k out of the data segment, not the stack */

char pcmout2[4096];

//

struct wavfile
{
    char        id[4];          // should always contain "RIFF"
    int     totallength;    // total file length minus 8
    char        wavefmt[8];     // should be "WAVEfmt "
    int     format;         // 16 for PCM format
    short     pcm;            // 1 for PCM format
    short     channels;       // channels
    int     frequency;      // sampling frequency, 16000 in this case
    int     bytes_per_second;
    short     bytes_by_capture;
    short     bits_per_sample;
    char        data[4];        // should always contain "data"
    int     bytes_in_data;
};

//Writes a header to a file that has been opened (take heed that the correct flags
//must've been used. Binary mode required, then you should choose whether you want to 
//append or overwrite current file
void write_wav_header(FILE* fileOut, int samples, int channels, int frequency){
    struct wavfile filler;
    strcpy(filler.id, "RIFF");
    filler.totallength = (samples * channels) + sizeof(struct wavfile) - 8; //81956
    strcpy(filler.wavefmt, "WAVEfmt ");
    filler.format = 16;
    filler.pcm = 1;
    filler.channels = channels;
    filler.frequency = frequency;
    filler.bits_per_sample = 16;
    filler.bytes_per_second = filler.channels * filler.frequency * filler.bits_per_sample/8;
    filler.bytes_by_capture = filler.channels*filler.bits_per_sample/8;
    filler.bytes_in_data = samples * filler.channels * filler.bits_per_sample/8;    
    strcpy(filler.data, "data");
    fwrite(&filler, 1, sizeof(filler), fileOut);
}

static short ToInt16(char* value, int startIndex)
{
	char* numRef = &value[startIndex];    
    if ((startIndex % 2) == 0)
    {
        return *(((short*) numRef));
    }
    if (1) //IsLittleEndian
    {
        return (short) (numRef[0] | (numRef[1] << 8));
    }
    return (short) ((numRef[0] << 8) | numRef[1]);
}

static void GetBytes(short value, char* output)
{
	char* arr = (char*)&value;
	output[0] = arr[0];
	output[1] = arr[1];
}

//return length
static int ConvertFromStereoToMono(int length, char* input, char* output)
{
	int outputIndex = 0;
	int i = 0;

    for (; i < length; i += 4)
    {
        // copy in the first 16 bit sample
        output[outputIndex++] = input[i];
        output[outputIndex++] = input[i + 1];

		//int channel1 = ToInt16(input, i);
		//int channel2 = ToInt16(input, i + 2);
		//short channelMixed = (short)(channel1 + channel2)/2;
		//GetBytes(channelMixed, output + outputIndex);
		//outputIndex += 2;
    }
    return outputIndex;
}


int main(){
  OggVorbis_File vf;
  int eof=0;
  int current_section;

  int channelCount;
  long sampleCount;
  int rate;

  FILE* fileIn = fopen("music_level.ogg" , "rb");
  FILE* fileOut = fopen("music_level.wav" , "wb");

  if (!fileIn || !fileOut)
	  return;

#ifdef _WIN32 /* We need to set stdin/stdout to binary mode. Damn windows. */
  /* Beware the evil ifdef. We avoid these where we can, but this one we 
     cannot. Don't add any more, you'll probably go to hell if you do. */
  _setmode( _fileno( stdin ), _O_BINARY );
  _setmode( _fileno( stdout ), _O_BINARY );
#endif

  if(ov_open_callbacks(/*stdin*/fileIn, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
      fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
      exit(1);
  }

  /* Throw the comments plus a few lines about the bitstream we're
     decoding */
  {
	vorbis_info *vi=ov_info(&vf,-1);
	char **ptr=ov_comment(&vf,-1)->user_comments;    
    while(*ptr){
      fprintf(stderr,"%s\n",*ptr);
      ++ptr;
    }
    fprintf(stderr,"\nBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
    fprintf(stderr,"\nDecoded length: %ld samples\n",
            (long)ov_pcm_total(&vf,-1));
    fprintf(stderr,"Encoded by: %s\n\n",ov_comment(&vf,-1)->vendor);

	channelCount = vi->channels;
	sampleCount = (long)ov_pcm_total(&vf,-1);
	rate = vi->rate;
  }
  
  write_wav_header(fileOut, sampleCount, /*channelCount*/1, rate);

  while(!eof){
    long byteCount = ov_read(&vf,pcmout,sizeof(pcmout),0,2,1,&current_section);
    if (byteCount == 0) {
      /* EOF */
      eof=1;
    } else if (byteCount < 0) {
      if(byteCount==OV_EBADLINK){
        fprintf(stderr,"Corrupt bitstream section! Exiting.\n");
        exit(1);
      }

      /* some other error in the stream.  Not a problem, just reporting it in
         case we (the app) cares.  In this case, we don't. */
    } else {
      /* we don't bother dealing with sample rate changes, etc, but
         you'll have to*/

		//fwrite(pcmout, 1, byteCount, fileOut);

		int byteCountNew = ConvertFromStereoToMono(byteCount, pcmout, pcmout2);
		fwrite(pcmout2, 1, byteCountNew, fileOut);
    }
  }

  /* cleanup */
  ov_clear(&vf);
    
  fprintf(stderr,"Done.\n");

  fclose(fileIn);
  fclose(fileOut);

  return(0);
}
