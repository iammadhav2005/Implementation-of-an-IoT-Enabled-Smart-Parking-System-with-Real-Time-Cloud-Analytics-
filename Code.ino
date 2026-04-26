#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define IR_SENSOR D5
#define GREEN_LED D7
#define RED_LED D8
#define SERVO_PIN D6

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

String apiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";
const char* server = "api.thingspeak.com";

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gateServo;

int slotAvailable = 1;

void setup()
{
  Serial.begin(115200);

  pinMode(IR_SENSOR, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  gateServo.attach(SERVO_PIN);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Smart Parking");

  WiFi.begin(ssid, password);

  lcd.setCursor(0,1);
  lcd.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("WiFi Connected");
  delay(2000);
}

void loop()
{
  int sensorValue = digitalRead(IR_SENSOR);

  if(sensorValue == LOW)   // Car detected
  {
    slotAvailable = 0;

    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);

    gateServo.write(90);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Parking Full");
    lcd.setCursor(0,1);
    lcd.print("Slots: 0");
  }
  else
  {
    slotAvailable = 1;

    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);

    gateServo.write(0);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Slot Available");
    lcd.setCursor(0,1);
    lcd.print("Slots: 1");
  }

  sendToThingSpeak(slotAvailable);

  delay(15000);
}

void sendToThingSpeak(int data)
{
  WiFiClient client;

  if (!client.connect(server, 80))
  {
    Serial.println("Connection Failed");
    return;
  }

  String url = "/update?api_key=" + apiKey + "&field1=" + String(data);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Data sent to ThingSpeak");
}