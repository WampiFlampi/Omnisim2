#include <iostream>
#include "olcNoiseMaker.h"

using namespace std;

#define SINE 0
#define SQUARE 1
#define OSC_TRIANGLE 2
#define OSC_SAW_ANA 3

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


struct sEnvelope
{
	double dAttack;
	double dDecay;
	double dRelease;

	double dPeak;
	double dSustain;
	
	sEnvelope() {
		dAttack = 0.05;
		dDecay = 0.01;
		dPeak = 0.04;
		dSustain = 0.03;
		dRelease = 0.10;
	}
	double GetAmp(double dTime)
	{
		double dAmp = 0.0;
		return 0;
	}


};
//wave generation function
double osc(double dHertz, double dTime, int nType = SINE)
{
	switch (nType)
	{
	case SINE: // Sine wave bewteen -1 and +1
		return .05*sin(w(dHertz) * dTime);

	case OSC_TRIANGLE: // Triangle wave between -1 and +1
		return asin(0.05*sin(w(dHertz) * dTime)) * (2.0 / PI);

	case OSC_SAW_ANA: // Saw wave (analogue / warm / slow)
	{
		double dOutput = 0.0;

		for (double n = 1.0; n < 40.0; n++)
			dOutput += (sin(n * w(dHertz) * dTime)) / n;

		return dOutput * (2.0 / PI)*.1;
	}


	default:
		return 0.0;
	}
}




double ChangePitch(double base, double note) {
	return base * pow(2, (note / 12));
}

double Chord(double Time, int nType) {
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
		wave += osc(ChangePitch(pitch, map[i]), Time, nType);
	}
	dBlack = wave;

	return dBlack;
}

double MakeNoise(double dTime)  
{
	
	return Chord(dTime, OSC_TRIANGLE);

}

int main()
{
	//I didn't write this, interfacing with the sound card is hard lmao
    vector<wstring> devices = olcNoiseMaker<short>::Enumerate();
	olcNoiseMaker<short> sound(devices[0], 44100, 1, 16, 256);
    sound.SetUserFunction(MakeNoise);

	//If you can figure out how to animate this be my guest
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
	double base = 130;
	vector<double> basis = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };
	
	for (int i = 0; i < 6; i++) {
		basis[i] = base;
		base = ChangePitch(base, 7);

		
	}
	


    while (1)
    {
        //this function sucks, again its windows specific, so I'm not going to bother
		// yandere dev time
		if (GetAsyncKeyState('Q') & 0x8000) {
			dFreqOut = basis[0];
		}
		else if (GetAsyncKeyState('W') & 0x8000) {
			dFreqOut = basis[1];
		}
		else if (GetAsyncKeyState('E') & 0x8000) {
			dFreqOut = basis[2];
			wcout << GetAsyncKeyState('E') << endl << endl;
		}
		else if (GetAsyncKeyState('A') & 0x8000) {
			dFreqOut = basis[3];

		}
		else if (GetAsyncKeyState('S') & 0x8000) {
			dFreqOut = basis[4];
		}
		else if (GetAsyncKeyState('D') & 0x8000) {
			dFreqOut = basis[5];
		}
        else {
            dFreqOut = 0.0;
        }
		


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
