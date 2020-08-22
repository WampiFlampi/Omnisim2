#include <iostream>
#include "olcNoiseMaker.h"

using namespace std;

#define SINE 0
#define OSC_SQUARE 1
#define OSC_SAW_ANA 2

//nonsense, disregard
atomic<double> dFreqOut = 0.0;
atomic<double> dBlack = 0.0;


bool maj = true;
bool min = false;
bool seven = false;


//convert to hertz from angular momentum 
double w(double dHertz)
{
	return dHertz * 2.0 * PI;
}




double ChangePitch(double base, double note) {
	return base * pow(2, (note / 12));
}



struct sEnvelope
{
	double dAttack;
	double dDecay;
	double dRelease;
	bool bNoteOn;
	double dPeak;
	double dSustain;
	double dTimeOn;
	double dTimeOff;

	
	sEnvelope() {
		dAttack = 0.07;
		dDecay = 0.01;
		dPeak = 1;
		dSustain = 0.8;
		dRelease = 0.4;
		dTimeOn = 0.0;
		dTimeOff = 0.0;
		bNoteOn = false;

	}

	void noteOn(double dOn) 
	{
		bNoteOn = true;
		dTimeOn = dOn;

	}
	
	void noteOff(double dOff)
	{
		bNoteOn = false;
		dTimeOff = dOff;
	}
	
	double GetAmp(double dTime)
	{
		double dAmp = 0;
		double dLife = dTime - dTimeOn;

		if (bNoteOn)
		{
			
			if (dLife <= dAttack)
			{
				dAmp = (dLife / dAttack) * dPeak;
			}
			
			if (dLife > dAttack && dLife <= (dAttack + dDecay))
			{
				dAmp = ((dLife - dAttack) / dDecay) * (dSustain - dPeak) + dPeak;
			}

			if (dLife > (dAttack + dDecay))
			{
				dAmp = dSustain;
			}
			
			
		}
		else
		{
			dAmp = ((dTime - dTimeOff) / dRelease) * (0.0 - dSustain) + dSustain;
			
		}
		
		if (dAmp > 1.0 || dAmp < 0.001) {
			dAmp = 0;
		}

		
		return dAmp;
	}


};


sEnvelope envelope;

double osc(double dHertz, double dTime, int nType = SINE)
{
	double dConst = 0.03;
	double dMod = w(dHertz) * dTime + 0.002 * dHertz * (sin(w(3) * dTime));

	switch (nType)
	{
	case SINE: // Sine wave bewteen -1 and +1
		return (dConst) * sin(dMod);

	case OSC_SQUARE: // Square wave between -1 and +1
		return (dConst)*sin(w(dHertz) * dTime) > 0 ? 1.0 : -1.0;

	case OSC_SAW_ANA: // Saw wave (analogue / warm / slow)
	{
		double dOutput = 0.0;

		for (double n = 1.0; n < 40.0; n++)
			dOutput += (sin(n * dMod)) / n;

		return dOutput * (2.0 / PI) * dConst;
	}


	default:
		return 0.0;
	}
}

double Chord(double dTime, int nType) {
	int notes = 0;
	double wave = 0.0;
	vector<int> map;
	double pitch = dFreqOut;
	if (maj) {
		notes = 3;
		map = { 0,4,7 };
	}
	if (min) {
		notes = 3;
		map = { 0,3,7 };
	}
	if (seven) {
		notes = 4;
		map = { 0,4,7,10 };

	}

	for (int i = 0; i < notes; i++) {
		wave += osc(ChangePitch(pitch, map[i]), dTime, nType) * envelope.GetAmp(dTime);
	}
	dBlack = wave;

	return dBlack;
}


double MakeNoise(double dTime)  
{
	
	return Chord(dTime, 2);

}

int main()
{
	//I didn't write this, interfacing with the sound card is hard lmao
    vector<wstring> devices = olcNoiseMaker<short>::Enumerate();
	
	olcNoiseMaker<short> sound(devices[0], 44100, 1, 16, 256);
    
	sound.SetUserFunction(MakeNoise);

	wcout << endl <<

		"|        |  |        |  |        |" << endl <<
		"|        |  |        |  |        |" << endl <<
		"|    j   |  |     K  |  |     L  |" << endl <<
		"|________|  |________|  |________|" << endl <<
		"                                  " << endl <<
		"                                  " << endl <<
		"                                  " << endl <<
		"                                  " << endl <<
		"|        |  |        |  |        |" << endl <<
		"|        |  |        |  |        |" << endl <<
		"|    q   |  |      w |  |     e  |" << endl <<
		"|________|  |________|  |________|" << endl <<
		"                                  " << endl <<
		"|        |  |        |  |        |" << endl <<
		"|        |  |        |  |        |" << endl <<
		"|    a   |  |      s |  |     d  |" << endl <<
		"|________|  |________|  |________|" << endl <<
		"                                  " << endl <<
		"So like amplitude scaling based on frequency," << endl <<
		"we don't take kindly to such devil worship round these parts" << endl << endl;
	
	//c++ isn't like the other girls, it has vectors
	double base = 148.02;
	vector<double> basis = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	vector<int> notes = { 6,1,8,3,10,5 };
	
	for (int i = 0; i < 6; i++) {
		
		basis[i] = ChangePitch(base, notes[i]);

		
	}
	

	bool dKeyPress;
	bool bCheck = false;
	bool bCheck2 = false;

    while (1)
    {	
		dKeyPress = false;
		for (int i = 0; i < 6; i++)
		{
			
			if (GetAsyncKeyState((unsigned char)("QWEASD"[i])) & 0x8000) 
			{

				dKeyPress = true;
				
				if (!bCheck2) 
				{
					dFreqOut = basis[i];
					envelope.noteOn(sound.GetTime());
					bCheck2 = true;
				}

				if (dKeyPress) {
					bCheck = true;
					break;
				}
			}
			
			
		}
		if (!dKeyPress) {
			
			bCheck2 = false;

			if (bCheck) 
		{
			envelope.noteOff(sound.GetTime());
			bCheck = false;
		}

		}
		
		//this function sucks, again its windows specific, so I'm not going to bother
		// yandere dev time
		
		


		//toggle that juicy chord flavor, might add some cronchy ones later

		if (GetAsyncKeyState('J') & 0x8000) {
			min = false;
			seven = false;
			maj = true;
		}

		if (GetAsyncKeyState('K') & 0x8000) {
			seven = false;
			maj = false;
			min = true;
		}

		if (GetAsyncKeyState('L') & 0x8000) {
			maj = false;
			min = false;
			seven = true;
		}



		
        
    }
}
