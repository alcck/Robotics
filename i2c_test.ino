#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // ESP32-C3 OLED pinleri (SDA: 5, SCL: 6)
  Wire.begin(5, 6);
  Serial.println("\nI2C Tarayici Baslatiliyor...");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Cihazlar taraniyor...");

  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C cihazi bulundu, adres: 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Bilinmeyen hata, adres: 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
    Serial.println("Hicbir I2C cihazi bulunamadi!\n");
  else
    Serial.println("Tarama tamamlandi.\n");

  delay(5000);
}