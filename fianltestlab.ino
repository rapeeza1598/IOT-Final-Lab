// -------------------- Feb, 2024 ------------------
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "DHT.h"

#define DHTPIN D2
#define LENPIN D1
#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321
#define SENSOR_PIN A0

char ssid[] = "IoT-Gateway";                     //ชื่อไวไฟ
char pass[] = "0840445565";                      //รหัสไวไฟ
unsigned long myChannelNumber = 2419339;         //เลข ID
const char* myWriteAPIKey = "VXW7LT8OWKWW53ZC";  //API KEY

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;


unsigned long period = 1000;  //ระยะเวลาที่ต้องการรอ
unsigned long last_time = 0;  //ประกาศตัวแปรเป็น global เพื่อเก็บค่าไว้ไม่ให้ reset จากการวนloop

void setup() {
  Serial.begin(115200);
  pinMode(LENPIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  Serial.println(F("DHTxx test!"));
  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(3000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  if (t > 30) {
    // if( millis() - last_time > period) {
    //   digitalWrite(LENPIN, HIGH);
    // }
    // if( millis() - last_time > period+500){
    //   digitalWrite(LENPIN, LOW);
    // }
    digitalWrite(LENPIN, HIGH);
    delay(500);
    digitalWrite(LENPIN, LOW);
    delay(500);
  } else {
    digitalWrite(LENPIN, LOW);
  }

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  int sensorValue = analogRead(SENSOR_PIN);
  int senmap = map(sensorValue, 0, 1023, 0, 100);
  Serial.print(F("Soil Moisture Sensor: "));
  Serial.print(senmap);
  Serial.println(" %");

  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3, sensorValue);

  if (millis() - last_time > 5000) {
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.println(F("°F"));
}
