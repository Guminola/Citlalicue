#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(5, 8);  // Configura los pines CE y CSN del módulo nRF24L01
const byte address[6] = "00001"; // Dirección de red del módulo, debe ser igual en el receptor

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C de la pantalla LCD

const int button1Pin = 14;  // Pin del botón 1
const int button2Pin = 13;  // Pin del botón 2

bool communicationEstablished = false;
bool ignitionRequested = false;
bool ignitionCompleted = false;
unsigned long ignitionStartTime = 0;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Waiting...");

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);

  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
}

void loop() {
  if (!communicationEstablished) {
    if (checkCommunication()) {
      communicationEstablished = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Comm OK");
      delay(5000); // Esperar 5 segundos
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Relay 1: OFF");
      lcd.setCursor(0, 1);
      lcd.print("Relay 2: OFF");
      lcd.setCursor(13, 0);
      lcd.print("ARM");
    }
  }

  if (communicationEstablished) {
    bool button1Pressed = digitalRead(button1Pin) == LOW;
    bool button2Pressed = digitalRead(button2Pin) == LOW;

    if (button1Pressed && button2Pressed && !ignitionRequested) {
      ignitionRequested = true;
      ignitionStartTime = millis();
    } else if (!button1Pressed || !button2Pressed) {
      ignitionRequested = false;
    }

    if (ignitionRequested && !ignitionCompleted) {
      unsigned long elapsedTime = millis() - ignitionStartTime;
      int countdown = (elapsedTime < 5000) ? (5 - (elapsedTime / 1000)) : 0;


      lcd.setCursor(13, 1);
      lcd.print(countdown);

      if (countdown == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sending ignition");
        sendConfirmation("IGNITION_ON");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Fire!");
        delay(2000); // Mostrar "Fire!" por 2 segundos
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Relay 1: OFF");
        lcd.setCursor(0, 1);
        lcd.print("Relay 2: OFF");
        ignitionCompleted = true;
        lcd.setCursor(13, 0);
        lcd.print("FIRE");
        lcd.setCursor(13, 1);
        lcd.print("    ");
      }
    } else {
      ignitionCompleted = false;
      lcd.setCursor(13, 0);
      lcd.print("ARM");
      lcd.setCursor(13, 1);
      lcd.print("5 ");
    }
  }
}

bool checkCommunication() {
  radio.stopListening();
  radio.openWritingPipe(address);
  const char *message = "Hello";
  bool success = radio.write(message, strlen(message));
  radio.startListening();
  return success;
}

void sendConfirmation(const char *message) {
  radio.stopListening();
  radio.openWritingPipe(address);
  radio.write(message, strlen(message));
  radio.startListening();
}