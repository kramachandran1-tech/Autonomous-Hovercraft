#include <bluefruit.h>
#include <ctype.h>


const uint8_t ENC_KEY[] = { 0x4A, 0x91, 0xC3, 0x7E, 0x2D, 0x88, 0xF1, 0x56 };
#define ENC_KEY_LEN 8

void decryptBuffer(uint8_t* data, uint8_t len, uint8_t keyOffset) {
  for (uint8_t i = 0; i < len; i++) {
    data[i] ^= ENC_KEY[(i + keyOffset) % ENC_KEY_LEN];
  }
}


#define KEY_COUNT 10

const uint8_t rollingKeys[KEY_COUNT] = {
  0x12, 0x34, 0x56, 0x78, 0x9A,
  0xBC, 0xDE, 0xF0, 0xA5, 0x5A
};

int setSpeed = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);

  unsigned long start = millis();
  while (!Serial && millis() - start < 3000) {}

  Serial.println("Scanning for encrypted BLE advertisements...");

  Bluefruit.begin();
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.start(0);
}

void loop() {}

/* ================= SCAN CALLBACK ================= */

void scan_callback(ble_gap_evt_adv_report_t* report)
{
  uint8_t* p = report->data.p_data;
  uint8_t len = report->data.len;

  uint8_t index = 0;

  while (index < len) {
    uint8_t fieldLen = p[index];
    if (fieldLen == 0) break;

    uint8_t fieldType = p[index + 1];

    if (fieldType == 0xFF) {
      uint8_t dataLen = fieldLen - 1;
      uint8_t* encData = &p[index + 2];

      uint8_t encBuf[32];
      memcpy(encBuf, encData, dataLen);

      for (uint8_t k = 0; k < KEY_COUNT; k++) {
        uint8_t decBuf[32];
        memcpy(decBuf, encBuf, dataLen);

        decryptBuffer(decBuf, dataLen, rollingKeys[k]);
        decBuf[dataLen] = 0;

        if (strncmp((char*)decBuf, "RXHCGCS", 7) == 0) {
          char c1 = decBuf[7];
          char c2 = decBuf[8];
          char c3 = decBuf[9];
          char c4 = decBuf[10];

          if (isdigit(c1) && isdigit(c2) && isdigit(c3) && isdigit(c4)) {
            setSpeed =
              (c1 - '0') * 1000 +
              (c2 - '0') * 100 +
              (c3 - '0') * 10 +
              (c4 - '0');

            Serial.print("Decrypted message: ");
            Serial.println((char*)decBuf);

            Serial.print("Received speed: ");
            Serial.println(setSpeed);

            Serial1.println(setSpeed);

            Bluefruit.Scanner.resume();
            return;
          }
        }
      }
    }

    index += fieldLen + 1;
  }

  Bluefruit.Scanner.resume();
}
