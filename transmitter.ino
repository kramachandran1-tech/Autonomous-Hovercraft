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

int setSpeed = 1500;
char plainMsg[16];
uint8_t encMsg[16];
uint8_t msgLen;

/* ================= MODES ================= */

enum TxMode {
  MODE_IDLE,
  MODE_STOP,
  MODE_RAMP
};

TxMode currentMode = MODE_IDLE;

/* ================= FUNCTIONS ================= */

void updateRollingKey() {
  if (millis() - lastKeyChange >= KEY_INTERVAL_MS) {
    lastKeyChange = millis();
    keyIndex = (keyIndex + 1) % KEY_COUNT;
  }
}

void sendAdvertisement() {
  Bluefruit.Advertising.stop();
  Bluefruit.Advertising.clearData();

  sprintf(plainMsg, "RXHCGCS%04d", setSpeed);
  msgLen = strlen(plainMsg);

  memcpy(encMsg, plainMsg, msgLen);
  encryptBuffer(encMsg, msgLen, rollingKeys[keyIndex]);

  Bluefruit.Advertising.addManufacturerData(encMsg, msgLen);
  Bluefruit.Advertising.start(1);
}

/* ================= SETUP ================= */

void setup() {
  Bluefruit.begin();
  Bluefruit.setName("CPB-TX");

  pinMode(PIN_BUTTON1, INPUT_PULLDOWN); // Button A
  pinMode(PIN_BUTTON2, INPUT_PULLDOWN); // Button B

  lastKeyChange = millis();
}

/* ================= LOOP ================= */

void loop() {
  updateRollingKey();

  static bool lastA = false;
  static bool lastB = false;

  bool aPressed = digitalRead(PIN_BUTTON1);
  bool bPressed = digitalRead(PIN_BUTTON2);

  /* ---- Button A: toggle STOP / IDLE ---- */
  if (aPressed && !lastA) {
    if (currentMode == MODE_STOP) {
      currentMode = MODE_IDLE;
      Bluefruit.Advertising.stop();
    } else {
      currentMode = MODE_STOP;
      setSpeed = 0000;
      sendAdvertisement();
    }
  }

  /* ---- Button B: start ramp ---- */
  if (bPressed && !lastB) {
    currentMode = MODE_RAMP;
    setSpeed = 1500;
  }

  lastA = aPressed;
  lastB = bPressed;

  /* ---- Ramp behavior ---- */
  if (currentMode == MODE_RAMP) {
    sendAdvertisement();

    setSpeed += 10;
    if (setSpeed > 1800) {
      setSpeed = 1500;
    }

    delay(100); // 10 Hz
  }
}
