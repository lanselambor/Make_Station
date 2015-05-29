#include "DigitalTones.h"

//Output pin
DigitalTones::DigitalTones(int pin)
{
	_pin = pin;
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
}
//play single tone
void DigitalTones::toneManual(int frequency, int duration)
{
	unsigned long period = 1000000/frequency;
	unsigned long length;
	boolean state = false;
	for (length = 0; length < (long) duration * 1000; length += period) 
	{
		state = !state;
		digitalWrite(_pin, state);

		delayMicroseconds(period - 38);
	}
}

//play melody
void DigitalTones::playMelody(int *melody, int *noteDurations, int notes)
{
	int i;
	for (i = 0; i < notes; i++) 
	{
		toneManual(melody[i], noteDurations[i]);
		delay(noteDurations[i] * 6/10);
	}
}


/*----------- end of file --------------*/
