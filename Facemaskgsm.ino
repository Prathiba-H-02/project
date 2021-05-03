
#include<SoftwareSerial.h>
SoftwareSerial message(2, 3); //(rx, tx) Serial communication

#include"gsm.h"
#include"ultrasonic.h"

#include<LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

#include <Servo.h>
Servo myservo;

#include<DS3231.h>
DS3231 rtc(SDA, SCL);
Time t;

uint8_t door = 0, level = 0;

ULTRASONIC ultra1, ultra2;

int lower_uv = A0;
int middle_uv = A1;
int upper_uv = A2;
int Min, Hour, Sec;

void send_sms(String number, String text);

void setup()
{
  Serial.begin(115200);  //iot (TX0-RX)// nodemcu digital pin
  message.begin(9600); //gsm(3-RX)
  lcd.begin(16, 2);
  ultra1.begin(4, 5); //DOOR
  ultra2.begin(6, 7); //LEVEL
  rtc.begin();
  
  rtc.setTime(7, 59, 00);     // Set the time to 12:00:00 (24hr format)
  //
  //  rtc.setDate(21, 3, 2021);   // Set the date to January 1st, 2014
  myservo.attach(A3);
  // LED activation
  pinMode(lower_uv, OUTPUT);
  pinMode(middle_uv, OUTPUT);
  pinMode(upper_uv, OUTPUT);
  digitalWrite(lower_uv, LOW);
  digitalWrite(middle_uv, LOW);
  digitalWrite(upper_uv, LOW);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("SMART");
  lcd.setCursor(0, 1);
  lcd.print("FaceMasks Collection");
  myservo.write(0);
  delay(2000);
}

void loop()
{
  t = rtc.getTime();
  Min = t.min;
  Hour = t.hour;
  Sec = t.sec;
  digitalWrite(lower_uv, HIGH); // Bottom most UV Led light set on 
  door = ultra1.ultra();
  level = ultra2.ultra();
  //  Serial.println("D1:" + String(door));
  //  Serial.println("D2:" + String(level));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("D:" + String(door));
  lcd.setCursor(6, 0);
  lcd.print("L:" + String(level));
  lcd.setCursor(11, 0);
  lcd.print("T:");
  lcd.print(String(Hour));
  lcd.print(":");
  lcd.print(String(Min));

  if (Hour == 8 && Min == 00 && Sec <= 15)
  {
    level = ultra2.ultra();
    switch (level)
    {
      case 5 ...9:
        lcd.setCursor(0, 0);
        lcd.print("Message sending");
        send_sms("1234567890", "Bin is FULL, disinfection ongoing");// Provide phone number of respective user as first parameter 
        break;
      case 10 ...12:
        lcd.setCursor(0, 0);
        lcd.print("Message sending");
        send_sms("1234567890", "Bin is 50% FILLED, second LED activated");// Provide phone number of respective user as first parameter
        break;
      case 13 ...17:
        lcd.setCursor(0, 0);
        lcd.print("Message sending");
        send_sms("1234567890", "Bin is EMPTY");// Provide phone number of respective user as first parameter
        break;
    }
  }
// bin level measurement total bin length =20 cm
  if (door < 15)
  {
    lcd.setCursor(0, 1);
    lcd.print("DOOR OPENED");
    myservo.write(180);
    delay(5000);
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("DOOR CLOSED");
    myservo.write(0);
    delay(1000);
  }

  if (level > 5 && level < 10)
  {
    digitalWrite(upper_uv, HIGH);// Top most Led UV light set on 
    lcd.setCursor(0, 1);
    lcd.print("BIN FULL");
    Serial.print("*90#"); //iot
    delay(5000);
  }
  else if (level > 10 && level < 13)
  {
    digitalWrite(middle_uv, HIGH);// Middle Led UV light set on
    lcd.setCursor(0, 1);
    lcd.print("PARTIALLY FILLED");
    Serial.print("*50#"); //iot
    delay(5000);
  }
  else if (level > 13)
  {
    digitalWrite(upper_uv, LOW);
    digitalWrite(middle_uv, LOW);
    lcd.setCursor(0, 1);
    lcd.print("BIN EMPTY");
    Serial.print("*20#"); //iot
    delay(5000);
  }
  delay(700);
}
