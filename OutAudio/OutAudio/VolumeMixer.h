#pragma once

#ifndef _VolumeMixer_H_
#define _VolumeMixer_H_

#ifndef Uint8
#define Uint8 unsigned char
#endif // !Uint8

#ifndef Uint32
#define Uint32 unsigned long
#endif

#ifndef Uint16
#define Uint16 unsigned short
#endif

#ifndef Sint16
#define Sint16 short
#endif

#ifndef Sint8
#define Sint8 char
#endif

#define AUDIO_U8        0x0008  /**< Unsigned 8-bit samples */
#define AUDIO_S8        0x8008  /**< Signed 8-bit samples */
#define AUDIO_U16LSB    0x0010  /**< Unsigned 16-bit samples */
#define AUDIO_S16LSB    0x8010  /**< Signed 16-bit samples */
#define AUDIO_U16MSB    0x1010  /**< As above, but big-endian byte order */
#define AUDIO_S16MSB    0x9010  /**< As above, but big-endian byte order */
#define AUDIO_U16       AUDIO_U16LSB
#define AUDIO_S16       AUDIO_S16LSB


void MixAudioFormat(Uint8 * dst, const Uint8 * src, int format, Uint32 len, int volume);

#endif