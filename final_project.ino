#include "FirebaseESP8266.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
// #include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include "DHT.h"



// //Provide the token generation process info.
// #include "addons/TokenHelper.h"
// //Provide the RTDB payload printing info and other helper functions.
// #include "addons/RTDBHelper.h"


//lcd display
  
// LiquidCrystal_I2C lcd(0x27,16,2);


////servo
Servo myservo;


//firebase
#define FIREBASE_HOST "smart-home-b3499-default-rtdb.firebaseio.com"
#define web_api_key "AIzaSyDIhU9EnUmFFt9w0Hvjfoqp9ajMSpiondM"
#define user_email "kronosbd82@gmail.com"
#define user_pass "kronos"
//wifi
#define WIFI_SSID "Yroy"
#define WIFI_PASSWORD "truestrock"
//Define FirebaseESP8266 data object
FirebaseData firebaseData;
FirebaseData ledData;
FirebaseData firebaseData_home_light;
FirebaseAuth auth;
FirebaseConfig config;


 
#define servo_motor 16 //D0
#define DHTPIN 2 //D4
#define green_led 14 //D5
#define home_light 12 //D6
#define exaust_fan 13 //d7 
#define red_led_buzzer 15 //D8 
#define gas_sensor 0 //A0



//gas sensor
int reading=0;
int fan=0;
int gas=0;
int override=0;

//temparature sensor
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Wire.begin();
  Serial.begin(9600);

  //servo motor setup start
  myservo.attach(servo_motor);
  myservo.write(0);


  //servo motor setup end

// Initialize the LCD display
  // lcd.init();
  // lcd.backlight();
  // lcd.clear();

  // // Print the initial message on the LCD display
  // lcd.setCursor(0, 0);
  // lcd.print("Gas Sensor Value:");


//wifi connection start
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("\nConnecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    //digitalWrite(LED_BUILTIN, LOW) ;
    Serial.print(".");
    delay(300);
    }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
//wifi connection end
  

//firebase connection start
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = web_api_key;
  config.database_url = FIREBASE_HOST;
  auth.user.email = user_email;
  auth.user.password = user_pass;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(2);
//firebase connection end


  dht.begin();
  pinMode(exaust_fan,OUTPUT);
  pinMode(red_led_buzzer,OUTPUT);
  pinMode(green_led,OUTPUT);
  pinMode(home_light,OUTPUT);
 // pinMode(LED_BUILTIN, OUTPUT);


}

void loop()
{
  //DHT 11 start
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
  Serial.println("\nFailed to read from DHT sensor!");}
  Serial.print("\nTemperature ");
  Serial.println(t);
  Serial.print("Humidity ");
  Serial.println(h);
  Firebase.setString(firebaseData,"/temp", t);
  Firebase.setString(firebaseData,"/humidity", h);
  //DHT 11 end

  //gas sensor start
  int gas_sensorValue = analogRead(gas_sensor);
  //float gasConcentration = map(gas_sensorValue, 0, 1023, 0, 100);
  Serial.print("\nGas Sensor Value: ");
  Serial.print(gas_sensorValue);
   Firebase.setString(firebaseData,"/gas", gas_sensorValue);  
  // Print the gas concentration value on the LCD display
  // lcd.setCursor(0, 1);
  // lcd.print("        ");
  // lcd.setCursor(0, 1);
  // lcd.print(gas_sensorValue);
  //gas sensor end


// servo controll by serial start
// if (Serial.available()) {    // if serial data is available
//     int pos = Serial.parseInt();    // read the serial data as an integer
//     myservo.write(pos);    // move the servo to the specified position
//   }
//Serial.print(myservo.read());
//servo controll by serial end

  
  //printing gas sensor value
  if(gas_sensorValue>120){
    digitalWrite(red_led_buzzer,HIGH);
    digitalWrite(green_led,LOW);
    digitalWrite(exaust_fan,HIGH);


    //delay(600);
    Firebase.setString(firebaseData,"/buzzer", "1");
    Firebase.setString(firebaseData,"/fan", "1");
    digitalWrite(red_led_buzzer,LOW);
    
    myservo.write(180);

  }
  else{
    Firebase.setString(firebaseData,"/buzzer", "0");
    Firebase.setString(firebaseData,"/fan", "0");
    digitalWrite(green_led,HIGH);
    digitalWrite(exaust_fan,LOW);
    digitalWrite(red_led_buzzer,LOW);
    myservo.write(0);
  }

  //for home light on off start
  Firebase.getString(firebaseData_home_light, "/home light");
  if(firebaseData_home_light.stringData()== "1"){
    digitalWrite(home_light,HIGH);
  }

  else{
    //Serial.printf(" home light=", firebaseData_home_light.stringData() );
    digitalWrite(home_light,LOW);
  }
  
  //home light on off end
  
}
