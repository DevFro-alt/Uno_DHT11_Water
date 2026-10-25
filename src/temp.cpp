#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 2
#define BUTTON_PIN 4
#define BUZZER_PIN 8
#define WATER_PIN A0
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

int displayMode = 0;

int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

unsigned long lastSensorReadTime = 0;
const unsigned long sensorInterval = 500;

void updateDisplay() {
  int waterVal = analogRead(WATER_PIN);

  if (waterVal >= 300) {
    tone(BUZZER_PIN, 2000);
  } else {
    noTone(BUZZER_PIN);
  }

  if (displayMode == 0) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    lcd.setCursor(0, 0);
    lcd.print("Mode: Temp & Hum");

    lcd.setCursor(0, 1);
    if (isnan(temp) || isnan(hum)) {
      lcd.print("DHT Read Error! ");
    } else {
      lcd.print("T:");
      lcd.print((int)temp);
      lcd.print((char)223);
      lcd.print("C  H:");
      lcd.print((int)hum);
      lcd.print("%   ");
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Mode: Water Level");

    lcd.setCursor(0, 1);
    if (waterVal < 50) {
      lcd.print("Status: Empty   ");
    } else if (waterVal < 300) {
      lcd.print("Status: Low     ");
    } else {
      lcd.print("Status: Full!   ");
    }
  }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("Mode Toggle Sys");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(1500);
  lcd.clear();

  updateDisplay();
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    static int buttonState = HIGH;
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        displayMode = !displayMode;
        lcd.clear();
        updateDisplay();
      }
    }
  }
  lastButtonState = reading;

  unsigned long currentMillis = millis();
  if (currentMillis - lastSensorReadTime >= sensorInterval) {
    lastSensorReadTime = currentMillis;
    updateDisplay();
  }
}