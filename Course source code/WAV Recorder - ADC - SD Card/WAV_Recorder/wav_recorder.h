/*
 * wav_recorder.h
 *
 *  Created on: 30 Oct 2020
 *      Author: Mohamed Yaqoob
 */

#ifndef WAV_RECORDER_WAV_RECORDER_H_
#define WAV_RECORDER_WAV_RECORDER_H_

#include "main.h"

//Audio buffer state
typedef enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF,
  BUFFER_OFFSET_FULL,
}BUFFER_StateTypeDef;

typedef enum
{
  Record_Idle=0,
  Record_Pause,
  Record_Resume,
}Record_State_e;

typedef struct
{
  uint32_t   ChunkID;       /* 0 */
  uint32_t   FileSize;      /* 4 */
  uint32_t   FileFormat;    /* 8 */
  uint32_t   SubChunk1ID;   /* 12 */
  uint32_t   SubChunk1Size; /* 16*/
  uint16_t   AudioFormat;   /* 20 */
  uint16_t   NbrChannels;   /* 22 */
  uint32_t   SampleRate;    /* 24 */

  uint32_t   ByteRate;      /* 28 */
  uint16_t   BlockAlign;    /* 32 */
  uint16_t   BitPerSample;  /* 34 */
  uint32_t   SubChunk2ID;   /* 36 */
  uint32_t   SubChunk2Size; /* 40 */

}WAV_HeaderTypeDef;


/**
 * @brief Create WAV Recording file
 */
bool wav_recorder_fileCreate(const char* filePath);

/**
 * @brief Start Recording
 */
void wav_recorder_record(void);

/**
 * @brief Stop Recording
 */
void wav_recorder_stop(void);

/**
 * @brief Process WAV
 */
void wav_recorder_process(void);

/**
 * @brief Is Finished
 */
bool wav_recorder_isFinished(void);

/**
 * @brief WAV Pause/Resume
 */
void wav_recorder_pause(void);
void wav_recorder_resume(void);

/**
 * @brief WAV Recording Callback
 */
void wavRecorder_halfTransfer_Callback(void);
void wavRecorder_fullTransfer_Callback(void);

#endif /* WAV_RECORDER_WAV_RECORDER_H_ */
