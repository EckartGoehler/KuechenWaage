// Eine Küchenwaage, mit Wägezelle für HX711 AD, 5kg, und Display-Ausgabe
// $Id:$


#include "Arduino.h"
#include <Wire.h> 
#include "stdio.h" // snprintf
#include "HX711.h"
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Fonts/FreeSansBold9pt7b.h>
#include <EEPROM.h>

//#define DEBUG 1

#ifdef DEBUG
  #define DEBUG_OUTPUT "%d"
#else
 #define DEBUG_OUTPUT " "
#endif 

// Pin definitions (actually GPIO numbers)
#define TFT_CS         15
#define TFT_RST        -1 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         16
 
#define EEPROM_SCALE_ADDR  (0 * sizeof(double))
#define EEPROM_SIGMA_ADDR  (1 * sizeof(double))
#define EEPROM_OFFSET_ADDR (2 * sizeof(double))


#define DISPLAYED_WEIGHT_FRACTION 10

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 0;
const int BUTTON_TARE_PIN = 5; 

// button variables
unsigned long tare_started_millisec = 0;
static const unsigned long TARE_DEBOUNCE_WINDOW = 500;
const int TARE_NUM = 5; // number of samples for tare



// the loadcell ADC access class
HX711 scale;

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


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
  scale.tare(1);
  Serial.println("Completed");

  tft.initR(INITR_GREENTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setFont(&FreeSansBold9pt7b);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  
  tft.setTextSize(2);
  tft.setFont(); // reset font to default
  tft.setRotation(3);
  
  tft.setCursor(5, 20);
  tft.print("Let's start!");          


  pinMode(BUTTON_TARE_PIN, INPUT_PULLUP);

  scale.tare(tare_average_num);                // reset the scale to 0
  tft.fillScreen(ST77XX_BLACK);
  
}

bool blink_on = false;

void loop()
{
  if (measurement_averaged_num == MAX_AVERAGE) {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else {
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  }

  
  // fetch new value, averaged:
  double single_weight = scale.get_units(1);

  // weight apparently changed
  if (fabs(single_weight - last_weight) > 5 * single_weight_sigma) {
    measurement_averaged_num = 0;
  }

  last_weight = single_weight;

  if (measurement_averaged_num == 0) {
     weight_averaged =single_weight;
     measurement_averaged_num = 1;
  } else if (measurement_averaged_num < MAX_AVERAGE) {
    // increase average:
    weight_averaged = (weight_averaged * measurement_averaged_num + single_weight) / (measurement_averaged_num + 1);
    measurement_averaged_num ++;
  } else {
    // running mean:
    weight_averaged = weight_averaged + (single_weight - weight_averaged)/ MAX_AVERAGE;
  }

  double output_weight = weight_averaged;

  // print out value:
  char out[17];
  bool is_positive;
  if (static_cast<int32_t>(output_weight * DISPLAYED_WEIGHT_FRACTION) >= 0) {
    is_positive = true;
  }
  else {
    is_positive = false;
    output_weight = - output_weight;
  }

  int32_t weight_full = static_cast<int32_t>(output_weight);
  int32_t weight_fraction = static_cast<int32_t>(output_weight * DISPLAYED_WEIGHT_FRACTION) - static_cast<int32_t>(output_weight) * DISPLAYED_WEIGHT_FRACTION;
    
  
  snprintf(out,sizeof(out), "%c%4ld.%01ldg " DEBUG_OUTPUT "          ", 
            is_positive ? ' ' : '-',
            weight_full, 
            weight_fraction
#ifdef DEBUG            
            ,measurement_averaged_num
#endif
            );

  Serial.println(out);
  tft.setCursor(0, 20);          
  tft.print(out);

  // check for tare button press
  bool tare_button_pressed = digitalRead(BUTTON_TARE_PIN) == 0;
  if (tare_button_pressed && millis() - tare_started_millisec > TARE_DEBOUNCE_WINDOW) {
    tare_started_millisec = millis();
    scale.tare(TARE_NUM);
    measurement_averaged_num = 0;
  } 


}
