#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  // ESP32-C3 için I2C pin tanımlaması (SDA: 5, SCL: 6)
  Wire.begin(5, 6);
  Serial.println("\nI2C Tarayici Baslatiliyor...");
  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU başarıyla bağlandı!");
  } else {
    Serial.println("MPU bağlanmadı!");
    while(1);
  }
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  // Read accelerometer & gyroscope
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.print("Accel (X Y Z): ");
  Serial.print(ax); Serial.print(" ");
  Serial.print(ay); Serial.print(" ");
  Serial.print(az);

  Serial.print(" | Gyro (X Y Z): ");
  Serial.print(gx); Serial.print(" ");
  Serial.print(gy); Serial.print(" ");
  Serial.println(gz);

  delay(500);
}
