#include <bluefruit.h>

void setup() {
  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 3000) {}

  Serial.println("Scanning for BLE messages containing 'HEELOO'...");

  Bluefruit.begin();
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.start(0);  
}

void loop() {
}

void scan_callback(ble_gap_evt_adv_report_t* report)
{
  // Convert advertisement to  string
  char msg[64];
  uint8_t len = min(report->data.len, (uint8_t)63);

  for (uint8_t i = 0; i < len; i++) {
    char c = report->data.p_data[i];
    msg[i] = (c >= 32 && c <= 126) ? c : '.';  
  }
  msg[len] = 0;  // null terminator

  // Check if the message contains KEYWORD: KREESHA
  if (strstr(msg, "KREESHA")) {
    //Serial.print("Found message with 'KREESHA': ");
    //Serial.println(msg);
    char c1 = msg[9];
    char c2 = msg[10];
    char c3 = msg[11];

    int setSpeed = (c1 - '0') * 100 + (c2 - '0') * 10 + (c3 - '0');

    Serial.print("Set speed: ");
    Serial.println(setSpeed);

  }

  Bluefruit.Scanner.resume(); 
}
