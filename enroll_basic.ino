#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <EEPROM.h> // Library to save data permanently

// LCD: RS=4, EN=5, D4=6, D5=7, D6=8, D7=9
LiquidCrystal lcd(4, 5, 6, 7, 8, 9);

// Fingerprint: RX=2, TX=3
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Each name gets 15 bytes in EEPROM memory
const int nameSize = 15;

uint8_t id;
char personName[nameSize];

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  finger.begin(57600);
  
  lcd.print("Enroll & Save");
  delay(1000);
}

// Function to save name to EEPROM at a specific ID address
void saveNamePermanently(int id, char *name) {

 int addr = (id-1) * nameSize;

  for (int i = 0; i < nameSize; i++) {

    if (i < strlen(name)) {
      EEPROM.update(addr + i, name[i]);
    } 
    else {
      EEPROM.write(addr + i, '\0');
    }

  }

}

void loop() {
  Serial.println("\nStep 1: Type ID (1-50):");
  lcd.clear(); lcd.print("Enter ID on PC");

  id = 0;
  while (id == 0) {
    if (Serial.available()) id = Serial.parseInt();
  }

  Serial.println("Step 2: Type Name:");
  lcd.clear(); lcd.print("Enter Name on PC");
  
 while(Serial.available()) Serial.read();

int index = 0;

while(index < nameSize - 1){
  if(Serial.available()){
    char c = Serial.read();

    if(c == '\n') break;

    personName[index++] = c;
  }
}

personName[index] = '\0';

  lcd.clear();
  lcd.print("ID:"); lcd.print(id);
  lcd.setCursor(0, 1); lcd.print((char*)personName);
  
  if (getFingerprintEnroll()) {
    saveNamePermanently(id, personName);
    Serial.println("Name saved to Database!");
  }
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  lcd.clear(); lcd.print("Place Finger...");
  while (p != FINGERPRINT_OK) p = finger.getImage();

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) return false;

  lcd.clear(); lcd.print("Remove Finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) p = finger.getImage();

  lcd.clear(); lcd.print("Place Again");
  p = -1;
  while (p != FINGERPRINT_OK) p = finger.getImage();

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) return false;

  if (finger.createModel() != FINGERPRINT_OK) return false;
  if (finger.storeModel(id) != FINGERPRINT_OK) return false;

  lcd.clear();
  lcd.print("Success!");
  lcd.setCursor(0, 1);
  lcd.print("Database Updated");
  delay(3000);
  return true;
}