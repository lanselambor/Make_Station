#ifndef DIGITALTONES_H
#define DIGITALTONES_H

#include <Arduino.h>

class DigitalTones
{
	public:
		int tones[8] = {523, 587, 659, 698, 784, 880, 988, 1047};
		
		DigitalTones(int pin);  
		void toneManual(int frequency, int duration);  //play single tone
		void playMelody(int *melody, int *noteDurations, int notes); //play melody
	private:
		int _pin;
		
};

#endif