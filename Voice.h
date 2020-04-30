//-----------------------------------------------------------------------------
// VOICE.H
// Copyright (c) 2004 by omega237
//-----------------------------------------------------------------------------

#ifndef __CLASS_VOICE__
#define __CLASS_VOICE__

#include <stdlib.h> // NULL
#include <math.h>

typedef long double ld;

#define PI 3.14159265358979323846
#define PI2 6.28318530717958647692

enum Waveforms_t
{
  wf_Null = 0,
  wf_Sine,
  wf_Square,
  wf_Triangle
};

enum SampleRates_t
{
  sr_Null = 0,
  sr_Min = 11025,
  sr_Mid = 22050,
  sr_Norm = 44100,
  sr_Max = 96000
};

enum Interpolations_t
{
  ip_None = 0,
  ip_Linear,
  ip_Virtual,
  ip_Extreme
};

class Voice
{
private:
  int WavetableSize;
  int mMaxFrequency; 
  int mPhase;
  int mFrequency;
  ld mStartIndex;
  ld mIncrement;
  ld mAdsr[3][1];
  ld *mSineWavetable;
  ld *mSquareWavetable;
  ld *mTriangleWavetable;
  ld *mExtraWavetable;
  Waveforms_t Waveform;
  SampleRates_t SampleRate;
  Interpolations_t Interpolation;
  bool FillSineWavetable();
  bool FillSquareWavetable();
  bool FillTriangleWavetable();
  void CalcIncrement();
  void CalcStartIndex();
  // immer noch auf die Wellenform achten, bisher nur sin
  ld GetNextSampleNone();
  ld GetNextSampleLin();
  ld GetNextSampleVirt();
  ld GetNextSampleExtreme(); // relativ unnötig??
  int GetNextPossibleIndex();
  void CheckIndexBounds();
protected:
public:
  Voice();
  ~Voice();
  bool InitWavetables();
  bool InitExtraWavetable();
  bool SetFrequency(int Frequency);
  int GetFrequency();
  bool SetPhase(int Phase);
  int GetPhase();
  bool SetSampleRate(SampleRates_t SRate);
  int GetSampleRate();
  ld GetNextSample();
  bool SetWaveform(Waveforms_t Wf);
  int GetWaveform();
  int GetInterpolation();
  bool SetInterpolation(Interpolations_t ip);
};

#endif //__CLASS_VOICE__