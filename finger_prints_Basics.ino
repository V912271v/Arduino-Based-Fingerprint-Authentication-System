#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// LCD Setup
LiquidCrystal lcd(4, 5, 6, 7, 8, 9);

// Fingerprint Sensor
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const int relayPin = 13;
const int nameSize = 15;

char foundName[nameSize];

void setup() {

  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  lcd.begin(16,2);
  lcd.print("System Loading");

  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found");
  }
  else {
    Serial.println("Sensor not found");
    while(1);
  }

  delay(1500);
  lcd.clear();
}


// Function to read name from EEPROM
void readNameFromEEPROM(int id, char *name) {

  int addr = (id-1) * nameSize;

  for(int i=0;i<nameSize;i++) {

    char c = EEPROM.read(addr + i);

    if(c == '\0' || c == 255) {
      name[i] = '\0';
      return;
    }

    name[i] = c;
  }

  name[nameSize-1] = '\0';
}


void loop() {

  lcd.setCursor(0,0);
  lcd.print("Scan Finger... ");

  getFingerprintID();

  delay(50);
}



uint8_t getFingerprintID() {

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return p;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return p;

  p = finger.fingerFastSearch();

  if (p != FINGERPRINT_OK) {

    lcd.clear();
    lcd.print("Access Denied");

    delay(2000);
    lcd.clear();

    return p;
  }


  // MATCH FOUND

  readNameFromEEPROM(finger.fingerID, foundName);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Access Granted");

  lcd.setCursor(0,1);
  lcd.print("Hi ");
  lcd.print(foundName);

  digitalWrite(relayPin,HIGH);

  delay(2000);

  digitalWrite(relayPin,LOW);

  delay(1500);

  lcd.clear();

  return finger.fingerID;
}