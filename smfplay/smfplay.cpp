#if _MSC_VER < 1600
#define nullptr       NULL
#endif

#include "smf.hpp"

#include <conio.h>

struct SMFLENGTH
{
  SMFTICKS    ticks;
  MIDITIME    miditime;
  Dword       milliseconds;
};

void* OpenFileAsMapping(__in const char* path, Qword maxSize, __out Qword* size);
void* OpenFileAsMapping(__in const wchar_t* path, Qword maxSize, __out Qword* size);
void  CloseFileMapping(__inout_opt void* p);
int64_t GetPerfCounter();
int64_t GetPerfFrequency();
void SleepMS(Dword ms);
int SetPriority(bool restore, int priority);

interface IMidiDevice {
  virtual void Release() = 0;
  virtual bool Send(PCByte data, Dword size) = 0;
};

IMidiDevice* CreateMidiDevice();

SMFRESULT CalculateLength(PCByte data, Dword size, __out SMFLENGTH* length) {
  struct StubEventSink : public ISmfEventSink {
    bool OnSmfEvent(const Smf&, const SmfEvent&) {
      return true;
    }
  };
  ::memset(length, 0, sizeof(*length));
  Smf smf;
  SMFRESULT ret = smf.Init(data, size);
  if (ret == SMF_OK) {
    StubEventSink sink;
    ret = smf.EnumerateEventsTillMidiTime(MidiTime::MAX, &sink);
  }
  if (ret == SMF_S_END_OF_FILE) {
    length->ticks = smf.GetPosition();
    length->miditime = smf.SmfTicksToMidiTime(length->ticks);
    length->milliseconds = MidiTime::MidiTimeToMilliseconds(length->miditime);
    ret = SMF_OK;
  } else {
    ret = ret == SMF_OK ? SMF_E_UNEXPECTED : ret;
  }
  return ret;
}

SMFRESULT Play(PCByte data, Dword size) {
  struct EventSink : public ISmfEventSink {
    IMidiDevice* dev;
    EventSink() : dev(CreateMidiDevice()) {  }
    ~EventSink() {
      if (this->dev != nullptr) {
        this->dev->Release();
      }
    }
    bool Init() {
      return this->dev != nullptr;
    }
    bool OnSmfEvent(const Smf&, const SmfEvent& event) {
      if (event.GetType() > 0xF0) {
        return true;
      }
      this->dev->Send(event.GetRawData(), event.GetSize());
      // for (Dword i = 0; i < event.GetSize(); i++) {
        // printf("%02x ", event.GetRawData()[i]);
      // }
      // printf("\n");
      return true;
    }
    void SysEx(const Byte* data, uint32_t size) {
      this->dev->Send(data, size);
    }
  };
  Smf smf;
  SMFRESULT ret = smf.Init(data, size);
  if (ret == SMF_OK) {
    const int64_t frequency = GetPerfFrequency();
    const int64_t start = GetPerfCounter();
    EventSink sink;
    if (!sink.Init()) {
      return SMF_E_DRIVER_OPEN;
    }
    do {
      if (::_kbhit()) {
        break;
      }
      int64_t counter = GetPerfCounter() - start;
      MIDITIME time = MidiTime::PerfCounterToMidiTime(counter, frequency);
      // printf("%I64d\n", time);
      ret = smf.EnumerateEventsTillMidiTime(time, &sink);
      SleepMS(1);
    } while (ret == SMF_OK);
  }
  if (ret >= SMF_OK) {
    return SMF_OK;
  }
  return ret;
}

int main(int argc, char** argv)
{
  if (argc < 2) {
    return 1;
  }
  Qword size64;
  PByte const data = static_cast<PByte>(OpenFileAsMapping(argv[1], 0xfffffff, &size64));
  if (data == nullptr) {
    printf("Cannot open file\n");
    return SMF_E_SMF_OPEN_FILE;
  }
  const Dword size = size64 & 0xffffffff;

  SMFRESULT ret;
  SMFLENGTH smfLength;
  ret = CalculateLength(data, size, &smfLength);
  if (ret == SMF_OK) {
    printf("SMF Length: %u:%02u.%03u\n"
      , smfLength.milliseconds / 1000 / 60
      , smfLength.milliseconds / 1000 % 60
      , smfLength.milliseconds % 1000);
    const int oldPriority = SetPriority(false, 0);
    ret = Play(data, size);
    SetPriority(true, oldPriority);
  }
  if (ret != SMF_OK) {
    printf("Error: %d\n", ret);
  }

  CloseFileMapping(data);
  return 0;
}

#undef mmioFOURCC
#undef interface
#include <windows.h>

void* OpenFileAsMapping(HANDLE hf, Qword maxSize, __out Qword* size) {
  *size = 0;
  void* data = nullptr;
  LARGE_INTEGER cb;
  cb.QuadPart = 0;
  if (::GetFileSizeEx(hf, &cb) && cb.QuadPart >= 0 && static_cast<Qword>(cb.QuadPart) < maxSize) {
    HANDLE hfm = ::CreateFileMapping(hf, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (hfm != nullptr) {
      data = ::MapViewOfFile(hfm, FILE_MAP_READ, 0, 0, 0);
      *size = cb.QuadPart;
      ::CloseHandle(hfm);
    }
  }
  return data;
}

void* OpenFileAsMapping(__in const char* path, Qword maxSize, __out Qword* size) {
  *size = 0;
  void* data = nullptr;
  HANDLE hf = ::CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
  if (hf != INVALID_HANDLE_VALUE) {
    data = OpenFileAsMapping(hf, maxSize, size);
    ::CloseHandle(hf);
  }
  return data;
}

void* OpenFileAsMapping(__in const wchar_t* path, Qword maxSize, __out Qword* size) {
  *size = 0;
  void* data = nullptr;
  HANDLE hf = ::CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
  if (hf != INVALID_HANDLE_VALUE) {
    data = OpenFileAsMapping(hf, maxSize, size);
    ::CloseHandle(hf);
  }
  return data;
}

void CloseFileMapping(__inout_opt void* p) {
  if (p != nullptr) {
    ::UnmapViewOfFile(p);
  }
}

int64_t GetPerfCounter() {
  LARGE_INTEGER x;
  ::QueryPerformanceCounter(&x);
  return x.QuadPart;
}

int64_t GetPerfFrequency() {
  LARGE_INTEGER x;
  ::QueryPerformanceFrequency(&x);
  return x.QuadPart;
}

void SleepMS(Dword ms) {
  ::Sleep(ms);
}

int SetPriority(bool restore, int priority) {
  HANDLE hthread = ::GetCurrentThread();
  if (restore) {
    ::SetThreadPriority(hthread, priority);
    return 0;
  } else {
    int oldPriority = ::GetThreadPriority(hthread);
    ::SetThreadPriority(hthread, THREAD_PRIORITY_HIGHEST);
    return oldPriority;
  }
}

#include <mmsystem.h>
#include <shlwapi.h>
#define VLSG_IMPORT_DLL
#include <vlsg.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "vlsg.lib")

struct LsgDevice : public IMidiDevice {
  static const UINT NUM_WBUFS = 16;
  volatile bool stopped;
  void* romData;
  void* waveBuf;
  UINT counter;
  HANDLE hthread;
  int mode;
  HWAVEOUT hwo;
  WAVEHDR whdr[NUM_WBUFS];

  enum lsgFreq
  {
    lsgFreq11KHz  = 0x00,
    lsgFreq22KHz  = 0x01,
    lsgFreq44KHz  = 0x02,
  };

  enum lsgPoly
  {
    lsgPoly24     = 0x10,
    lsgPoly32     = 0x11,
    lsgPoly48     = 0x12,
    lsgPoly64     = 0x13,
  };

  enum lsgFx
  {
    lsgFxOff      = 0x20,
    lsgFxReverb1  = 0x21,
    lsgFxReverb2  = 0x22,
  };

  LsgDevice() : stopped(), romData(), waveBuf(), counter(), hthread(), mode(), hwo() { }
  ~LsgDevice() { this->Close(); }
  void Release() {
    delete this;
  }
  bool Send(PCByte data, Dword size) {
    lsgWrite(data, size);
    return true;
  }

  bool Init() {
    this->stopped = true;
    // this->hthread = ::CreateThread(nullptr, 0, ThreadFunc, this, 0, nullptr);
    if (lsgInit() == false) {
      TRACE("lsgInit() failed\n");
    }
    lsgOpen();
    this->stopped = false;

    ::memset(this->whdr, 0, sizeof(this->whdr));
    WAVEFORMATEX wfmt = { WAVE_FORMAT_PCM };
    wfmt.nChannels = 2;
    wfmt.nSamplesPerSec = 44100;
    wfmt.wBitsPerSample = 16;
    wfmt.nBlockAlign = wfmt.nChannels * wfmt.wBitsPerSample / 8;
    wfmt.nAvgBytesPerSec = wfmt.nSamplesPerSec * wfmt.nBlockAlign;
    MMRESULT mmr = ::waveOutOpen(&this->hwo, WAVE_MAPPER, &wfmt, reinterpret_cast<DWORD_PTR>(waveOutProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
    if (mmr == MMSYSERR_NOERROR) {
      int i;
      for (i = 0; i < NUM_WBUFS && mmr == MMSYSERR_NOERROR; i++) {
        this->whdr[i].lpData = reinterpret_cast<LPSTR>(this->waveBuf) + i * 0x1000;
        this->whdr[i].dwBufferLength = 0x1000;
        this->whdr[i].dwUser = i;
        mmr = ::waveOutPrepareHeader(this->hwo, &this->whdr[i], sizeof(WAVEHDR));
      }
      if (mmr != MMSYSERR_NOERROR) {
        for (; --i >= 0; ) {
          ::waveOutUnprepareHeader(this->hwo, &this->whdr[i], sizeof(WAVEHDR));
        }
      }
    }
    if (mmr == MMSYSERR_NOERROR) {
      for (int i = 0; i < NUM_WBUFS; i++) {
        ::waveOutWrite(hwo, &this->whdr[i], sizeof(WAVEHDR));
      }
    }
    if (mmr == MMSYSERR_NOERROR) {
      ::timeBeginPeriod(1);
    }
    return mmr == MMSYSERR_NOERROR;
  }
  void Close() {
    if (this->hwo != nullptr) {
      this->stopped = true;
      ::waveOutReset(this->hwo);
      for (int i = 0; i < NUM_WBUFS; i++) {
        ::waveOutUnprepareHeader(this->hwo, &this->whdr[i], sizeof(WAVEHDR));
      }
      ::waveOutClose(hwo);
      ::timeEndPeriod(1);
      this->hwo = nullptr;
    }
//    if (this->hthread != nullptr) {
//      ::WaitForSingleObject(this->hthread, 5000);
//      ::CloseHandle(this->hthread);
//      this->hthread = nullptr;
//    }
    lsgClose();
    lsgExit();
  }
  void WaveOutDone(WAVEHDR* wHdr) {
    if (this->stopped == false) {
      DWORD i = wHdr->dwUser;
      lsgBuffer();
      waveOutPrepareHeader(hwo, &this->whdr[i], sizeof(WAVEHDR));
      waveOutWrite(hwo, &this->whdr[i], sizeof(WAVEHDR));
    }
  }
  static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT message, DWORD instance, DWORD param1, DWORD param2) {
    if (message == WOM_DONE) {
      LsgDevice* self = reinterpret_cast<LsgDevice*>(instance);
      return self->WaveOutDone(reinterpret_cast<WAVEHDR*>(param1));
    }
  }

  static uint32_t VLSG_CALLTYPE lsgGetTime()
  {
    return ::timeGetTime();
  }

  bool lsgInit()
  {
    TCHAR fileName[MAX_PATH];
    ::GetModuleFileName(nullptr, fileName, _countof(fileName));
    ::PathRemoveFileSpec(fileName);
    ::PathAppend(fileName, TEXT("romsxgm.bin"));
    uint64_t d;
    return (this->romData = OpenFileAsMapping(fileName, 0xfffffff, &d)) != nullptr;
  }

  void lsgExit()
  {
    CloseFileMapping(this->romData);
  }

  bool lsgWrite(const void* data, int size)
  {
    const DWORD time = VLSG_GetTime();
    const BYTE* p = reinterpret_cast<const BYTE*>(data);
    for (; size > 0; size--, p++) {
      VLSG_Write(&time, 4);
      VLSG_Write(p, 1);
    }
    return true;
  }

  bool lsgSetFrequency(lsgFreq freq)
  {
    VLSG_SetParameter(PARAMETER_Frequency, freq);
    return true;
  }

  bool lsgSetPoly(lsgPoly poly)
  {
    const BYTE data[] = { 0xF0, 0x44, 0x0E, 0x03, poly, 0xF7 };
    lsgWrite(data, sizeof(data));
    VLSG_SetParameter(PARAMETER_Polyphony, poly);
    return true;
  }

  bool lsgSetEffect(lsgFx effe)
  {
    const BYTE data[] = { 0xF0, 0x44, 0x0E, 0x03, effe, 0xF7 };
    lsgWrite(data, sizeof(data));
    VLSG_SetParameter(PARAMETER_Effect, effe);
    return true;
  }

  bool lsgOpen()
  {
    VLSG_SetFunc_GetTime(lsgGetTime);
    lsgSetFrequency(lsgFreq44KHz);
    lsgSetPoly(lsgPoly64);
    lsgSetEffect(lsgFxReverb2);
    // lsgSetEffect(lsgFxOff);
    VLSG_SetRomAddress(this->romData);
    VLSG_Init();
    this->waveBuf = ::calloc(65536, 1);
    VLSG_SetWaveBuffer(this->waveBuf);
    this->counter = 0;
    return true;
  }

  bool lsgClose()
  {
    DWORD d = 0x007bb0;
    for (int i = 0; i < 16; i++) {
      d = (d & 0xfff0) | i;
      lsgWrite(&d, 3);
    }
    ::Sleep(500);
    VLSG_Exit();
    ::free(this->waveBuf);
    this->waveBuf = nullptr;
    return true;
  }

  bool lsgBuffer()
  {
    VLSG_Buffer(this->counter);
    this->counter++;
    return true;
  }
};

struct WinMMDevice : public IMidiDevice {
  HMIDIOUT hmo;
  WinMMDevice() : hmo() { }
  ~WinMMDevice() { this->Close(); }
  bool Init() {
    MMRESULT mmr = ::midiOutOpen(&this->hmo, MIDI_MAPPER, 0, 0, CALLBACK_NULL);
    if (mmr == MMSYSERR_NOERROR) {
      ::timeBeginPeriod(1);
    }
    return mmr == MMSYSERR_NOERROR;
  }
  void Close() {
    if (this->hmo != nullptr) {
      ::midiOutClose(this->hmo);
      ::timeEndPeriod(1);
      this->hmo = nullptr;
    }
  }
  bool Send(Dword shortMsg) {
    MMRESULT mmr = ::midiOutShortMsg(hmo, shortMsg);
    return mmr == MMSYSERR_NOERROR;
  }
  void Release() {
    delete this;
  }
  bool Send(PCByte data, Dword size) {
    if (size == 1) {
      return true;
    }
    if (size <= 3 && (data[0] & 0x80) < 0xF0) {
      const Dword shortMsg = MAKE_MIDIEVENT(data[0], data[1], size == 2 ? 0 : data[2]);
      return this->Send(shortMsg);
    }
    MIDIHDR mHdr = {};
    mHdr.lpData           = reinterpret_cast<LPSTR>(const_cast<PByte>(data));
    mHdr.dwBufferLength   = size;
    mHdr.dwBytesRecorded  = size;
    MMRESULT mmr = ::midiOutPrepareHeader(hmo, &mHdr, sizeof(mHdr));
    if (mmr == MMSYSERR_NOERROR) {
      mmr = ::midiOutLongMsg(hmo, &mHdr, sizeof(mHdr));
      ::midiOutUnprepareHeader(hmo, &mHdr, sizeof(mHdr));
    }
    return mmr == MMSYSERR_NOERROR;
  }
};

IMidiDevice* CreateMidiDevice() {
  // WinMMDevice* dev = new WinMMDevice();
  LsgDevice* dev = new LsgDevice();
  if (dev->Init()) {
    return dev;
  }
  delete dev;
  return nullptr;
}
