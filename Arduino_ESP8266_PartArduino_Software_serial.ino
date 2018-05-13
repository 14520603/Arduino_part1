#include <SoftwareSerial.h>
#include "DHT.h"
String readString;
SoftwareSerial ESPSerial(7,8); // RX, TX

const int DHTPIN = 9;       //Đọc dữ liệu từ DHT11 ở chân 9 trên mạch Arduino
const int DHTTYPE = DHT11;  //Khai báo loại cảm biến
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Open serial communications
  Serial.begin(9600);
  // set the data rate for the SoftwareSerial port
  // if this doesnt work you for you then try different baud rate
  ESPSerial.begin(9600);
  Serial.println("Arduino Uno R3 Is Now Ready To Work");
  pinMode(10,OUTPUT);
  digitalWrite(10,LOW);
  pinMode(11,OUTPUT);
  digitalWrite(11,LOW);
  dht.begin();         // Khởi động cảm biến
}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 1.0;

void loop(void) {
  while (ESPSerial.available()) {
    // reads input from ESP8266 
    delay(5); 
    char c = ESPSerial.read();
    readString += c; 
  }
  
  readString.trim();
  if (readString.length() >0) {

    if(readString == "switching on"||readString == "switching off"){
      Serial.println(readString);
    }
    
    if (readString == "1"){     
      Serial.println(readString);
      //Serial.println("Pin 10 is HIGH");
      digitalWrite(10, HIGH);
    }
    
    if (readString == "0"){
      Serial.println(readString);     
      //Serial.println("Pin 10 is LOW");
      digitalWrite(10, LOW);
    }

     if (readString == "2"){ 
      Serial.println(readString);
      //Serial.println("Pin 11 is HIGH");
      digitalWrite(11, HIGH);
    }
    
    if (readString == "3"){
      Serial.println(readString);     
      //Serial.println("Pin 11 is LOW");
      digitalWrite(11, LOW);
    }

    readString=""; 
  }
  
  while (Serial.available()) {
    // reads input from PC and writes to ESP8266
    ESPSerial.write(Serial.read());
  }
  
  //DHT11 part
  long DHTtime = millis();
  if (DHTtime - lastMsg > 2000) {
    lastMsg = DHTtime;

    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();
    // Đọc giá trị nhiệt độ F(isFahrenheit = true)
    float f = dht.readTemperature(true);
    
    // Kiểm tra quá trình đọc thành công hay không
    if (isnan(newTemp) || isnan(newHum) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
    }
    
    if (checkBound(newTemp, temp, diff)) {
      temp = newTemp;
      Serial.print("New temperature:");
      Serial.println(String(temp).c_str());
      ESPSerial.write("Temperature update");
      delay(100);
      ESPSerial.write(String(temp).c_str());
      delay(5);
    }

    if (checkBound(newHum, hum, diff)) {
      hum = newHum;
      Serial.print("New humidity:");
      Serial.println(String(hum).c_str());
      ESPSerial.write("Humidity update");
      delay(100);
      ESPSerial.write(String(hum).c_str());
      delay(5);
    }
  }
}
