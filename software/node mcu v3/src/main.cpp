#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>
#include "LittleFS.h"
#include <ESP8266WiFi.h>
// for uno test
// const int e = 13;
// const int t = 12;
// const int pwmPin = 10;
// const int ledPin = 11;
// const int potPin = A0;

// for esp8266 test *use gpio pin*
// blynk for esp8266 in platformio
/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           ""
#define BLYNK_TEMPLATE_NAME         ""
#define BLYNK_AUTH_TOKEN            ""

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

// *********************************************
// setting web server
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output13State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
// const int output13= 13;
// const int output4 = 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
// *********************************************

#include <BlynkSimpleEsp8266.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

BlynkTimer timer;

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}
//end blynk setting

//lcd 1602 and PCF8574
const int sda = 4; //fix
const int scl = 5; //fix
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 for PCF8574 and 0x3F for PCF8574A

// ultrasonic
const int e = 0;
const int t = 16;

const int pwmPin = 14;
// const int ledPin = 12;
const int ledPin2 = 13;

// motor driver L298N
const int enA = 12;
const int in1 = 10;
// const int in2 = 16;
const int enB = 15;
const int in3 = 1;
const int in4 = 3;


String fsText = "";

// analog input = A0
const int potPin=A0;

int VV;

int i = 0;
long duration, distance;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0){
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
//   if (value == 1) {
//     analogWrite(ledPin, 255);
//   } else {
//     analogWrite(ledPin, 0);
//   }
    //   Blynk.virtualWrite(V1 , value);
}
BLYNK_WRITE(V1){
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();
  VV = value;
    // analogWrite(ledPin, value);

    Serial.println("value : "+String(value));
  // Update state
//   if (value == 1) {
//     analogWrite(ledPin, 255);
//   } else {
//     analogWrite(ledPin, 0);
//   }
    //   Blynk.virtualWrite(V1 , value);
}

// lettleFs function
void lettleFS(){
    if(!LittleFS.begin()){
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    
    File file = LittleFS.open("/text.txt", "r");
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }
    
    Serial.print("File Content:");
    while(file.available()){
        fsText = file.readString();
        Serial.println(file.readString());
    }
    file.close();
}

// lcd 1602 and PCF8574 setup function
void lcdsetting(){
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Start...");
}

// wifi web server and blynk function
void web_wifi_blynk(){
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass); // wifi connect
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); // wifi connect too! but blynk

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();

    
    // You can also specify server:
    //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
    //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
    timer.setInterval(1000L, myTimerEvent);
}

// web server and setup dashboard function
void webServ_DashSet(int a,int b){
    WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    // Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /13/on") >= 0) {
              Serial.println("GPIO 13 on");
              output13State = "on";
              digitalWrite(ledPin2, HIGH);
            } else if (header.indexOf("GET /13/off") >= 0) {
              Serial.println("GPIO 13 off");
              output13State = "off";
              digitalWrite(ledPin2, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
            //   digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
            //   digitalWrite(output4, LOW);
            } else if (header.indexOf(" /refresh") >= 0) {
                client.println("");
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            // client.println("<meta http-equiv=\"refresh\" content=\"1\" \">"); // refresh every 1 sec
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server Dash Board</h1>");

            client.println("<h2>"+fsText+"</h2>");
            
            // Display current state, and ON/OFF buttons for GPIO 13  
            client.println("<p>GPIO 13 - State " + output13State + "</p>");
            // If the output13State is off, it displays the ON button       
            if (output13State=="off") {
              client.println("<p><a href=\"/13/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/13/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO   
            client.println("<p>GPIO led1 - value : " + String(VV) + "</p>");
            client.println("<p>UltrasonicSensor : " + String(distance) + "</p>");
            client.println("<p>Manual POT(~1023): " + String(a) + "</p>");
            client.println("<p>Manual POT(~255): " + String(b) + "</p>");


            client.println("<p><a href=\"/refresh\"><button class=\"button button2\">ReFresh</button></a></p>");
            // int CVV = VV;
            // if (CVV != VV){
            //     client.println("<p>GPIO led1 - value " + String(VV) + "</p>");
            // } 
            

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";

    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void setup()
{
    lcd.setCursor(0, 0);
    lcd.print("Hello World");

    Serial.begin(115200);

    // wifi web server and blynk setup
    // web_wifi_blynk();

    Serial.println("Hello World222555");

    // call littleFS
    // lettleFS();
    // Serial.println("txt: "+fsText);
    
    // lcd 1602 and PCF8574 setup
    lcdsetting();
    // lcd.begin(16, 2);
    // lcd.clear();

    // pinMode(A1,INPUT);
    pinMode(pwmPin, OUTPUT);

    pinMode(e, INPUT);
    pinMode(t, OUTPUT);
    // pinMode(ledPin, OUTPUT);
    pinMode(ledPin2, OUTPUT);
    // pinMode(A2)

}
void loop()
{
    Blynk.run();
    timer.run();

    int a = 191;
    // int a = map(analogRead(potPin), 0, 757, 0, 255);
    int b = map(analogRead(potPin),0,1024,0, 255);

      digitalWrite(t, LOW);
			delayMicroseconds(2);
			
			// Sets the trigPin on HIGH state for 10 micro seconds
			digitalWrite(t, HIGH);
			delayMicroseconds(10);
			digitalWrite(t, LOW);
			
			// Reads the echoPin, returns the sound wave travel time in microseconds
			duration = pulseIn(e, HIGH);
			
			// Calculating the distance
			distance= duration*0.034/2;

      // lcd.setCursor(0, 0);
      lcd.setCursor(0, 1);
      lcd.print("b:" + String(a));
      lcd.setCursor(8, 1);
      lcd.print("d:" + String(distance));
      
      Serial.println("distance : " + String(distance));


    // int a = analogRead(potPin);


    // call weq server and dash board set fuction
    webServ_DashSet(a,b);
          // 
     
    // You can inject your own code or combine it with other sketches.
    // Check other examples on how to communicate with Blynk. Remember
    // to avoid delay() function!

    // Serial.println("a : " + String(a));
    // Serial.println("b : " + String(b));



    // digitalWrite(ledPin, HIGH);
    // delay(1000);

   

    analogWrite(pwmPin, a);

    // Serial.println(analogread(1));
    // Serial.println("b : "+String(b));

     if (distance <= 5)
    { // ระยะการใช้งาน
        digitalWrite(ledPin2, 1);
    } // เงื่อนไข ถ้าระยะน้อยกว่า 5cm ให้ led ติด
    else
    {
        digitalWrite(ledPin2, 0);
    }

    // digitalWrite(ledPin2, 1);



    // float b = map(a,0,1023,0,255);
    // analogWrite(A1,255);
    // digitalWrite(10,1);
    
    // lcd.clear(); 
    lcd.setCursor(3, 1);
    lcd.print("    ");
    lcd.setCursor(10, 1);
    lcd.print("    ");
    // put your main code here, to run repeatedly:
}