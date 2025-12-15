#include <bluefruit.h>

const uint8_t ENC_KEY[] = { 0x4A, 0x91, 0xC3, 0x7E, 0x2D, 0x88, 0xF1, 0x56 };
#define ENC_KEY_LEN 8

void encryptBuffer(uint8_t* data, uint8_t len, uint8_t keyOffset) {
  for (uint8_t i = 0; i < len; i++) {
    data[i] ^= ENC_KEY[(i + keyOffset) % ENC_KEY_LEN];
  }
}

#define KEY_COUNT 10
#define KEY_INTERVAL_MS 2000

const uint8_t rollingKeys[KEY_COUNT] = {
  0x12, 0x34, 0x56, 0x78, 0x9A,
  0xBC, 0xDE, 0xF0, 0xA5, 0x5A
};

uint8_t keyIndex = 0;
unsigned long lastKeyChange = 0;

int setSpeed = 423;
char plainMsg[32];
uint8_t encMsg[32];
uint8_t msgLen;

void printHex(const uint8_t* data, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
}

void updateRollingKey() {
  if (millis() - lastKeyChange >= KEY_INTERVAL_MS) {
    lastKeyChange = millis();
    keyIndex = (keyIndex + 1) % KEY_COUNT;
  }
}

void updateAdvertisement() {
  Bluefruit.Advertising.stop();
  Bluefruit.Advertising.clearData();

  sprintf(plainMsg, "RXHCGCS%d", setSpeed);
  msgLen = strlen(plainMsg);

  memcpy(encMsg, plainMsg, msgLen);

  encryptBuffer(encMsg, msgLen, rollingKeys[keyIndex]);

  Bluefruit.Advertising.addManufacturerData(encMsg, msgLen);
  Bluefruit.Advertising.start();

  Serial.print("Plaintext: ");
  Serial.println(plainMsg);

  Serial.print("Encrypted: ");
  printHex(encMsg, msgLen);
  Serial.println();
  Serial.println("-------------------------");
}

/* ================= SETUP / LOOP ================= */

void setup() {
  Serial.begin(115200);
  Bluefruit.begin();
  Bluefruit.setName("CPB-TX");

  lastKeyChange = millis();
  updateAdvertisement();

  Serial.println("Encrypted beacon started");
}

void loop() {
  updateRollingKey();
  updateAdvertisement();
  delay(100);
}
