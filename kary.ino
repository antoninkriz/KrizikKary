#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

#define reed A1
int reedVal;
int maxDebounceTicks = 100;
int currentDebounceTicks;
int time = 0;
float radius = 15.00000/100000.00000;
float circumference;
float velocity = 0.00;
float distance = 0.00;

int analogInput = 0;
float vout = 0.0;
float vin = 0.0;
float R1 = 100000.0;
float R2 = 10000.0;
int value = 0;

void setup()
{
  lcd.begin(16, 2);
  
  currentDebounceTicks = maxDebounceTicks;

  circumference = 2 * 3.14 * radius;

  pinMode(reed, INPUT);

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 1999;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11);
  TIMSK1 |= (1 << OCIE1A);

  sei();
}

void loop() {
  displayText();
  printvolt();
 
  delay(500);
}

ISR(TIMER1_COMPA_vect) {
  reedVal = digitalRead(reed);
  if (reedVal) {
    if (currentDebounceTicks == 0) {
      velocity = circumference/(float(time)/1000/3600);
      time = 0;
      currentDebounceTicks = maxDebounceTicks;
      distance = distance + circumference;

    } else {
      if (currentDebounceTicks > 0) {
        currentDebounceTicks -= 1;
      }
    }
  } else {
    if (currentDebounceTicks > 0) {
      currentDebounceTicks -= 1;
    }
  }

  if (time > 2000) {
    velocity = 0;
  } else {
    time += 1;
  }
}

void displayText(){
  lcd.clear();
  lcd.setCursor(0, 0);
  
  lcd.print(int(velocity));
  lcd.print(" km/h");

  int len = 2;
  int dis = 0;
  String unit = " m";
  if (distance > 1) {
    dis = distance;
    unit = " km";
    len = 3;
    printtext(dis, unit, len);
  } else {
    dis = int(distance * 1000);
    unit = " m";
    len = 2;
    printtext(dis, unit, len);
  }
}

void printtext(int dis, String unit, int len) {
  int nDigits = floor(log10(abs(dis)));
  
  lcd.setCursor(15 - nDigits - len, 0);
  lcd.print(dis);
  lcd.print(unit);
}

void printvolt() {
  value = analogRead(analogInput);
  vout = ((value * 5.0) / 1024.0) * 1.0116;
  vin = vout / (R2/(R1+R2)); 
  if (vin<0.09) {
    vin=0.0;
  }
  lcd.setCursor(0, 1);
  lcd.print(vin);
  lcd.print(" V");
}
