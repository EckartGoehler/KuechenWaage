// Eine einfache wage mit Wägezelle für HX711 AD, 5kg, und Serieller Ausgabe
// $Id:$


#include "Arduino.h"
#include <Wire.h> 
#include "stdio.h" // snprintf
#include "HX711.h"
#include <EEPROM.h>

//#define DEBUG 1

#ifdef DEBUG
  #define DEBUG_OUTPUT "%d"
#else
 #define DEBUG_OUTPUT " "
#endif 

 
#define EEPROM_SCALE_ADDR  (0 * sizeof(double))
#define EEPROM_SIGMA_ADDR  (1 * sizeof(double))
#define EEPROM_OFFSET_ADDR (2 * sizeof(double))


#define DISPLAYED_WEIGHT_FRACTION 10

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 0;

// the loadcell ADC access class
HX711 scale;


double scaling = 1./(1000./379351.56);
const int tare_average_num = 20;

// last measured weight:
double last_weight = 0;

// running average:
int measurement_averaged_num = 0;

// current averaged number:
double weight_averaged = 0;

const int MAX_AVERAGE = 10;

// sqrt(variance) of single weight measurement [g], estimate a priori
double single_weight_sigma = 0.1;

void setup()
{
  Serial.begin(57600);
  EEPROM.begin(256);

  
  // output of LED to report measurement update
  pinMode(LED_BUILTIN, OUTPUT);

  // read scaling factor from EEPROM, if there is any:
  if (EEPROM.read(EEPROM_SCALE_ADDR) != 0) {
    EEPROM.get(EEPROM_SCALE_ADDR,scaling);
  }

  // read sigma from EEPROM, if there is any:
  if (EEPROM.read(EEPROM_SIGMA_ADDR) != 0) {
    double raw_sigma;
    EEPROM.get(EEPROM_SIGMA_ADDR,raw_sigma);
    single_weight_sigma = raw_sigma / scaling;
  }
  
  Serial.println("");
	Serial.println("Start");
  


  // Initialize library with data output pin, clock input pin and gain factor.
  // Channel selection is made by passing the appropriate gain:
  // - With a gain factor of 64 or 128, channel A is selected
  // - With a gain factor of 32, channel B is selected
  // By omitting the gain factor parameter, the library
  // default "128" (Channel A) is used here.
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Tarieren");


  scale.set_scale(scaling);    // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare(tare_average_num);                // reset the scale to 0
  Serial.println("Completed");
}


bool blink_on = false;

void loop()
{
  if (blink_on) {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else {
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  }
  blink_on = !blink_on;

  
  // fetch new value, averaged:
  double single_weight = scale.get_units(1);


    char out[17];
    snprintf(out,sizeof(out), "%f",single_weight);
    Serial.println(out);

  delay(1000);
}
