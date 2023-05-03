// #include <ESP8266WiFi.h>
const int e=30;
const int t=29;
const int pwmPin=28;
const int ledPin=27;
const int potPin=26;
int i=0;
long duration, distance;
void setup() {
  Serial.begin(9600);
  // pinMode(A1,INPUT);
  pinMode(pwmPin,OUTPUT);
  pinMode(e, INPUT);
  pinMode(t, OUTPUT);
  pinMode(ledPin,OUTPUT);
  // pinMode(A2)
}

void loop() {
  int a = map(analogRead(potPin),0,1023,0,255);
  Serial.println("a : "+String(a));
  // float b = map(a,0,1023,0,255);
  // analogWrite(A1,255);
  // digitalWrite(10,1);
  analogWrite(pwmPin,potPin);
  // Serial.println(analogread(1));
  // Serial.println("b : "+String(b));

  digitalWrite(t, LOW); 
 delayMicroseconds(5); 
 digitalWrite(t, HIGH); 
 delayMicroseconds(5); 
 digitalWrite(t, LOW); //ใช้งานขา trig

 duration = pulseIn(e, HIGH); //อ่านค่าของ echo
 distance = (duration/2) / 29.1; //คำนวณเป็น centimeters
 Serial.print(distance); 
 Serial.println(" cm\n");
  if(distance <= 5){ //ระยะการใช้งาน
  digitalWrite(ledPin, LOW);
 } //เงื่อนไข ถ้าระยะน้อยกว่า 5cm ให้ led ติด
 else{
  digitalWrite(ledPin, HIGH);
 }
  
  // put your main code here, to run repeatedly:

}
