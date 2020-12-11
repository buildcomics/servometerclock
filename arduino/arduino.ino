/* Build Comics
    Connect DS3231 SCL to A5 on Arduino Nano
    Connect DS3231 SDA to A4 on Arduino Nano

    Connect Button 1 to A1 on Arduino Nano
    Connect Button 2 to A0 on Arduino Nano

    Connect button for daylight saving to D13

  Display pinout LTC4946m:
  1  2  3   4  5   6  7   8   9  10  11  12  13  14  15
  G  C  P1  B  P2  A  P3  F   E   D  CAx CA1 CA2 CA3 CA4
  P1     P2     P3        DP1 DP2

  P1, P2, P3 triangle red LEDS with Uf=1,7V
  dp1, dp2 and green LEDS with Uf = 1,9V

  CAx = Common Anode xtra (red P1,P2,P3 points dp1,dp2)
  CA1 = Common Anode Digit1
  CA2 = Common Anode Digit2
  CA3 = Common Anode Digit3
  CA4 = Common Anode Digit4

             P1              P2        P3

  |--A--|   |--A--|        |--A--|   |--A--|
  F     B   F     B  dp1   F     B   F     B
  |     |   |     |        |     |   |     |
  --G--     --G--          --G--     --G--
  |     |   |     |        |     |   |     |
  E     C   E     C  dp2   E     C   E     C
  |--D--|   |--D--|        |--D--|   |--D--|

  Digit1   Digit2         Digit3    Digit4

 *  */

#include <DS3232RTC.h>  //https://github.com/JChristensen/DS3232RTC
#include <Servo.h> //https://www.arduino.cc/reference/en/libraries/servo/write/

#define BUTTON1 A1 //for button 1
#define BUTTON2 A2 //for button 2
#define DAYLIGHT_SAVING 13 //for daylight saving switch, when using pin 13 connect other side switch to V+ due to onboard LED

#define SERVO_HOUR 10 //Pin for servo of hour meter
#define SERVO_MINUTE 9 //Pin for servo of Minute Meter

//Calibration values for servo's:
#define HOUR_ZERO 1060
#define HOUR_TWELVE 2128

#define MINUTE_ZERO 1122
#define MINUTE_SIXTY 2140

//Cathode Display Segment Pin Definitions:
#define CATHODE_SEGMENT_A 5
#define CATHODE_SEGMENT_B 6
#define CATHODE_SEGMENT_C 7
#define CATHODE_SEGMENT_D 2
#define CATHODE_SEGMENT_E 3
#define CATHODE_SEGMENT_F 4
#define CATHODE_SEGMENT_G 8

//Character Display Digit Common Anode Pin Definitions:
#define ANODE_DIGIT_0 11
#define ANODE_DIGIT_1 12
#define ANODE_DIGIT_2 A0
#define ANODE_DIGIT_3 A3
#define NR_DIGITS 4

#define RUNNING 1 //value for normal running mode
#define SETTING 2 //value for setting time

#define ELEMENT_YEAR 0
#define ELEMENT_MONTH 1
#define ELEMENT_DAY 2
#define ELEMENT_HOUR 3
#define ELEMENT_MINUTE 4
#define ELEMENT_SECOND 5

unsigned int state = RUNNING ; //Variable for containing state, starting with running
unsigned int timeout = 0; //Counting for timeout

Servo s_minute,s_hour;

time_t t; //time construct containing the time
tmElements_t tm; //Time elements for setting the time
unsigned long int previousMillis; //Holds previous millis for updating time
unsigned int interval = 1000; //This is our interval for updating the screen
unsigned int val = 0; //Value for setting date/time
byte element = ELEMENT_YEAR; //element number to store (second,minute,hour,day,month,year)
bool daylightSaving = false; //Storing if daylight saving time is enabled (e.g. +1 hour)
byte maxVals[] = {99, 12, 31, 23, 59, 59}; //Maximum values for the ELEMENT definitions

//Match cathode pins to arduino pins
byte sc[] = {
  CATHODE_SEGMENT_A,
  CATHODE_SEGMENT_B,
  CATHODE_SEGMENT_C,
  CATHODE_SEGMENT_D,
  CATHODE_SEGMENT_E,
  CATHODE_SEGMENT_F,
  CATHODE_SEGMENT_G
};

//Match anode pins to arduino pins
byte da[] = {
  ANODE_DIGIT_0,
  ANODE_DIGIT_1,
  ANODE_DIGIT_2,
  ANODE_DIGIT_3
};

//Segments to number mapping
byte num_to_seg[] = {
  0b0111111,  //  0
  0b0000110,  //  1
  0b1011011,  //  2
  0b1001111,  //  3
  0b1100110,  //  4
  0b1101101,  //  5
  0b1111101,  //  6
  0b0000111,  //  7
  0b1111111,  //  8
  0b1101111,  //  9
};

void setup()
{
  Serial.begin(115200);
  setSyncProvider(RTC.get);   // the function to get the time from the RTC

  //Attach servo objects to correct pins
  s_hour.attach(SERVO_HOUR);
  s_minute.attach(SERVO_MINUTE);

  //Set to 90 degrees:
  s_hour.writeMicroseconds(HOUR_ZERO); //1070 is zero, 2120 = 12
  delay(200); //give first servo some time to move...
  s_minute.writeMicroseconds(MINUTE_ZERO); //1120 is zero, 2140 = 60

  if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  }
  else {
    Serial.println("RTC has set the system time");
  }

  for (int i = 0; i < 8; i++) { //set cathode pins to output
    pinMode(sc[i], OUTPUT);
    digitalWrite(sc[i], HIGH);
  }

  for (int i = 0; i < 4; i++) { //set common anode pins to output
    pinMode(da[i], OUTPUT);
    digitalWrite(da[i], LOW);
  }
  pinMode(BUTTON1, INPUT_PULLUP); //Enable pullup for button
  pinMode(BUTTON2, INPUT_PULLUP); //Enable pullup for button
  pinMode(DAYLIGHT_SAVING, INPUT_PULLUP); //Enable pullup for daylight saving switch
}

void loop() {
  if (state == RUNNING) {
    dispClr();
    interval = 1000; //1000 millisecond interval for normal operation
  }
  else if (state == SETTING) {
    interval = 150; //Shorter interval for checking buttons
    dispVal(val);
    dispNum(0, element);
  }
  else {
    Serial.println("Unknown state");
    state = RUNNING; //should never come here, but anyway.
  }
    unsigned long currentMillis = millis();
    unsigned long timeDif = currentMillis - previousMillis;
  if (timeDif >= interval) { //Next interval
    previousMillis = currentMillis; //reset counter
    if (state == RUNNING) {
      Serial.println("Running...");
      printDateTime();
      if (digitalRead(DAYLIGHT_SAVING) == LOW) { //DAylight saving enabled, add one hour
          daylightSaving = true;
      }
      else {
        daylightSaving = false;
      }
      Serial.print("Daylight Saving:");
      delay(200); //give first servo some time to move...
      Serial.println(daylightSaving);
      //s_minute.writeMicroseconds(map(second(), 0, 59, MINUTE_ZERO, MINUTE_SIXTY)); //for speed testing
      s_minute.writeMicroseconds(map(minute(), 0, 59, MINUTE_ZERO, MINUTE_SIXTY));
      delay(200); //give first servo some time to move...
      //s_hour.writeMicroseconds(map(minute(), 0, 59, HOUR_ZERO, HOUR_TWELVE)); //for speed testing
      byte hr = hour(); //24h system to 12 Hour
      if (daylightSaving) { //Add one for daylight saving time
        hr++;
      }
      hr = hr % 12; //24h system to 12 Hour (24 will also go to zero as a consequence)
      s_hour.writeMicroseconds(map(hr, 0, 12, HOUR_ZERO, HOUR_TWELVE));

      if (digitalRead(BUTTON1) == LOW) { //Setting button pressed
        delay(500); //wait 500ms to double check
        if (digitalRead(BUTTON1) == LOW) { //Setting button still pressed
          state = SETTING; //Change to setting state
          element = ELEMENT_YEAR; //Set element to 0 = year
          val = year() - 2000; //Only last two digits of year
          while (digitalRead(BUTTON1) == LOW); //Wait untill button is depressed
          delay(100); //debounce wait
        }
      }
    }
    else if (state == SETTING) {
      timeout++;
      if (digitalRead(BUTTON2) == LOW) {
        val = (val + 1) % maxVals[element];
        timeout = 0; //reset timeout
      }
      if (digitalRead(BUTTON1) == LOW) { //Setting button pressed
          //Store current date/time element:
          timeout = 0; //reset timeout
          t = now();
          breakTime(t, tm); //Make time elements from time
          switch (element) { //Set the correct time element
            case ELEMENT_YEAR:
              tm.Year = CalendarYrToTm(2000 + val);
              element = ELEMENT_MONTH; //Next Element to set
              val = month();
              break;
            case ELEMENT_MONTH:
              tm.Month = val;
              element = ELEMENT_DAY;
              val = day();
              break;
            case ELEMENT_DAY:
              tm.Day = val;
              element = ELEMENT_HOUR;
              val = hour();
              break;
            case ELEMENT_HOUR:
              tm.Hour = val;
              element = ELEMENT_MINUTE;
              val = minute();
              break;
            case ELEMENT_MINUTE:
              tm.Minute = val;
              element = ELEMENT_SECOND;
              val = second();
              break;
            case ELEMENT_SECOND:
              tm.Second = val;
              state = RUNNING;
              break;
          }
          Serial.print("Setting next element ");
          Serial.print(element);
          Serial.print(" starting value ");
          Serial.println(val);

          t = makeTime(tm); //make time_t from tm struct
          RTC.set(t); // use the time_t to set the time and date
          setTime(t); //Set it in the arduino clock as well.
          setSyncProvider(RTC.get);   // the function to get the time from the RTC
          while (digitalRead(BUTTON1) == LOW); //Wait untill button is depressed
          delay(100); //debounce wait
      }
      if (timeout > 100) { //after 100 loops of 200ms intervals = 20 seconds
        state = RUNNING; //Run again
      }
    }
    else {
      Serial.println("Unexpected state");
      state = RUNNING; //should never come here, but anyway.
    }
  }
}

//Print date and time
void printDateTime()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(' ');
  Serial.print(day());
  Serial.print(' ');
  Serial.print(month());
  Serial.print(' ');
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits)
{
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(':');
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

//Display number (digit, number)
void dispNum(byte d, byte n) {
  for (int dt = 0; dt < NR_DIGITS; dt++) { //For all digits, turn them on first
    digitalWrite(da[dt], LOW);
  }

  bool l;
  for (int s = 0; s < 7; s++) { //For all segments
    if (num_to_seg[n % 10] & (1 << s)) {
      l = LOW;
    }
    else {
      l = HIGH;
    }
    digitalWrite(sc[s], l);

  }
  for (int dt = 0; dt < NR_DIGITS; dt++) { //For all digits, turn the correct one one
    if (dt == d) {
      digitalWrite(da[dt], HIGH);
    }
    else {
      digitalWrite(da[dt], LOW);
    }
  }
  delay(1);
}

//Display a number between 0 and 99 on the second two digits
void dispVal(byte val) {
    dispNum(2,val/10);
    dispNum(3,val % 10);
}

//Clear display
void dispClr() {
  for (int dt = 0; dt < NR_DIGITS; dt++) { //For all digits, turn them on first
    digitalWrite(da[dt], LOW);
  }
}
