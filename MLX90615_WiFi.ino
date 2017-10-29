#include "MLX90615.h"
#include <I2cMaster.h>
#include <SoftwareSerial.h>
#define SDA_PIN 3   //define the SDA pin
#define SCL_PIN 2   //define the SCL pin

SoftI2cMaster i2c(SDA_PIN, SCL_PIN);
MLX90615 mlx90615(DEVICE_ADDR, &i2c);

//WiFi
// replace with your channel's thingspeak API key
String apiKey = "K93JGGZ3LTFGQSFO";

// connect 2 to TX of Serial USB
// connect 3 to RX of serial USB
SoftwareSerial ser(6,7); // RX, TX


void setup()
{
  //Serial.println("Setup...");
  Serial.begin(9600); 
  // enable software serial
  ser.begin(9600);

  //mlx90615.writeEEPROM(Default_Emissivity); //write data into EEPROM when you need to adjust emissivity.
  //mlx90615.readEEPROM(); //read EEPROM data to check whether it's a default one.
//WiFi
// reset ESP8266
  ser.println("AT+RST");
}

void loop()
{
  Serial.print("Object temperature: ");
  Serial.println(mlx90615.getTemperature(MLX90615_OBJECT_TEMPERATURE));
  Serial.print("Ambient temperature: ");
  Serial.println(mlx90615.getTemperature(MLX90615_AMBIENT_TEMPERATURE));
  delay(1000);
  //WiFi
  esp_8266();
  }
  void esp_8266()
{
 // convert to string
  char buf[32];
  String strTob = dtostrf( mlx90615.getTemperature(MLX90615_OBJECT_TEMPERATURE), 4, 1, buf);
  String strTam = dtostrf( mlx90615.getTemperature(MLX90615_AMBIENT_TEMPERATURE), 4, 1, buf);
  Serial.print(strTob);
  Serial.print(strTam);
 
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  ser.println(cmd);
   
  if(ser.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String( mlx90615.getTemperature(MLX90615_AMBIENT_TEMPERATURE));
  getStr +="&field2=";
  getStr += String(mlx90615.getTemperature(MLX90615_OBJECT_TEMPERATURE));
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ser.println(cmd);

  if(ser.find(">")){
    ser.print(getStr);
  }
  else{
    ser.println("AT+CIPCLOSE");
    // alert user
    Serial.println("AT+CIPCLOSE");
  }
    
  // thingspeak needs 15 sec delay between updates
  delay(16000);  
  
}
