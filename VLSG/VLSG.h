/**
 *
 *  Copyright (C) 2022 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#if !defined(_VLSG_H_INCLUDED_)
#define _VLSG_H_INCLUDED_

#include <stdint.h>

#if defined(VLSG_STATIC) || !defined(WIN32)
#define VLSG_IMPORT
#define VLSG_EXPORT
#define VLSG_CALLTYPE
#else
#define VLSG_IMPORT         DECLSPEC_IMPORT
#define VLSG_EXPORT         DECLSPEC_EXPORT
#define VLSG_CALLTYPE       WINAPI
#endif

#ifdef VLSG_DLL
#define VLSG_API            VLSG_EXPORT int VLSG_CALLTYPE
#define VLSG_API_(type)     VLSG_EXPORT type VLSG_CALLTYPE
#else
#define VLSG_API            VLSG_IMPORT int VLSG_CALLTYPE
#define VLSG_API_(type)     VLSG_IMPORT type VLSG_CALLTYPE
#endif


#define VLSG_FALSE  0
#define VLSG_TRUE   1

typedef int VLSG_Bool;


enum ParameterType
{
    PARAMETER_OutputBuffer  = 1,
    PARAMETER_ROMAddress    = 2,
    PARAMETER_Frequency     = 3,
    PARAMETER_Polyphony     = 4,
    PARAMETER_Effect        = 5,
};

VLSG_API_(uint32_t) VLSG_GetVersion(void);
VLSG_API_(const char*) VLSG_GetName(void);
VLSG_API_(void) VLSG_SetFunc_GetTime(uint32_t (*get_time)(void));

VLSG_API_(VLSG_Bool)  VLSG_SetParameter(uint32_t type, uintptr_t value);
VLSG_API_(VLSG_Bool)  VLSG_Init(void);
VLSG_API_(VLSG_Bool)  VLSG_Exit(void);
VLSG_API_(void)       VLSG_Write(const void* data, uint32_t len);
VLSG_API_(int32_t)    VLSG_Buffer(uint32_t output_buffer_counter);

int32_t VLSG_PlaybackStart(void);
int32_t VLSG_PlaybackStop(void);
void VLSG_AddMidiData(uint8_t *ptr, uint32_t len);
int32_t VLSG_FillOutputBuffer(uint32_t output_buffer_counter);

#endif

