/*
 * wav_recorder.c
 *
 *  Created on: 30 Oct 2020
 *      Author: Mohamed Yaqoob
 */

#include "wav_recorder.h"
#include "fatfs.h"
#include "adc.h"
#include "tim.h"

//WAV File System
static FIL wavFile;
static UINT recorderWriteBytes = 0;

//WAV Audio Buffer
#define AUDIO_BUFFER_SIZE  4096
static int16_t audioBuffer[AUDIO_BUFFER_SIZE];
static __IO uint32_t audioAccumSize;
static bool isFinished=0;
static WAV_HeaderTypeDef wavHeader;

#define WAV_RECORDER_ADC_START() \
  HAL_ADC_Start_DMA(&adc1Handle, (uint32_t *)audioBuffer, AUDIO_BUFFER_SIZE); \
  HAL_TIM_Base_Start(&htim3)

#define WAV_RECORDER_ADC_STOP() \
  HAL_ADC_Stop_DMA(&adc1Handle); \
  HAL_TIM_Base_Stop(&htim3)

//WAV Recorder process states
typedef enum
{
  Recorder_Control_Idle=0,
  Recorder_Control_HalfBuffer,
  Recorder_Control_FullBuffer,
  Recorder_Control_EndOfFile,
}Recorder_Control_e;
static volatile Recorder_Control_e recorderControlSM = Recorder_Control_Idle;

//Recorder reset
static void wavRecorder_reset(void)
{
  audioAccumSize=0;
  recorderWriteBytes=0;
  recorderControlSM = Recorder_Control_Idle;
}

/**
 * @brief Create WAV Recording file
 */
bool wav_recorder_fileCreate(const char* filePath)
{
  //Create WAV Header
  wavHeader.ChunkID = 0x46464952;
  wavHeader.FileFormat = 0x45564157;
  wavHeader.SubChunk1ID = 0x20746d66;
  wavHeader.SubChunk1Size = 16;             //16-bit
  wavHeader.AudioFormat = 1;                //PCM
  wavHeader.NbrChannels = 1;                //Mono
  wavHeader.SampleRate = 44100;              //44.1KHz
  wavHeader.BitPerSample = 16;
  wavHeader.ByteRate = wavHeader.SampleRate*wavHeader.NbrChannels*wavHeader.BitPerSample/8;
  wavHeader.BlockAlign = wavHeader.NbrChannels*wavHeader.BitPerSample/8;
  wavHeader.SubChunk2ID = 0x61746164;
  //Note at the end of recording, need to set the following header parameters:
  //  - FileSize = 36 + SubChunk2Size
  //  - SubChunk2Size = NumSamples * NumChannels * BitsPerSample/8
  //where NumSamples: The number of Mic Data samples

//  wavHeader.SubChunk2Size = 40960*wavHeader.NbrChannels*wavHeader.BitPerSample/8;
//  wavHeader.FileSize = 36 + wavHeader.SubChunk2Size;

  //Open WAV file for Write/Overwrite
  if(f_open(&wavFile, filePath, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
  {
    return false;
  }
  //Skip Header section (44 Bytes)
  f_lseek(&wavFile, 44);
//  f_write(&wavFile, (uint8_t *)&wavHeader, 44, &recorderWriteBytes);
  return true;
}

/**
 * @brief Start Recording
 */
void wav_recorder_record(void)
{
  isFinished = false;
  wavRecorder_reset();
  //Start ADC
  WAV_RECORDER_ADC_START();
}

/**
 * @brief Stop Recording
 */
void wav_recorder_stop(void)
{
  WAV_RECORDER_ADC_STOP();
  recorderControlSM = Recorder_Control_EndOfFile;
}

/**
 * @brief Process WAV
 */
void wav_recorder_process(void)
{
  switch(recorderControlSM)
  {
  case Recorder_Control_Idle:
    break;

  case Recorder_Control_HalfBuffer:
    recorderWriteBytes=0;
    for(uint16_t i=0; i<AUDIO_BUFFER_SIZE/2; i++)
    {
      audioBuffer[i]= 8*(audioBuffer[i] - 1900); //Remove DC Offset;
    }
    f_write(&wavFile, (uint8_t *)&audioBuffer[0], AUDIO_BUFFER_SIZE, &recorderWriteBytes);
    audioAccumSize+=AUDIO_BUFFER_SIZE/2;
    recorderControlSM = Recorder_Control_Idle;
    break;

  case Recorder_Control_FullBuffer:
    recorderWriteBytes=0;
    for(uint16_t i=0; i<AUDIO_BUFFER_SIZE/2; i++)
    {
      audioBuffer[AUDIO_BUFFER_SIZE/2 + i]= 8*(audioBuffer[AUDIO_BUFFER_SIZE/2 + i]-1900); //Remove DC Offset;
    }
    f_write(&wavFile, (uint8_t *)&audioBuffer[AUDIO_BUFFER_SIZE/2], AUDIO_BUFFER_SIZE, &recorderWriteBytes);
    audioAccumSize+=AUDIO_BUFFER_SIZE/2;
    recorderControlSM = Recorder_Control_Idle;
    break;

  case Recorder_Control_EndOfFile:
//    f_close(&wavFile);
//    f_open(&wavFile, filePath, FA_WRITE);
    f_rewind(&wavFile);
    wavHeader.SubChunk2Size = audioAccumSize*wavHeader.NbrChannels*wavHeader.BitPerSample/8;
    wavHeader.FileSize = 36 + wavHeader.SubChunk2Size;
    f_write(&wavFile, (uint8_t *)&wavHeader, 44, &recorderWriteBytes);
    f_close(&wavFile);
    wavRecorder_reset();
    isFinished = true;
    recorderControlSM = Recorder_Control_Idle;
    break;
  }
}

/**
 * @brief Is Finished
 */
bool wav_recorder_isFinished(void)
{
  return isFinished;
}

/**
 * @brief WAV Pause/Resume
 */
void wav_recorder_pause(void)
{
  WAV_RECORDER_ADC_STOP();
}
void wav_recorder_resume(void)
{
  WAV_RECORDER_ADC_START();
}

/**
 * @brief Half/Full transfer Audio callback for buffer management
 */
void wavRecorder_halfTransfer_Callback(void)
{
  recorderControlSM = Recorder_Control_HalfBuffer;
}
void wavRecorder_fullTransfer_Callback(void)
{
  recorderControlSM = Recorder_Control_FullBuffer;
}
