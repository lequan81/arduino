/*
  ╔------------------------------------------------------------╗
  ║                 ESP8266 Clock with Lunar                   ║
  ╠------------------------------------------------------------╣
  ║ A simple project base on ESP8266, also using DS3231RTC and ║
  ║NTP server (Internet mode) with lunar (Vietnamese version). ║
  ╠------------------------------------------------------------╣
  ║ Filename: ESP_clock_lunar.ino                              ║
  ║ Last modified: 28/3/2021 by lequanruby@gmail.com           ║
  ╚------------------------------------------------------------╝
*/
//==================== libraries ====================//
#include <ESP8266WiFi.h>
#include <Math.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "beep.h"
#include "blinkLed.h"
#include "printFrame.h"

#define buzzer 2
//==================================================//
const char* ssid = "XuanLiem";          // Enter SSID here
const char* password = "haydoiday@75";  // Enter Password here
//==================================================//
int hr, mins, sec, dd, mm, yy;
int led_State = LOW;
float temp;
bool hasRun = false;
unsigned long epochTime;
unsigned long lastTime = 0;
unsigned long lastTime1 = 0;
unsigned long interval = 150;   // interval = 150 milisecond = 0.15s
WiFiClient client;
//=================================================//
LiquidCrystal_I2C lcd(0x27, 20, 4); // Display  I2C 20 x 4
RTC_DS3231 RTC;
//=================================================//
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7] = {"SUNDAY   ", "MONDAY   ", "TUESDAY  ", "WEDNESDAY", "THURSDAY ", "FRIDAY   ", "SATURDAY "};

//Month names
String months[12] = {"01", "02", "03", "04", "05", "06", "07", "08" , "09", "10", "11", "12"};
//=================================================//
byte verticalLine[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte char2[8] = {
  B00000,
  B00000,
  B00000,
  B11100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte char1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00111,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};

byte char3[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00111,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte char4[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b11100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
//=================================================//
void setup()  {
  Wire.begin(D1, D2);      // set I2C pins [SDA = D1, SCL = D2]
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  pinMode(buzzer, OUTPUT); // Set buzzer as an output
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  RTC.begin();

  Serial.begin(115200);
  Serial.println("OK");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (( millis()) <= (30000) ) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      LCD_connect();
    } else if (WiFi.status() == WL_CONNECTED) {
      LCD_connected();
      timeClient.begin();
      // Set offset time in seconds to adjust for your timezone, for example:
      // GMT +1 = 3600
      // GMT +7 = 25200
      timeClient.setTimeOffset(25200);
      delay(300);
      if (! RTC.begin()) {
        Serial.println("Couldn't find RTC");
        RTC_wire();
        RTC.adjust(DateTime(__DATE__, __TIME__));
      } else if (RTC.lostPower()) {
        Serial.println("RTC lost power, let's set the time!");
        RTC_lostPower();
        RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
      }
      //RTC.adjust(DateTime(2021, 3, 20, 17, 37, 0));
      //Add 50s for correct RTC after set times
      return;
    }
  }
}

void loop() {
  // We show the current date and time
  LCD_time();
  NTP_time();

  // Get temperature
  update_temp();
  // print Lunar Date to LCD
  if (hr > 0 && hasRun == false) {
    update_lunar(dd, mm, yy);
    hasRun = true;
  } else if ( hr > 0 && hasRun == true) {
    return;
  } else if (hr == 0 && hasRun == true || hr == 0 && hasRun == false) {
    update_lunar(dd, mm, yy);
    hasRun = true;
  }

}
//================================================================================//
void createCustomCharacters()
{
  lcd.createChar(0, verticalLine);
  lcd.createChar(1, char1);
  lcd.createChar(2, char2);
  lcd.createChar(3, char3);
  lcd.createChar(4, char4);
}
//================================================================================//
void LCD_connect() {
  lcd.clear();
  createCustomCharacters();
  printFrame();
  lcd.setCursor(3, 1);
  lcd.print("CONNECT  FAIL!");
  lcd.setCursor(2, 2);
  lcd.print("MODE: RTC MODULE");
  delay(2000);
  lcd.clear();
  digitalWrite(BUILTIN_LED, HIGH);
}
//================================================================================//
void LCD_connected() {
  lcd.clear();
  createCustomCharacters();
  printFrame();
  lcd.setCursor(4, 1);
  lcd.print("CONNECTED!!!");
  lcd.setCursor(3, 2);
  lcd.print("MODE: INTERNET");
  delay(2000);
  lcd.clear();
  digitalWrite(BUILTIN_LED, LOW);
}
//================================================================================//
void RTC_wire() {
  lcd.clear();
  createCustomCharacters();
  printFrame();
  lcd.setCursor(2, 1);
  lcd.print("RTC NOT RUNNING!");
  lcd.setCursor(3, 2);
  lcd.print("CHECK WIRRING!");
  delay(2000);
  lcd.clear();
}
//================================================================================//
void RTC_lostPower() {
  lcd.clear();
  createCustomCharacters();
  printFrame();
  lcd.setCursor(3, 1);
  lcd.print("RTC LOST POWER!");
  lcd.setCursor(3, 2);
  lcd.print("CHECK BATTERY!");
  delay(2000);
  lcd.clear();
}
//================================================================================//
void LCD_time() {
  DateTime now = RTC.now();

  hr = now.hour();
  mins = now.minute();
  sec = now.second();
  dd = now.day();
  mm = now.month();
  yy = now.year();

  // print Hour on LCD
  lcd.setCursor(6, 1);

  if (hr <= 9) {
    lcd.print("0");
  }
  lcd.print(hr, DEC);
  lcd.print(":");

  if (mins <= 9) {
    lcd.print("0");
  }
  lcd.print(mins, DEC);
  lcd.print(":");

  if (sec <= 9) {
    lcd.print("0");
  }
  lcd.print(sec, DEC);

  // print Date on LCD
  lcd.setCursor(10, 0);

  if (dd <= 9) {
    lcd.print("0");
  }
  lcd.print(dd, DEC);
  lcd.print("/");

  if (mm <= 9) {
    lcd.print("0");
  }
  lcd.print(mm, DEC);
  lcd.print("/");
  lcd.print(yy, DEC);

  // print Day on LCD
  lcd.setCursor(0, 0);
  char DOW[][10] = {"SUNDAY   ", "MONDAY   ", "TUESDAY  ", "WEDNESDAY", "THURSDAY ", "FRIDAY   ", "SATURDAY "};
  lcd.print(DOW[now.dayOfTheWeek()]); // if it appears error in the code, enter the code given below
  //lcd.print(DOW[now.dayOfWeek()]);

  //Beep each hour
  if (mins == 0 && sec == 0) {
    beep(buzzer);
  } else {
    return;
  }
}
//================================================================================//
void NTP_time() {
  if (WiFi.status() == WL_CONNECTED) {
    //blink LED
    blinkLed(led_State, lastTime, interval);
    timeClient.update();

    epochTime = timeClient.getEpochTime();
    hr = timeClient.getHours();
    mins = timeClient.getMinutes();
    sec = timeClient.getSeconds();

    String Day = weekDays[timeClient.getDay()];
    //Get a time structure
    struct tm *ptm = gmtime ((time_t *)&epochTime);
    dd = ptm->tm_mday;
    mm = ptm->tm_mon + 1;
    yy = ptm->tm_year + 1900;

    // print Hour on LCD
    lcd.setCursor(6, 1);

    if (hr <= 9) {
      lcd.print("0");
    }
    lcd.print(hr, DEC);
    lcd.print(":");

    if (mins <= 9) {
      lcd.print("0");
    }
    lcd.print(mins, DEC);
    lcd.print(":");

    if (sec <= 9) {
      lcd.print("0");
    }
    lcd.print(sec, DEC);

    // print Date on LCD
    lcd.setCursor(10, 0);

    if (dd <= 9) {
      lcd.print("0");
    }
    lcd.print(dd, DEC);
    lcd.print("/");

    if (mm <= 9) {
      lcd.print("0");
    }
    lcd.print(mm, DEC);
    lcd.print("/");
    lcd.print(yy, DEC);

    // print Day on LCD
    lcd.setCursor(0, 0);
    lcd.print(Day);

    RTC.adjust(DateTime(yy, mm, dd, hr, mins, sec));
    //Beep each hour
    if ( mins == 0 && sec == 0) {
      beep(buzzer);
    } else {
      return;
    }
  } else {
    digitalWrite(BUILTIN_LED, HIGH);
    return;
  }
}
//================================================================================//
void update_lunar(int dd, int mm, int yy) {
  lcd.setCursor(1, 2);
  //lcd.print("Lunar: ");
  convertSolar2Lunar(dd, mm, yy);
}
//================================================================================//
void update_temp() {
  DateTime now = RTC.now();
  // Temperature LCD
  temp = RTC.getTemperature();

  char tempF[5];
  dtostrf(temp, 7, 1, tempF);
  lcd.setCursor(1, 3);
  //lcd.print("Temp : ");
  lcd.print(tempF);
  lcd.print((char)223); // Show the symbol "°"
  lcd.print("C");
}
//================================================================================//
//get_Julius

long long getJulius(int dd, int mm, int yy) {
  int a, y, m, jd;
  a = (int)((14 - mm) / 12);
  y = yy + 4800 - a;
  m = mm + 12 * a - 3;
  jd = dd + (int)((153 * m + 2) / 5) + 365 * y + (int)(y / 4) - (int)(y / 100) + (int)(y / 400) - 32045;
  if (jd < 2299161) {
    jd = dd + (int)((153 * m + 2) / 5) + 365 * y + (int)(y / 4) - 32083;
  }
  return jd;
}
//================================================================================//
//get_New_Moon_Day
int getNewMoonDay(int k) {
  double T, T2, T3, dr, Jd1, M, Mpr, F, C1, deltat, JdNew;
  T = k / 1236.85;
  T2 = T * T;
  T3 = T2 * T;
  dr = PI / 180;
  double timeZone = 7.0;
  Jd1 = 2415020.75933 + 29.53058868 * k + 0.0001178 * T2 - 0.000000155 * T3;  // Mean new moon
  Jd1 = Jd1 + 0.00033 * sin((166.56 + 132.87 * T - 0.009173 * T2) * dr);  // Sun's mean anomaly
  M = 359.2242 + 29.10535608 * k - 0.0000333 * T2 - 0.00000347 * T3;  // Moon's mean anomaly
  Mpr = 306.0253 + 385.81691806 * k + 0.0107306 * T2 + 0.00001236 * T3;  // Moon's argument of latitude
  F = 21.2964 + 390.67050646 * k - 0.0016528 * T2 - 0.00000239 * T3;
  C1 = (0.1734 - 0.000393 * T) * sin(M * dr) + 0.0021 * sin(2 * dr * M);
  C1 = C1 - 0.4068 * sin(Mpr * dr) + 0.0161 * sin(dr * 2 * Mpr);
  C1 = C1 - 0.0004 * sin(dr * 3 * Mpr);
  C1 = C1 + 0.0104 * sin(dr * 2 * F) - 0.0051 * sin(dr * (M + Mpr));
  C1 = C1 - 0.0074 * sin(dr * (M - Mpr)) + 0.0004 * sin(dr * (2 * F + M));
  C1 = C1 - 0.0004 * sin(dr * (2 * F - M)) - 0.0006 * sin(dr * (2 * F + Mpr));
  C1 = C1 + 0.0010 * sin(dr * (2 * F - Mpr)) + 0.0005 * sin(dr * (2 * Mpr + M));
  if (T < -11) {
    deltat = 0.001 + 0.000839 * T + 0.0002261 * T2 - 0.00000845 * T3 - 0.000000081 * T * T3;
  } else {
    deltat = -0.000278 + 0.000265 * T + 0.000262 * T2;
  }
  JdNew = Jd1 + C1 - deltat;
  return (int)(JdNew + 0.5 + timeZone / 24);
}
//================================================================================//
//get_Sun_Longitude
int getSunLongitude(int jdn) {
  double timeZone = 7.0;
  double T, T2, dr, M, L0, DL, L;
  // Time in Julian centuries from 2000-01-01 12:00:00 GMT
  T = (jdn - 2451545.5 - timeZone / 24) / 36525;
  T2 = T * T;   // degree to radian
  dr = PI / 180;  // mean anomaly, degree
  M = 357.52910 + 35999.05030 * T - 0.0001559 * T2 - 0.00000048 * T * T2;  // mean longitude, degree
  L0 = 280.46645 + 36000.76983 * T + 0.0003032 * T2;
  DL = (1.914600 - 0.004817 * T - 0.000014 * T2) * sin(dr * M);
  DL = DL + (0.019993 - 0.000101 * T) * sin(dr * 2 * M) + 0.000290 * sin(dr * 3 * M);
  L = L0 + DL; // true longitude, degree
  L = L * dr;  // Normalize to (0, 2*PI)
  L = L - PI * 2 * (int)(L / (PI * 2));
  return (int)(L / PI * 6);
}
//================================================================================//
//get_Lunar_Month_11
int getLunarMonthll(int yy) {
  double k, off, nm, sunLong;
  off = getJulius(31, 12, yy) - 2415021;
  k = (int)(off / 29.530588853);
  nm = getNewMoonDay((int)k);
  // sun longitude at local midnight
  sunLong = getSunLongitude((int)nm);
  if (sunLong >= 9) {
    nm = getNewMoonDay((int)k - 1);
  }
  return (int)nm;
}
//================================================================================//
//get_Leap_Month_Offset
int getLeapMonthOffset(double a11) {
  double last, arc;
  int k, i;
  k = (int)((a11 - 2415021.076998695) / 29.530588853 + 0.5);
  last = 0;
  // We start with the month following lunar month 11
  i = 1;
  arc = getSunLongitude((int)getNewMoonDay((int)(k + i)));
  do {
    last = arc;
    i++;
    arc = getSunLongitude((int)getNewMoonDay((int)(k + i)));
  } while (arc != last && i < 14);
  return i - 1;
}
//================================================================================//
//convert_Solar_to_Lunar
int convertSolar2Lunar(int dd, int mm, int yy) {
  double dayNumber, monthStart, a11, b11, lunarDay, lunarMonth, lunarYear;
  //double lunarLeap;
  int k, diff;
  dayNumber = getJulius(dd, mm, yy);
  k = (int)((dayNumber - 2415021.076998695) / 29.530588853);
  monthStart = getNewMoonDay(k + 1);
  if (monthStart > dayNumber) {
    monthStart = getNewMoonDay(k);
  }
  a11 = getLunarMonthll(yy);
  b11 = a11;
  if (a11 >= monthStart) {
    lunarYear = yy;
    a11 = getLunarMonthll(yy - 1);
  } else {
    lunarYear = yy + 1;
    b11 = getLunarMonthll(yy + 1);
  }
  lunarDay = dayNumber - monthStart + 1;
  diff = (int)((monthStart - a11) / 29);
  //lunarLeap = 0;
  lunarMonth = diff + 11;
  if (b11 - a11 > 365) {
    int leapMonthDiff = getLeapMonthOffset(a11);
    if (diff >= leapMonthDiff) {
      lunarMonth = diff + 10;
      if (diff == leapMonthDiff) {
        //lunarLeap = 1;
      }
    }
  }
  if (lunarMonth > 12) {
    lunarMonth = lunarMonth - 12;
  }
  if (lunarMonth >= 11 && diff < 4) {
    lunarYear -= 1;
  }

  lcd.print(int(lunarDay));
  lcd.print("/");
  lcd.print(int(lunarMonth));
  lcd.print("     ");
  /*
    lcd.print("/");
    lcd.print(int(lunarYear));
  */
}
