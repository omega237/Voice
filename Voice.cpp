//-----------------------------------------------------------------------------
// VOICE.H
// Copyright (c) 2004 by omega237
//-----------------------------------------------------------------------------

#include "Voice.h"

Voice::Voice()
{
  mMaxFrequency = 0;
  mPhase = 0;
  mFrequency = 0;
  mIncrement = 0.0;
  mStartIndex = 0.0;
  for(int i=0; i<3; i++)
    mAdsr[i][0] = 0.0;
  mSineWavetable = NULL;
  mSquareWavetable = NULL;
  mTriangleWavetable = NULL;
  mExtraWavetable = NULL;
  Waveform = wf_Null;
  SampleRate = sr_Null;
  Interpolation = ip_None;
  WavetableSize = 1024;
};

Voice::~Voice()
{
  if(mSineWavetable != NULL)
    delete mSineWavetable;
  if(mSquareWavetable != NULL)
    delete mSquareWavetable;
  if(mTriangleWavetable != NULL)
    delete mTriangleWavetable;
  if(mExtraWavetable != NULL)
    delete mExtraWavetable;
};

bool Voice::InitWavetables()
{
  bool res;

  res = FillSineWavetable();
  res &= FillSquareWavetable();
  res &= FillTriangleWavetable();
  return res;
};

bool Voice::FillSineWavetable()
{
  ld val = 0;

  if(mSineWavetable != NULL)
    return true;

  mSineWavetable = new ld[WavetableSize];
  if(mSineWavetable == NULL)
    return false;

  val = (PI2/WavetableSize);
  for(int i=0; i<WavetableSize; i++)
    mSineWavetable[i] = sin(i*val);
  return true;
};

bool Voice::FillSquareWavetable()
{
  int i=0;
  int WtsHalf = (int)(WavetableSize/2);

  if(mSquareWavetable != NULL)
    return true;

  mSquareWavetable = new ld[WavetableSize];
  if(mSquareWavetable == NULL)
    return false;

  for(i; i<WtsHalf; i++)
    mSquareWavetable[i] = 1;
  for(i=WtsHalf; i<WavetableSize; i++)
    mSquareWavetable[i] = -1;
  return true;
};

bool Voice::FillTriangleWavetable()
{
  int idx=0, i=0;
  int WtsQuart = (int)(WavetableSize/4);
  ld WtsQuartInv = (ld)(1/(ld)WtsQuart) ;

  if(mTriangleWavetable != NULL)
    return true;

  mTriangleWavetable = new ld[WavetableSize];
  if(mTriangleWavetable == NULL)
    return false;

  for(idx; idx<WtsQuart; idx++)
    mTriangleWavetable[idx] = (WtsQuartInv*idx);
  for(i; i<WtsQuart*2; i++, idx++)
    mTriangleWavetable[idx] = 1-(WtsQuartInv*i);
  for(i=0; i<WtsQuart; i++, idx++)
    mTriangleWavetable[idx] = -1+(WtsQuartInv*i);
  return true;
};

bool Voice::SetFrequency(int Frequency)
{
  if(SampleRate == sr_Null)
    return false;

  if((Frequency >= mMaxFrequency) || (!Frequency))
    return false;

  mFrequency = Frequency;
  CalcIncrement();
  return true;
};

int Voice::GetFrequency()
{
  return mFrequency;
};

bool Voice::SetPhase(int Phase)
{
  if(Phase > 359)
  {
    mPhase = Phase%360;
    return true;
  }
  mPhase = Phase;
  CalcStartIndex();
  return true;
};

int Voice::GetPhase()
{
  return mPhase;
};

bool Voice::SetSampleRate(SampleRates_t SRate)
{
  SampleRate = SRate;
  mMaxFrequency = (int)(SampleRate/2);
  return true;
};

int Voice::GetSampleRate()
{
  return SampleRate;
};

void Voice::CalcIncrement()
{
  mIncrement = ((WavetableSize*mFrequency) / (ld)SampleRate);
};

void Voice::CalcStartIndex()
{
  ld PhaseInRad = mPhase*(PI/180);
  mStartIndex = PhaseInRad*(WavetableSize/PI2);
};

bool Voice::SetWaveform(Waveforms_t Wf)
{
  if(Wf == wf_Null)
    return false;

  Waveform = Wf;
  return true;
};

int Voice::GetWaveform()
{
  return Waveform;
};

ld Voice::GetNextSample()
{
  if((Waveform == wf_Null)|| (SampleRate == sr_Null)|| (!mFrequency))
    return -10; // sonst unmöglicher Wert

  switch(Interpolation)
  {
  case ip_None:
    return GetNextSampleNone();

  case ip_Linear:
    return GetNextSampleLin();

  case ip_Virtual:
    return GetNextSampleVirt();

  case ip_Extreme:
    return GetNextSampleExtreme();
  }
  return -10; // irgendwas ist schiefgelaufen
}

void Voice::CheckIndexBounds()
{
  if(mStartIndex >= WavetableSize)
    mStartIndex = mStartIndex-WavetableSize;
  return;
};

int Voice::GetNextPossibleIndex()
{
  int intpart = 0, idxTrunc = 0;

  CheckIndexBounds();
  intpart = (int)mStartIndex;
  if((mStartIndex-intpart)<0.5)
    idxTrunc = intpart;
  idxTrunc = intpart+1;
  mStartIndex += mIncrement;
  return idxTrunc;
};

ld Voice::GetNextSampleNone() // langsamer als lin??? wenn ja dann nur trunc
{
  int idx = GetNextPossibleIndex();
  ld *wt = NULL;

  switch(Waveform)
  {
  case wf_Sine:
    wt = mSineWavetable; break;
  case wf_Square:
    wt = mSquareWavetable; break;
  case wf_Triangle:
    wt = mTriangleWavetable; break;
  }
  return wt[idx];
};

ld Voice::GetNextSampleLin()
{
  int idxLeft = 0, idxRight = 0;
  ld slope = 0.0, factor = 0.0, retval = 0.0;
  ld *wt = NULL;

  if(Waveform == wf_Square)
    return GetNextSampleNone();

  if(Waveform == wf_Sine)
    wt = mSineWavetable;
  else wt = mTriangleWavetable;
  CheckIndexBounds();
  idxLeft = (int)mStartIndex;
  idxRight = idxLeft+1;
  factor = mStartIndex-idxLeft;
  if(idxRight >= WavetableSize)
    idxRight=0;
  slope = wt[idxLeft]-wt[idxRight];
  slope = -slope;
  retval = wt[idxLeft]+(factor*slope);
  mStartIndex += mIncrement;
  return retval;
};

ld Voice::GetNextSampleVirt()
{
  //geht nur bei sin
  if(Waveform != wf_Sine)
    return GetNextSampleNone();
  CheckIndexBounds();
  ld divisor = WavetableSize/PI2;
  ld ridx = mStartIndex/divisor;
  ld retval = sin(ridx);
  mStartIndex += mIncrement;
  return retval;
};

ld Voice::GetNextSampleExtreme()
{
  return -10; // noch nicht implementiert
};

int Voice::GetInterpolation()
{
  return Interpolation;
};

bool Voice::SetInterpolation(Interpolations_t ip)
{
  Interpolation = ip;
  return true;
};