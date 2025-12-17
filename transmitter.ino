#include <bluefruit.h>

/* ================= ENCRYPTION ================= */

const uint8_t ENC_KEY[] = { 0x4A, 0x91, 0xC3, 0x7E, 0x2D, 0x88, 0xF1, 0x56 };
#define ENC_KEY_LEN 8

void encryptBuffer(uint8_t* data, uint8_t len, uint8_t keyOffset) {
  for (uint8_t i = 0; i < len; i++) {
    data[i] ^= ENC_KEY[(i + keyOffset) % ENC_KEY_LEN];
  }
}

/* ================= ROLLING KEYS ================= */

#define KEY_COUNT 10
#define KEY_INTERVAL_MS 2000

const uint8_t rollingKeys[KEY_COUNT] = {
  0x12, 0x34, 0x56, 0x78, 0x9A,
  0xBC, 0xDE, 0xF0, 0xA5, 0x5A
};

uint8_t keyIndex = 0;
unsigned long lastKeyChange = 0;

/* ================= PAYLOAD ================= */

int setSpeed = 1500;     // <-- CHANGE THIS VALUE
char plainMsg[16];
uint8_t encMsg[16];
uint8_t msgLen;

/* ================= FUNCTIONS ================= */

void updateRollingKey() {
  if (millis() - lastKeyChange >= KEY_INTERVAL_MS) {
    lastKeyChange = millis();
    keyIndex = (keyIndex + 1) % KEY_COUNT;
  }
}

void updateAdvertisement() {
  Bluefruit.Advertising.stop();
  Bluefruit.Advertising.clearData();

  // Format expected by receiver: RXHCGCS1500
  sprintf(plainMsg, "RXHCGCS%d", setSpeed);
  msgLen = strlen(plainMsg);

  memcpy(encMsg, plainMsg, msgLen);
  encryptBuffer(encMsg, msgLen, rollingKeys[keyIndex]);

  Bluefruit.Advertising.addManufacturerData(encMsg, msgLen);
  Bluefruit.Advertising.start();
}

/* ================= SETUP ================= */

void setup() {
  Bluefruit.begin();
  Bluefruit.setName("CPB-TX");

  lastKeyChange = millis();
  updateAdvertisement();
}

/* ================= LOOP ================= */

void loop() {
  updateRollingKey();
  updateAdvertisement();


  setSpeed += 10;
  if (setSpeed > 1800) setSpeed = 1500;

  delay(100);  // 10 Hz beacon
}
