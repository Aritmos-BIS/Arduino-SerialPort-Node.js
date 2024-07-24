#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define NOTE_DS8 4978
#define BUTTON_PIN  7

const int buzzerPin = 8; // Declarar el pin del buzzer
const int led1Pin = 4;   // LED para jugador 1
const int led2Pin = 2;   // LED para jugador 2
#define RST_PIN  9       // constante para referenciar pin de reset
#define SS_PIN  10       // constante para referenciar pin de slave select

MFRC522 mfrc522(SS_PIN, RST_PIN);    // crea objeto mfrc522 enviando pines de slave select y reset
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

bool isFirstRead = true;
bool battleActive = false;
int player1HP = 100;
int player2HP = 100;

String getNameFromUID(byte *uid, byte uidSize) {
  const byte uid1[] = {0xE3, 0x76, 0x97, 0x2F}; // UID de Axo
  const byte uid2[] = {0x33, 0xE0, 0x02, 0x03}; // UID de Cacti
  const byte uid3[] = {0x99, 0xCE, 0x03, 0x03}; // UID de Monarc
  const byte uid4[] = {0x3A, 0xB4, 0x02, 0x03}; // UID de Ari

  if (memcmp(uid, uid1, uidSize) == 0) {
    return "2"; //id 2 es Axo
  } else if (memcmp(uid, uid2, uidSize) == 0) {
    return "3"; //id 3 es Cacti
  } else if (memcmp(uid, uid3, uidSize) == 0) {
    return "4"; //id 4 es Monarc
  } else if (memcmp(uid, uid4, uidSize) == 0) {
    return "1"; //id 1 es Ari
  } else {
    return "Unknown";
  }
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16, 2);
  lcd.clear();

  pinMode(buzzerPin, OUTPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    Serial.println("Comando recibido: " + command); // Debugging

    if (command == "ACTIVE_BATTLE") {
      battleActive = true;
      Serial.println("Batalla Activa");
    } else if (command == "NO_ACTIVE_BATTLE") {
      battleActive = false;
      Serial.println("Batalla Inactiva");
      digitalWrite(led1Pin, LOW);  // Apagar el LED del jugador 1
      digitalWrite(led2Pin, LOW);  // Apagar el LED del jugador 2
      lcd.clear();// esto se va a cambiar por una animación y / o melodia de victoria
    }else if (command == "BATTLE_NOT_FOUND") {
      battleActive = false;
      Serial.println("Batalla Inactiva");
      digitalWrite(led1Pin, LOW);  // Apagar el LED del jugador 1
      digitalWrite(led2Pin, LOW);  // Apagar el LED del jugador 2
      lcd.setCursor(0, 0);
      lcd.print("Batalla no");
      lcd.setCursor(0, 1);
      lcd.print("encontrada");
      delay(3000);
      lcd.clear();// esto se va a cambiar por una animación y / o melodia de victoria
    } else if (command.startsWith("Player1HP:")) {
      player1HP = command.substring(command.indexOf(':') + 1).toInt();
      Serial.println("Player1HP: " + String(player1HP)); // Debugging
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("J1: " + String(player1HP) + " ");
    } else if (command.startsWith("Player2HP:")) {
      player2HP = command.substring(command.indexOf(':') + 1).toInt();
      Serial.println("Player2HP: " + String(player2HP)); // Debugging
      lcd.setCursor(0, 1);
      lcd.print("J2: " + String(player2HP) + " ");
    } else {
      Serial.println("Comando desconocido");
    }
  }

  if (battleActive) {
    return;
  }

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println();
  String name = getNameFromUID(mfrc522.uid.uidByte, mfrc522.uid.size);
  String printName;

  int intName = name.toInt();
  switch (intName) {
    case 1:
      printName = "Ari";
      break;
    case 2:
      printName = "Axo";
      break;
    case 3:
      printName = "Cacti";
      break;
    case 4:
      printName = "Monarc";
      break;
    default:
      printName = "Unknown";
  }

  if (isFirstRead) {
    Serial.print("Arimal 1: ");
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("J1: " + printName);
    digitalWrite(led1Pin, HIGH); // Enciende el LED del jugador 1
  } else {
    Serial.print("Arimal 2: ");
    lcd.setCursor(0, 1);
    lcd.print("J2: " + printName);
    
    digitalWrite(led2Pin, HIGH); // Enciende el LED del jugador 2
  }

  Serial.println(name);
  mfrc522.PICC_HaltA();
  tone(buzzerPin, 1000);
  delay(50);
  noTone(buzzerPin);

  isFirstRead = !isFirstRead;
}
