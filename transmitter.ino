#include <bluefruit.h>
BLEUart bleuart;
void setup() {
  Serial.begin(115200);

  // Wait up to 5 seconds for Serial Monitor instead of freezing forever
  unsigned long start = millis();
  while (!Serial && millis() - start < 5000) { }

  Serial.println("Serial is working — Starting BLE...");

  Bluefruit.begin();
  Serial.println("BLE initialized.");
  bleuart.begin();

  Bluefruit.setName("CPB-TX");

  uint8_t data[] = { 'H', 'E', 'L', 'L', 'O' };
  Bluefruit.Advertising.addManufacturerData(data, sizeof(data));
  Bluefruit.Advertising.addService(bleuart);

  Bluefruit.Advertising.start();
  Serial.println("Advertising started.");
}

void loop() {
  Serial.println("Loop running...");
  delay(1000);
  bleuart.write('11222');
  delay(100);
}
