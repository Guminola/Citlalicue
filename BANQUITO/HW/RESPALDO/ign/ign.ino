#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(5, 8);  // Configura los pines CE y CSN del módulo nRF24L01
const byte address[6] = "00001"; // Dirección de red del módulo, debe ser igual en el transmisor

const int relay1Pin = 4;  // Pin del relé 1
const int relay2Pin = 5;  // Pin del relé 2

bool ignitionReceived = false;
bool relay1Activated = false;
bool relay2Activated = false;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_HIGH);  // Configura la potencia de la señal
  radio.startListening();

  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);
}

void loop() {
  if (radio.available()) {
    char text[32] = "";  // Buffer para almacenar el paquete recibido
    radio.read(&text, sizeof(text));

    if (strcmp(text, "IGNITION_ON") == 0 && !ignitionReceived) {
      digitalWrite(relay1Pin, HIGH);  // Activa el relé 1
      Serial.println("Ignición encendida - Activa Relé 1");
      ignitionReceived = true;
      relay1Activated = true;
      delay(3000);  // Espera 3 segundos
      digitalWrite(relay1Pin, LOW);  // Desactiva el relé 1
      Serial.println("Relé 1 desactivado");
      relay1Activated = false;

      digitalWrite(relay2Pin, HIGH);  // Activa el relé 2
      Serial.println("Relé 2 activado");
      relay2Activated = true;
      delay(3000);  // Espera 3 segundos
      digitalWrite(relay2Pin, LOW);  // Desactiva el relé 2
      Serial.println("Relé 2 desactivado");
      relay2Activated = false;
    }
  }
}
