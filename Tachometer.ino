#include <LiquidCrystal.h>

/*
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */

const int triggerPin = 7;
const int sensorPin = 2;


const long debounceLength = 50000; // µs of deadzone after a PCINT on the button.
const long sensorDebounce = 20000;


volatile long int pulseCount = 0; // It's probably not going to pass by 2^16 times, but still!
unsigned long startMicros = 0;
unsigned long lastTrigger = 0;

long lastUpdate = 0;

bool amCounting = false;
bool hasData = false;

volatile float freq = 0;


LiquidCrystal lcd(12, 11, 6, 5, 4, 3); // We need the PCINT pins available

void sensorPCINT () {
  if (amCounting && micros() > lastTrigger+sensorDebounce) {
    lastTrigger = micros();
    ++pulseCount;
    freq = pulseCount/(float)(micros()-startMicros)*1000000.0;
  }
}

void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  
  lcd.setCursor(0,0);
  lcd.print("............");
  lcd.setCursor(12,0);
  lcd.print("Rpm");
  
  lcd.setCursor(0,1);
  lcd.print("............");
  lcd.setCursor(12,1);
  lcd.print("Hz");

  pinMode(sensorPin,INPUT);
  pinMode(triggerPin,INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(sensorPin), sensorPCINT, FALLING);
}

void loop() {
  if (millis() > lastUpdate+100) {
    lcd.setCursor(0,1);
    lcd.print("............");
    lcd.setCursor(0,1);
    lcd.print(freq);

    lcd.setCursor(0,0);
    lcd.print("............");
    lcd.setCursor(0,0);
    lcd.print(freq*60);

    lastUpdate = millis();
  }

  digitalWrite(13,digitalRead(triggerPin) != digitalRead(sensorPin));
  
  if (amCounting) {
    // If the trigger is released and the debounce has expired...
    if ( digitalRead(triggerPin) && micros() > startMicros + debounceLength) {
      startMicros = micros();
      amCounting = false;

      Serial.println(pulseCount);
      
      //lastTrigger = micros();
    }
  } else {
    // If the trigger is pressed and the debounce has expired...
    if ( !digitalRead(triggerPin) && micros() > startMicros + debounceLength) {
      startMicros = micros();
      amCounting = true;
      
      freq = pulseCount/(micros()-startMicros);
      pulseCount = 0;
      freq = 0;
    }
  }
}




