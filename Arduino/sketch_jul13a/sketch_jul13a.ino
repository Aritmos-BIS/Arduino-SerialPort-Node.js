#include <SPI.h>         // incluye libreria bus SPI
#include <MFRC522.h>     // incluye libreria especifica para MFRC522
#include <Wire.h>        // libreria de comunicacion por I2C
#include <LCD.h>         // libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>     // libreria para LCD por I2C

#define NOTE_DS8 4978
#define BUTTON_PIN  7

const int buzzerPin = 8; // Declarar el pin del buzzer
#define RST_PIN  9       // constante para referenciar pin de reset
#define SS_PIN  10       // constante para referenciar pin de slave select

MFRC522 mfrc522(SS_PIN, RST_PIN);    // crea objeto mfrc522 enviando pines de slave select y reset
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

bool isFirstRead = true;

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

  lcd.setBacklightPin(3, POSITIVE); // puerto P3 de PCF8574 como positivo
  lcd.setBacklight(HIGH); // habilita iluminacion posterior de LCD
  lcd.begin(16, 2); // 16 columnas por 2 lineas para LCD 1602A
  lcd.clear(); // limpia pantalla
  pinMode(buzzerPin, OUTPUT); // Configurar el pin del buzzer como salida
}

void loop() {
  String printName;
  if (!mfrc522.PICC_IsNewCardPresent()) // si no hay una tarjeta presente retorna al loop esperando por una tarjeta
    return;              
  
  if (!mfrc522.PICC_ReadCardSerial()) // si no puede obtener datos de la tarjeta
    return;              // retorna al loop esperando por otra tarjeta

  Serial.println();       // nueva linea
  
  // Obtener y enviar el nombre correspondiente al puerto serial
  String name = getNameFromUID(mfrc522.uid.uidByte, mfrc522.uid.size);
  int intName = name.toInt(); // Variable para almacenar el nombre a imprimir
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
    default:
      printName = "Monarc";
      break;
    }
  if (isFirstRead) {
    Serial.print("Arimal 1: ");
    lcd.setCursor(0, 0); // ubica cursor en columna 0 y linea 0
    lcd.clear();
    lcd.print("J1: " + printName); // escribe el texto
  } else {
    Serial.print("Arimal 2: ");
    lcd.setCursor(0, 1); // ubica cursor en columna 0 y linea 1
    lcd.print("J2: " + printName); // escribe el texto
  }

  Serial.println(name);
  mfrc522.PICC_HaltA();   // detiene comunicacion con tarjeta
  tone(buzzerPin, 1000); // Reproduce un tono de 1000Hz
  delay(50); // Espera 500 milisegundos
  noTone(buzzerPin); // Apaga el buzzer

  isFirstRead = !isFirstRead; // Cambiar el estado para la próxima lectura
}
