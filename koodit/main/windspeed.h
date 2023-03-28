

#include <EEPROM.h>

#define TIMSK1  TIMSK
// *** Customizable Parameters ***
int period = 0.1;                // (300 seconds = 5 minutes)
int fanSampleRate = 1; // How many milliseconds between taking windspeed readings (Needs to be small enough to capture the peak of the analog waveform)
int fanPin = PA0;
const int ledPin = PB1;
// *** Globals ***
int highest = 0;     // Keeps track of the highest windspeed over a period 
int seconds = 0;     // Keeps track of how many seconds have passed within a period
int promAddr = 0;    // Current index in EEPROM memory

pinMode(ledPin, OUTPUT);
digitalWrite(ledPin, LOW);
timer1_init(); //????????

/********************************************************
 * timer1_init()
 *
 * Sets up the registers to use TIMER 1 which will be
 * used to call an interrupt handler once every second
 * (Assumes Arduino Uno is based off ATMEGA328)
 ********************************************************/
void timer1_init() {
  // Used the Arduino Timer Calculator at http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html to generate this code
  
  // TIMER 1 for interrupt frequency 1 Hz:
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for 1 Hz increments
  OCR1A = 62499; // = 16000000 / (256 * 1) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 256 prescaler
  TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts 
}


/********************************************************
 * Interrupt Service Routine (Timer 1)
 *
 * This is called by Timer 1 once a second.
 ********************************************************/
ISR(TIMER1_COMPA_vect) {         // timer compare interrupt service routine

  // This section increments the seconds counter each time the interrupt handler is called.
  // Once the number of seconds reaches the period length, we run this block of code and reset.
  seconds++;
  if (seconds >= period) {
    seconds = 0;
    //Serial.print("m/s: ");
    //Serial.println(highest * 0.109 * 0.44);
    
    EEPROM.write(promAddr++, highest); // Write the highest windspeed of the period to EEPROM
 
    highest = 0;
  }
}



float windspeed_measurement() 
{
  int newReading = analogRead(fanPin);
  
    if (newReading > highest)
    {
        highest = newReading;
    }

    delay(fanSampleRate); // Time in between taking readings from anemometer (fan)
    
    return highest;
}
