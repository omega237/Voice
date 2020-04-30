
#include <iostream>
#include <stdio.h>
#include "Voice.h"


signed short waveformData[1323000];


using namespace std;
int main()
{
  Voice v;

  v.InitWavetables();
  v.SetSampleRate(sr_Norm);
  v.SetFrequency(440);
  v.SetWaveform(wf_Sine);
  v.SetInterpolation(ip_Virtual);


  Voice v2;

  v2.InitWavetables();
  v2.SetSampleRate(sr_Norm);
  v2.SetFrequency(880);
  v2.SetWaveform(wf_Sine);
  v2.SetInterpolation(ip_Virtual);


  Voice v3;

  v3.InitWavetables();
  v3.SetSampleRate(sr_Norm);
  v3.SetFrequency(5);
  v3.SetWaveform(wf_Triangle);
  v3.SetInterpolation(ip_Linear);


  Voice v4;

  v4.InitWavetables();
  v4.SetSampleRate(sr_Norm);
  v4.SetFrequency(5);
  v4.SetWaveform(wf_Sine);
  v4.SetInterpolation(ip_Virtual);

  for(int i=0; i<=1323000; i++)
  {
    waveformData[i] = (signed short)((8000*v4.GetNextSample())*(v.GetNextSample()+v2.GetNextSample()+v3.GetNextSample()));
  };

  FILE * pFile;
	pFile = fopen ("test.raw","wb");
	fwrite(&waveformData,sizeof(waveformData[0]),1323000,pFile);
	fclose(pFile);

  return 0;
}