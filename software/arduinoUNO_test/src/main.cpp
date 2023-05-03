#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>

// for arduino uno test
const int e = 13;
const int t = 12;
const int pwmPin = 10;
const int ledPin = 11;
const int potPin = A0;
const int sda = A4; //fix
const int sdl = A5; //fix

// for esp8266 test *use gpio pin*
//lcd 1602 and PCF8574
// const int sda = 4; //fix
// const int sdl = 5; //fix
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 for PCF8574 and 0x3F for PCF8574A

// ultrasonic
// const int e = 0;
// const int t = 2;

// const int pwmPin = 14;
// const int ledPin = 12;

// analog input = A0
// const int potPin=A0;

int i = 0;
long duration, distance;

void setup()
{
    Serial.begin(9600);
    Serial.println("Hello World222555");
    // pinMode(A1,INPUT);
    pinMode(pwmPin, OUTPUT);
    pinMode(e, INPUT);
    pinMode(t, OUTPUT);
    pinMode(ledPin, OUTPUT);
    // pinMode(A2)

    // lcd 1602 and PCF8574 setup
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Hello World");
}
void loop()
{
    int b = analogRead(potPin);
    // int a = map(analogRead(potPin), 0, 757, 0, 255);
    int a = map(analogRead(potPin),0,1023,0, 255);

    Serial.println("a : " + String(a));
    Serial.println("b : " + String(b));

    // float b = map(a,0,1023,0,255);
    // analogWrite(A1,255);
    // digitalWrite(10,1);

    analogWrite(pwmPin, a);

    // Serial.println(analogread(1));
    // Serial.println("b : "+String(b));

    digitalWrite(t, LOW);
    delayMicroseconds(5);
    digitalWrite(t, HIGH);
    delayMicroseconds(5);
    digitalWrite(t, LOW); // ใช้งานขา trig

    duration = pulseIn(e, HIGH);      // อ่านค่าของ echo
    distance = (duration / 2) / 29.1; // คำนวณเป็น centimeters
    Serial.print(distance);
    Serial.println(" cm");

    if (distance <= 5)
    { // ระยะการใช้งาน
        digitalWrite(ledPin, LOW);
    } // เงื่อนไข ถ้าระยะน้อยกว่า 5cm ให้ led   ติด
    else
    {
        digitalWrite(ledPin, HIGH);
    }
    // put your main code here, to run repeatedly:
}