
#include "RoveDynamixel.h"

Dynamixel test1, test3;

void printError(uint8_t errno) {
  if (errno == DYNAMIXEL_ERROR_SUCCESS) return;

  if ((errno & DYNAMIXEL_ERROR_VOLTAGE) !=0) {
    Serial.println("Whoops: DYNAMIXEL_ERROR_VOLTAGE!");
  }

  if ((errno & DYNAMIXEL_ERROR_ANGLE_LIMIT) !=0) {
    Serial.println("Whoops: DYNAMIXEL_ERROR_ANGLE_LIMIT!");
  }

  if ((errno & DYNAMIXEL_ERROR_OVERHEATING) !=0) {
    Serial.println("Whoops: DYNAMIXEL_ERROR_OVERHEATING!");
  }

  if ((errno & DYNAMIXEL_ERROR_RANGE) !=0) {
    Serial.println("Whoops: DYNAMIXEL_ERROR_RANGE!");
  }

  if ((errno & DYNAMIXEL_ERROR_CHECKSUM) !=0) {
    Serial.println("Whoops: DYNAMIXEL_ERROR_CHECKSUM!");
  }

  if ((errno & DYNAMIXEL_ERROR_OVERLOAD) !=0) {
    Serial.println("Whoops: DYNAMIXEL_ERROR_OVERLOAD!");
  }

  if ((errno & DYNAMIXEL_ERROR_UNKNOWN) !=0) {
    Serial.println("Whoops: DYNAMIXEL_ERROR_UNKNOWN!");
  }
}

void setup()
{ delay(2000);
  // put your setup code here, to run once:
  /*test1.id = 0x01;
  test1.type = MX;
  test1.uart = &Serial4;

  test3.id = 0x03;
  test3.type = MX;
  test3.uart = &Serial4;*/
  
  //memcpy(&Serial4, &test.uart, sizeof(HardwareSerial));
  
  DynamixelInit(&test1, MX, 0x01, 4, 57600);
  DynamixelInit(&test3, MX, 0x03, 4, 57600);
  
  Serial.begin(9600);
  //Serial4.begin(57600);
  Serial.println("Start");
  
  DynamixelSetMode(test1, Wheel);
  DynamixelSetMode(test3, Wheel);
  //DynamixelSetId(test1, 3);
}

void loop()
{
  byte voltage =0, temperature=0, pingError=0;
  uint16_t load=0, presentPos=0, presentSpeed=0;
  DynamixelMode myMode;
  
  
  Serial.println("Start");
  DynamixelGetVoltage(test1, &voltage);
  DynamixelGetLoad(test1, &load);
  DynamixelGetPresentPosition(test1, &presentPos);
  DynamixelGetPresentSpeed(test1, &presentSpeed);
  DynamixelGetTemperature(test1, &temperature);
  DynamixelGetMode(test1, &myMode);
  
  Serial.print("Mode:\t");
  Serial.println(myMode);
  Serial.print("Load:\t");
  Serial.println(load);
  Serial.print("Volts:\t");
  Serial.println(voltage/10.0);
  Serial.print("Temp:\t");
  Serial.println(temperature);
  Serial.print("Pos:\t");
  Serial.println(presentPos);
  Serial.print("Speed:\t");
  Serial.println(presentSpeed);
  
  pingError = DynamixelPing(test1);
  Serial.print("Ping:\t");
  Serial.println(pingError);
  
  Serial.println();
  /*
  for (int i=0; i<3; i++) {
    DynamixelTurnAt(test1, 1023, 1023);
    DynamixelTurnAt(test3, 1023, 1023);
    delay(1000);
    DynamixelTurnAt(test1, 1023, 0);
    DynamixelTurnAt(test3, 1023, 0);
    delay(1000);
  }
  for (int i=0; i<3; i++) {
    DynamixelTurnAt(test1, 1023, 2047);
    DynamixelTurnAt(test3, 1023, 2047);
    delay(1000);
    DynamixelTurnAt(test1, 1023, 1024);
    DynamixelTurnAt(test3, 1023, 1024);
    delay(1000);
  }
  */
  /*
  printError(DynamixelMoveTo(test3, 1024));
  printError(DynamixelMoveTo(test1, 1024));
  delay(1000);
  printError(DynamixelMoveTo(test3, 3096));
  printError(DynamixelMoveTo(test1, 3096));
  delay(1000);
  */
  Serial.println();
  delay(500);
}
