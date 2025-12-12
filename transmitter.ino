  #include <bluefruit.h>

int setSpeed = 000;
char msg[32];

void updateAdvertisement() {
  // Stop advertising 
  Bluefruit.Advertising.stop();

  // Clear old  data
  Bluefruit.Advertising.clearData();

  // Rebuild the message
  sprintf(msg, "KREESHA%d", setSpeed);

  Bluefruit.Advertising.addManufacturerData((uint8_t*)msg, strlen(msg));

  Bluefruit.Advertising.start();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Bluefruit.begin();
  Bluefruit.setName("CPB-TX");

  updateAdvertisement();

  Serial.println("Advertising started.");
}

void loop() {

  delay(1000);

  setSpeed = 100;   

  Serial.print("Updating speed to: ");
  Serial.println(setSpeed);

  updateAdvertisement();   

  delay(1000);

  setSpeed = 350;  

  Serial.print("Updating speed to: ");
  Serial.println(setSpeed);

  updateAdvertisement();   


}
