#include "Commander.h"

float debugValue;
Commander command = Commander(Serial);

void doDebugValue(char *cmd) {
    command.scalar(&debugValue, cmd);
    Serial.println(debugValue);
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  command.add('T', doDebugValue, "debug value");

}

void loop() {
  // put your main code here, to run repeatedly:

  command.run();
  delay(20);
}
