#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32  // 128x32 OLED display
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool isSleeping = true;
unsigned long lastMotionTime = 0;
const unsigned long sleepTimeout = 10000; // 10 seconds of stillness before sleeping
const float shakeThreshold = 15.0;        // Higher value = requires a much firmer shake to wake up

void setup() {
  Serial.begin(115200);
  delay(500);
  
  // Initialize I2C with your custom pins: SDA = 5, SCL = 6
  Wire.begin(5, 6);
  delay(250);

  // Wake up the MPU6050 from its default sleep mode
  Wire.beginTransmission(0x68);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // Clear sleep bit
  Wire.endTransmission(true);

  // Initialize OLED display (address 0x3C)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  drawSleepingFace(0);
  display.display();
  
  Serial.println("System initialized successfully!");
}

void loop() {
  // Read raw acceleration directly over I2C
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 6, true);

  if (Wire.available() >= 6) {
    int16_t ax = (Wire.read() << 8) | Wire.read();
    int16_t ay = (Wire.read() << 8) | Wire.read();
    int16_t az = (Wire.read() << 8) | Wire.read();

    float accX = (ax / 16384.0) * 9.81;
    float accY = (ay / 16384.0) * 9.81;
    float accZ = (az / 16384.0) * 9.81;

    float totalAcc = sqrt(pow(accX, 2) + pow(accY, 2) + pow(accZ, 2));
    float accDelta = abs(totalAcc - 9.81);

    // If movement exceeds the higher threshold, wake up!
    if (accDelta > shakeThreshold) {
      if (isSleeping) {
        isSleeping = false;
        Serial.println("Woke up!");
      }
      lastMotionTime = millis();
    }

    // Return to sleep after timeout of inactivity
    if (!isSleeping && (millis() - lastMotionTime > sleepTimeout)) {
      isSleeping = true;
      Serial.println("Going back to sleep...");
    }
  }

  // Calculate animation frames based on uptime
  unsigned long currentMillis = millis();
  int animFrame = (currentMillis / 400) % 3;

  // Update OLED face with animation
  display.clearDisplay();
  if (isSleeping) {
    drawSleepingFace(animFrame);
  } else {
    drawAwakeFace(currentMillis);
  }
  display.display();

  delay(50);
}

void drawSleepingFace(int frame) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 8); // Centered vertically
  display.print("(- -)");
  
  // Floating Zzz animation frames
  display.setTextSize(1);
  if (frame == 0) {
    display.setCursor(90, 12); display.print("Z");
    display.setCursor(102, 6); display.print("z");
  } else if (frame == 1) {
    display.setCursor(92, 8);  display.print("Z");
    display.setCursor(104, 2); display.print("z");
  } else {
    display.setCursor(94, 4);  display.print("Z");
    display.setCursor(106, 0); display.print("z");
  }
}

void drawAwakeFace(unsigned long t) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  // Lowered Y offset so the caret '^' eyes don't clip the top of the screen
  int yOffset = (sin(t / 150.0) > 0) ? 10 : 12; 
  
  display.setCursor(20, yOffset);
  display.print("(^o^)");
}
