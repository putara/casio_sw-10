#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <memory.h>

#undef interface
#define interface             struct __declspec(novtable)

#define TRACE(f, ...)         Debug::Trace(f, __VA_ARGS__)
#define ASSERT(e)             if (!(e)) TRACE("Assertion failed: %s(%u): %s\n", __FILE__, __LINE__, #e)

#define MAKE_SMF_S(code)      (code)
#define MAKE_SMF_E(id)        (-(id))

#ifndef mmioFOURCC
#define mmioFOURCC(ch0, ch1, ch2, ch3)                \
            ((Dword)(Byte)(ch0) | ((Dword)(Byte)(ch1) << 8) |\
            ((Dword)(Byte)(ch2) << 16) | ((Dword)(Byte)(ch3) << 24 ))
#endif

typedef enum SMFRESULT
{
  SMF_OK                      = 0,
  SMF_S_USER_ABORTED          = MAKE_SMF_S(1),
  SMF_S_END_OF_FILE           = MAKE_SMF_S(2),
  SMF_S_SMF_REACHED_TKMAX     = MAKE_SMF_S(3),
  SMF_S_SMF_END_OF_TRACK      = MAKE_SMF_S(4),
  SMF_E_NO_MEMORY             = MAKE_SMF_E(5),
  SMF_E_UNEXPECTED            = MAKE_SMF_E(6),
  SMF_E_SMF_OPEN_FILE         = MAKE_SMF_E(7),
  SMF_E_SMF_TOO_BIG_FILE      = MAKE_SMF_E(8),
  SMF_E_SMF_UNKNOWN_FORMAT    = MAKE_SMF_E(9),
  SMF_E_SMF_INVALID_DATA      = MAKE_SMF_E(10),
  SMF_E_DRIVER_OPEN           = MAKE_SMF_E(11),
  SMF_E_DRIVER_AUDIOFORMAT    = MAKE_SMF_E(12),
} SMFRESULT;

typedef void *PVoid;
typedef const void *PCVoid;
typedef uint8_t Byte, *PByte;
typedef const uint8_t *PCByte;
typedef uint16_t Word, *PWord;
typedef const uint16_t *PCWord;
typedef uint32_t Dword, *PDword;
typedef const uint32_t *PCDword;
typedef uint64_t Qword, *PQword;
typedef const uint64_t *PCQword;

typedef Dword      SMFTICKS;
typedef int64_t    MIDITIME;

namespace FourCC
{
  const Dword RIFF            = mmioFOURCC('R','I','F','F');
  const Dword RMID            = mmioFOURCC('R','M','I','D');
  const Dword data            = mmioFOURCC('d','a','t','a');
  const Dword MThd            = mmioFOURCC('M','T','h','d');
  const Dword MTrk            = mmioFOURCC('M','T','r','k');
}

namespace SMF
{
  const SMFTICKS MAX_TICKS    = ~0U;
}

#define MAKE_MIDIEVENT(b1, b2, b3)  ((Dword)(((Byte)(b1)|((Word)((Byte)(b2))<<8))|(((Dword)(Byte)(b3))<<16)))
#define MIDIEVENT_CHANNEL(msg)      ((Byte)(((Dword)(msg)) & 0x0000000F))
#define MIDIEVENT_MSG(msg)          ((Byte)(((Dword)(msg)) & 0x000000F0))
#define MIDIEVENT_TYPE(msg)         ((Byte)(((Dword)(msg)) & 0x000000FF))
#define MIDIEVENT_DATA1(msg)        ((Byte)((((Dword)(msg)) & 0x0000FF00) >> 8))
#define MIDIEVENT_DATA2(msg)        ((Byte)((((Dword)(msg)) & 0x007F0000) >> 16))

namespace MIDI
{
  const Dword DEFAULT_TEMPO   = 500000; // 120bpm

  const Byte MSG              = static_cast<Byte>(0x80);
  const Byte NOTEOFF          = static_cast<Byte>(0x80);
  const Byte NOTEON           = static_cast<Byte>(0x90);
  const Byte POLYPRESSURE     = static_cast<Byte>(0xA0);
  const Byte CONTROLCHANGE    = static_cast<Byte>(0xB0);
  const Byte PROGRAMCHANGE    = static_cast<Byte>(0xC0);
  const Byte CHANPRESSURE     = static_cast<Byte>(0xD0);
  const Byte PITCHBEND        = static_cast<Byte>(0xE0);
  const Byte METAEVENT        = static_cast<Byte>(0xFF);
  const Byte SYSEX            = static_cast<Byte>(0xF0);
  const Byte SYSEXEND         = static_cast<Byte>(0xF7);

  namespace META
  {
    const Byte TRACKNAME      = static_cast<Byte>(0x03);
    const Byte EOT            = static_cast<Byte>(0x2F);
    const Byte TEMPO          = static_cast<Byte>(0x51);
    const Byte TIMESIG        = static_cast<Byte>(0x58);
    const Byte KEYSIG         = static_cast<Byte>(0x59);
    const Byte SEQSPECIFIC    = static_cast<Byte>(0x7F);
  }

  namespace CTRL
  {
    namespace BANKSELECT
    {
      const Byte MSB          = static_cast<Byte>(0);
      const Byte LSB          = static_cast<Byte>(32);
    }
    namespace DATAENTRY
    {
      const Byte MSB          = static_cast<Byte>(6);
      const Byte LSB          = static_cast<Byte>(38);
    }
    const Byte MODULATION     = static_cast<Byte>(1);
    const Byte PORTAMENTOTIME = static_cast<Byte>(5);
    const Byte VOLUME         = static_cast<Byte>(7);
    const Byte PANPOT         = static_cast<Byte>(10);
    const Byte EXPRESSION     = static_cast<Byte>(11);
    const Byte HOLD           = static_cast<Byte>(64);
    const Byte PORTAMENTO     = static_cast<Byte>(65);
    const Byte SOSTENUTO      = static_cast<Byte>(66);
    const Byte SOFT           = static_cast<Byte>(67);
    const Byte REVERB         = static_cast<Byte>(91);
    const Byte CHORUS         = static_cast<Byte>(93);
    const Byte DELAY          = static_cast<Byte>(94);
    namespace NRPN
    {
      const Byte LSB          = static_cast<Byte>(98);
      const Byte MSB          = static_cast<Byte>(99);
    }
    namespace RPN
    {
      const Byte LSB          = static_cast<Byte>(100);
      const Byte MSB          = static_cast<Byte>(101);
    }
    const Byte ALLSOUNDOFF    = static_cast<Byte>(120);
    const Byte RESETCTRL      = static_cast<Byte>(121);
    const Byte ALLNOTEOFF     = static_cast<Byte>(123);
  }
}


struct Debug {
  static void Trace(const char* format, ...) {
    va_list argPtr;
    va_start(argPtr, format);
    // char buffer[1025];
    // ::_vsnprintf_s(buffer, _TRUNCATE, format, argPtr);
    // ::OutputDebugStringA(buffer);
    ::vprintf(format, argPtr);
    va_end(argPtr);
  }

  static void Trace(const wchar_t* format, ...) {
    va_list argPtr;
    va_start(argPtr, format);
    // wchar_t buffer[1025];
    // ::_vsnwprintf_s(buffer, _TRUNCATE, wformat, argPtr);
    // ::OutputDebugStringW(buffer);
    ::vwprintf(format, argPtr);
    va_end(argPtr);
  }
};

class MidiTime {
private:
  static uint64_t UI32x32To64(uint32_t a, uint32_t b) {
    return static_cast<uint64_t>(a) * b;
  }
  static uint32_t UI64To32(uint64_t a) {
    return a >= 0xffffffff ? 0xffffffff : static_cast<uint32_t>(a);
  }
  static int64_t I64x32Div32(int64_t a, int32_t b, int32_t c) {
    // return a * b / c;
#ifndef HAVE_WXUTIL
    return static_cast<int64_t>(a * b / c); // ignore integer overflow
#else
    return Int64x32Div32(a, b, c);
#endif
  }
  static int64_t I64MulDiv(int64_t a, int64_t b, int64_t c) {
    // return a * b / c;
#ifndef HAVE_WXUTIL
    return a * b / c; // ignore integer overflow
#else
    return llMulDiv(a, b, c);
#endif
  }
public:
  static const Dword MILLISECONDS = (1000);         // 10 ^ 3
  static const Dword NANOSECONDS = (1000000000);    // 10 ^ 9
  static const Dword UNITS = (NANOSECONDS / 100);   // 10 ^ 7
  static const MIDITIME MAX = 0x7FFFFFFFFFFFFFFFi64;

  static inline MIDITIME SmfTicksToMidiTime(SMFTICKS ticks, Dword tempo, Dword division) {
    ASSERT((division & ~0xFFFF) == 0);
    if (division & 0x8000) {
      Dword smpte = -static_cast<int>(static_cast<signed char>((division >> 8) & 0xFF));
      if (smpte == 29) {
        smpte = 30;
      }
      const Dword ticksPerSec = smpte * static_cast<Byte>(division & 0xFF);
//      return I64x32Div32(ticks, UNITS, ticksPerSec, 0);
      return UI32x32To64(ticks, UNITS) / ticksPerSec;
    }
    // result = ticks * tempo * (UNITS / MILLISECONDS / 1000) / division
    return I64x32Div32(UI32x32To64(ticks, tempo), (UNITS / MILLISECONDS) / 1000, division);
  }
  static inline Dword MidiTimeToSmfTicks(MIDITIME miditime, Dword tempo, Dword division) {
    ASSERT(miditime >= 0);
    ASSERT((division & ~0xFFFF) == 0);
    int64_t result;
    if (division & 0x8000) {
      Dword smpte = -static_cast<int>(static_cast<signed char>((division >> 8) & 0xFF));
      if (smpte == 29) {
        smpte = 30;
      }
      const Dword ticksPerSec = smpte * static_cast<Byte>(division & 0xFF);
      result = I64x32Div32(miditime, ticksPerSec, UNITS);
    } else {
      // result = miditime * division / (UNITS / MILLISECONDS) * 1000 / tempo
      result = I64x32Div32(I64x32Div32(miditime, division, (UNITS / MILLISECONDS)), 1000, tempo);
    }
    return UI64To32(result);
  }
  static inline MIDITIME PerfCounterToMidiTime(int64_t counter, int64_t frequency) {
    // result = counter * UNITS / frequency
    ASSERT(counter >= 0);
    ASSERT(frequency > 0);
    return I64MulDiv(counter, UNITS, frequency);
  }
  static inline Dword MidiTimeToMilliseconds(MIDITIME miditime) {
    return UI64To32(MidiTimeToMilliseconds64(miditime));
  }
  static inline int64_t MidiTimeToMilliseconds64(MIDITIME miditime) {
    if (miditime == MAX) {
      return MAX;
    }
    const Dword UPM = (UNITS / MILLISECONDS);
    return miditime / UPM;
  }
};

class Smf;
class SmfEvent;
class SmfTrack;

interface ISmfEventSink {
  virtual bool OnSmfEvent(const Smf& smf, const SmfEvent& event) = 0;
};

class Stream {
private:
  PCByte cur, end, begin;

public:
  Stream() : cur(), end(), begin() { }
  ~Stream() { }

  Stream(__in_bcount(cb) PCVoid p, rsize_t cb) {
    this->cur = static_cast<PCByte>(p);
    this->end = this->cur + cb;
    this->begin = this->cur;
  }
  Stream(__in const Stream& that) {
    operator =(that);
  }
  Stream& operator =(__in const Stream& that) {
    this->cur = that.cur;
    this->end = that.end;
    this->begin = that.begin;
    return *this;
  }
  void Init(__in const Stream& that, rsize_t cb) {
    ASSERT(that.end >= that.cur);
    if (that.end < that.cur || static_cast<rsize_t>(that.end - that.cur) < cb) {
      cb = 0;
    }
    this->cur = that.cur;
    this->end = that.cur + cb;
    this->begin = that.cur;
  }
  bool IsEOF() const {
    return (this->cur >= this->end);
  }
  PCVoid GetCurrentPtr(__out rsize_t* remains) const {
    *remains = this->end - this->cur;
    return this->cur;
  }
  bool GetByte(__out PByte p) {
    if (IsEOF()) {
      return false;
    }
    *p = *this->cur++;
    return true;
  }
  void Rewind() {
    this->cur = this->begin;
  }
  bool SeekTo(__in rsize_t relOffset) {
    PCByte p = this->cur + relOffset;
    if ((p >= this->cur) && (p < this->end)) {
      this->cur = p;
      return true;
    }
    this->cur = this->end;
    return false;
  }
  bool Read(__out_bcount(cb) void* p, size_t cb) {
    PByte dst = static_cast<PByte>(p);
    while (cb-- > 0) {
      if (this->GetByte(dst) == false) {
        return false;
      }
      dst++;
    }
    return true;
  }
  template <typename T>
  bool Read(__out T* p) {
    return this->Read(p, sizeof(T));
  }
  bool ReadWord(__out Word* p) {
    Byte b[2];
    bool ret = this->Read(b, sizeof(b));
    if (ret) {
      *p = (b[1]) | (b[0] << 8);
    }
    return ret;
  }
  bool ReadDword(__out Dword* p) {
    Byte b[4];
    bool ret = this->Read(b, sizeof(b));
    if (ret) {
      *p = (b[3]) | (b[2] << 8) | (b[1] << 16) | (b[0] << 24);
    }
    return ret;
  }
  bool ReadDwordLE(__out Dword* p) {
    Byte b[4];
    bool ret = this->Read(b, sizeof(b));
    if (ret) {
      *p = (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | (b[0]);
    }
    return ret;
  }
  bool ReadVDword(__out_range(0, 0x0FFFFFFF) Dword* p) {
    Dword c = 4, d = 0;
    Byte b;
    do {
      if (--c == 0) {
        TRACE("ReadVDWord: overflow\n");
        return false;
      }
      if (this->GetByte(&b) == false) {
        return false;
      }
      d = (d << 7) | (b & 0x7F);
    } while (b & 0x80);
    *p = d;
    return true;
  }
};

class SmfEvent {
private:
  static const Dword MAX_SMALL_EVENT_BYTES = 4;

  SMFTICKS  absTicks;
  MIDITIME  time;
  Dword     cbEvent;
  Dword     cbAllocate;
  PByte     event;
  Byte      smallEvent[MAX_SMALL_EVENT_BYTES];

  void Free() {
    if (this->event != this->smallEvent) {
      delete[] this->event;
    }
    this->event = nullptr;
    this->cbAllocate = 0;
  }

public:
  SmfEvent() : absTicks(), time(), cbEvent(), cbAllocate() { this->event = this->smallEvent; }
  ~SmfEvent() { this->Free(); }

  SMFTICKS GetTicks() const { return this->absTicks; }
  MIDITIME GetMidiTime() const { return this->time; }
  Dword GetSize() const { return this->cbEvent; }
  PCByte GetRawData() const { return this->event; }
  Byte GetType() const { ASSERT(this->cbEvent > 0); return this->event[0]; }
  Byte GetMetaType() const { ASSERT(this->cbEvent > 1 && this->event[0] == MIDI::METAEVENT); return this->event[1]; }
  void SetTicks(SMFTICKS absTicks) { this->absTicks = absTicks; }
  void SetMidiTime(MIDITIME mt) { this->time = mt; }

  PByte Allocate(const Dword cb) {
    if (cb > this->cbAllocate) {
      this->Free();
      if (cb <= MAX_SMALL_EVENT_BYTES) {
        this->event = this->smallEvent;
        this->cbAllocate = MAX_SMALL_EVENT_BYTES;
      } else {
        PByte const p = new Byte[cb];
        this->event = p;
        this->cbAllocate = cb;
      }
    }
    this->cbEvent = cb;
    ::memset(this->event, 0, cb);
    return this->event;
  }
};


class SmfTrack {
private:
  static const Dword MAX_EVENT_SIZE = 0xffffff;
  Stream    stream;
  SMFTICKS  absNext;
  Byte      runningStatus;
  bool      endOfTrack;

  static bool IsMessageLengthThreeBytes(Byte status) {
    // msg       0x80, 0x90, 0xA0, 0xB0, 0xC0,  0xD0,  0xE0
    // 3 bytes?  true, true, true, true, false, false, true
    return (0x4F & (1 << ((status >> 4) & 7))) != 0;
  }

public:
  SmfTrack() : absNext(), runningStatus(), endOfTrack() { }

  bool IsEnd() const { return this->endOfTrack; }
  SMFTICKS GetNextEventTicks() const { return this->absNext; }

  SMFRESULT Init(__inout Stream& smf) {
    Dword ckid, cb;
    if (smf.Read(&ckid) == false || smf.ReadDword(&cb) == false) {
      return SMF_E_SMF_INVALID_DATA;
    }
    if (ckid == FourCC::MTrk) {
      this->stream.Init(smf, cb);
      smf.SeekTo(cb);
      this->endOfTrack = (cb == 0);
    } else {
      this->stream.SeekTo(SIZE_MAX);
      smf.SeekTo(cb);
      this->endOfTrack = true;
    }
    return this->ReadNextDeltaTime();
  }
  SMFRESULT GetEvent(__in MIDITIME miditime, __out SmfEvent* event) {
    event->SetTicks(this->absNext);
    event->SetMidiTime(miditime);
    Byte firstByte;
    if (this->stream.GetByte(&firstByte) == false) {
      return SMF_E_SMF_INVALID_DATA;
    }
    SMFRESULT ret;
    if (firstByte < MIDI::MSG) {
      ret = this->GetEvent_MessageWithoutStatus(firstByte, event);
    } else if (firstByte < MIDI::SYSEX) {
      ret = this->GetEvent_Message(firstByte, event);
    } else if (firstByte == MIDI::SYSEX || firstByte == MIDI::SYSEXEND) {
      ret = this->GetEvent_Sysex(firstByte, event);
    } else if (firstByte == MIDI::METAEVENT) {
      ret = this->GetEvent_Meta(firstByte, event);
      if (ret == SMF_S_SMF_END_OF_TRACK) {
        return SMF_OK;
      }
    } else {
      ret = SMF_E_SMF_INVALID_DATA;
    }
    if (ret != SMF_OK) {
      return ret;
    }
    return ReadNextDeltaTime();
  }

private:
  SMFRESULT ReadNextDeltaTime() {
    if (this->endOfTrack) {
      return SMF_S_SMF_REACHED_TKMAX;
    }
    SMFTICKS relDelta;
    if (this->stream.ReadVDword(&relDelta) == false) {
      return SMF_E_SMF_INVALID_DATA;
    }
    SMFTICKS absTicks = this->absNext + relDelta;
    if (absTicks < this->absNext) {
      return SMF_E_SMF_INVALID_DATA;
    }
    this->absNext = absTicks;
    return SMF_OK;
  }
  SMFRESULT GetEvent_MessageWithoutStatus(Byte firstByte, __out SmfEvent* event) {
    if (this->runningStatus == 0) {
      return SMF_E_SMF_INVALID_DATA;
    }
    const bool threeBytes = IsMessageLengthThreeBytes(this->runningStatus);
    Byte* p = event->Allocate(threeBytes ? 3 : 2);
    *p++ = this->runningStatus;
    *p++ = firstByte;
    if (threeBytes && this->stream.GetByte(p) == false) {
      return SMF_E_SMF_INVALID_DATA;
    }
    return SMF_OK;
  }
  SMFRESULT GetEvent_Message(Byte firstByte, __out SmfEvent* event) {
    this->runningStatus = firstByte;
    if (this->stream.GetByte(&firstByte) == false) {
      return SMF_E_SMF_INVALID_DATA;
    }
    return this->GetEvent_MessageWithoutStatus(firstByte, event);
  }
  SMFRESULT GetEvent_Sysex(Byte firstByte, __out SmfEvent* event) {
    Dword cbEvent;
    if (this->stream.ReadVDword(&cbEvent) == false) {
      return SMF_E_SMF_INVALID_DATA;
    }
    if (cbEvent > MAX_EVENT_SIZE) {
      return SMF_E_SMF_INVALID_DATA;
    }
    if (firstByte == MIDI::SYSEX) {
      cbEvent++;
    }
    Byte* p = event->Allocate(cbEvent);
    if (firstByte == MIDI::SYSEX) {
      *p++ = 0xF0;
      cbEvent--;
    }
    for (; cbEvent-- > 0; p++) {
      if (this->stream.GetByte(p) == false) {
        return SMF_E_SMF_INVALID_DATA;
      }
    }
    return SMF_OK;
  }
  SMFRESULT GetEvent_Meta(Byte firstByte, __out SmfEvent* event) {
    Byte meta;
    if (this->stream.GetByte(&meta) == false) {
      return SMF_E_SMF_INVALID_DATA;
    }
    Dword cbEvent;
    if (this->stream.ReadVDword(&cbEvent) == false) {
      return SMF_E_SMF_INVALID_DATA;
    }
    if (cbEvent > MAX_EVENT_SIZE) {
      return SMF_E_SMF_INVALID_DATA;
    }
    Byte* p = event->Allocate(cbEvent + 2);
    *p++ = firstByte;
    *p++ = meta;
    for (; cbEvent-- > 0; p++) {
      if (this->stream.GetByte(p) == false) {
        return SMF_E_SMF_INVALID_DATA;
      }
    }
    if (meta == MIDI::META::EOT) {
      this->endOfTrack = true;
      return SMF_S_SMF_END_OF_TRACK;
    }
    return SMF_OK;
  }
};

class Smf {
private:
  size_t    cTracks;
  SmfTrack* tracks;
  Dword     division;
  SMFTICKS  absPosition;
  Dword     tempo;
  SMFTICKS  absTempo;
  MIDITIME  tempoTime;

  struct SmfHeader {
    Word format;
    Word tracks;
    Word division;
  };

public:
  Smf() : cTracks(), tracks(), division(), tempo(MIDI::DEFAULT_TEMPO), absTempo(), tempoTime() { }
  ~Smf() { delete[] this->tracks; }

  SMFTICKS GetPosition() const { return this->absPosition; }

  bool IsEOF() const {
    for (const SmfTrack *track = this->tracks, *trkEnd = this->tracks + this->cTracks; track < trkEnd; track++) {
      if (track->IsEnd() == false) {
        return false;
      }
    }
    return true;
  }
  MIDITIME SmfTicksToMidiTime(SMFTICKS absTicks) const {
    ASSERT(absTicks >= this->absTempo);
    return this->tempoTime + MidiTime::SmfTicksToMidiTime(absTicks - this->absTempo, this->tempo, this->division);
  }
  SMFTICKS MidiTimeToSmfTicks(MIDITIME miditime) const {
    ASSERT(miditime >= this->tempoTime);
    return this->absTempo + MidiTime::MidiTimeToSmfTicks(miditime - this->tempoTime, this->tempo, this->division);
  }
  SMFRESULT Init(__in_bcount(cb) PCByte const p, rsize_t cb) {
    Stream stream(p, cb);
    SkipRiffMidiHeader(p, cb, &stream);
    SMFRESULT ret;
    SmfHeader hdr;
    if ((ret = ParseSmfHeader(stream, &hdr)) != SMF_OK) {
      return ret;
    }
    stream.Rewind();
    // 14 = offset to the first track
    if (stream.SeekTo(14) == false) {
      return SMF_E_SMF_UNKNOWN_FORMAT;
    }
    this->cTracks = hdr.tracks;
    this->division = hdr.division;
    this->tracks = new SmfTrack[this->cTracks];
    for (size_t i = 0; i < this->cTracks && (ret = this->tracks[i].Init(stream)) == SMF_OK; i++);
    return ret;
  }
  SMFRESULT EnumerateEventsTillMidiTime(MIDITIME miditime, ISmfEventSink* sink) {
    SmfEvent event;
    SMFRESULT ret;
    for (;;) {
      ret = this->GetNextEvent(miditime, &event);
      if (ret != SMF_OK) {
        if (ret != SMF_S_SMF_REACHED_TKMAX) {
          return ret;
        } else {
          ret = SMF_OK;
          break;
        }
      }
      if (event.GetType() == MIDI::METAEVENT) {
        if (event.GetMetaType() == MIDI::META::TEMPO && (ret = this->SetNewTempo(event)) != SMF_OK) {
          return ret;
        }
      }
      if (sink->OnSmfEvent(*this, event) == false) {
        return SMF_S_USER_ABORTED;
      }
    }
    return ret;
  }

private:
  static void SkipRiffMidiHeader(__in_bcount(cb) PCByte const p, rsize_t cb, __inout Stream* stream) {
    Dword riff, rmid, data, size;
    if (stream->Read(&riff) && riff == FourCC::RIFF && stream->SeekTo(4) && stream->Read(&rmid) && rmid == FourCC::RMID && stream->Read(&data) && data == FourCC::data && stream->ReadDwordLE(&size)) {
      if (size > cb - 20) {
        size = static_cast<Dword>(cb - 20);
      }
      *stream = Stream(p + 20, size);
    } else {
      stream->Rewind();
    }
  }
  static bool ValidateDivision(Word division) {
    if (division & 0x8000) {
      Dword smpte = -static_cast<int>(static_cast<signed char>((division >> 8) & 0xFF));
      if (smpte == 29) {
        smpte = 30;
      }
      const Dword ticksPerSec = smpte * static_cast<Byte>(division & 0xFF);
      return (ticksPerSec != 0);
    }
    return (division != 0);
  }
  static SMFRESULT ParseSmfHeader(Stream& stream, SmfHeader* hdr) {
    Dword ckid;
    if (stream.Read(&ckid) == false) {
      return SMF_E_SMF_UNKNOWN_FORMAT;
    } else if (ckid != FourCC::MThd) {
      return SMF_E_SMF_UNKNOWN_FORMAT;
    }
    Dword cb;
    return stream.ReadDword(&cb) && stream.ReadWord(&hdr->format) && stream.ReadWord(&hdr->tracks) && stream.ReadWord(&hdr->division)
      && cb == 6 && hdr->format <= 1 && hdr->tracks > 0 && ValidateDivision(hdr->division) ? SMF_OK : SMF_E_SMF_INVALID_DATA;
  }
  SMFRESULT GetNextEvent(MIDITIME maxTime, SmfEvent* event) {
    SMFTICKS relMin = SMF::MAX_TICKS;
    SmfTrack* found = nullptr;
    for (SmfTrack *track = this->tracks, *trkEnd = this->tracks + this->cTracks; track < trkEnd; track++) {
      if (track->IsEnd()) {
        continue;
      }
      SMFTICKS absNext = track->GetNextEventTicks();
      SMFTICKS relTicks = absNext - this->absPosition;
      if (relTicks < relMin) {
        relMin = relTicks;
        found = track;
        if (relMin == 0) {
          break;
        }
      }
    }
    if (found == nullptr) {
      return SMF_S_END_OF_FILE;
    }
    if (maxTime < MidiTime::MAX) {
      const SMFTICKS absMaxTicks = MidiTimeToSmfTicks(maxTime);
      if (this->absPosition + relMin >= absMaxTicks) {
        return SMF_S_SMF_REACHED_TKMAX;
      }
    }
    this->absPosition = found->GetNextEventTicks();
    const MIDITIME miditime = SmfTicksToMidiTime(this->absPosition);
    return found->GetEvent(miditime, event);
  }
  SMFRESULT SetNewTempo(const SmfEvent& event) {
    if (event.GetSize() != 5) {
      return SMF_E_SMF_INVALID_DATA;
    } else if (this->division & 0x8000) {
      return SMF_OK;
    }
    PCByte const p = event.GetRawData() + 2;
    const Dword tempo = p[2] | (p[1] << 8) | (p[0] << 16);
    if (tempo == 0) {
      return SMF_E_SMF_INVALID_DATA;
    }
    this->tempo = tempo;
    this->absTempo = event.GetTicks();
    this->tempoTime = event.GetMidiTime();
    return SMF_OK;
  }
};
