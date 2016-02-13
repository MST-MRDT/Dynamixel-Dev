// RoveDynamixel.cpp
// Author: Gbenga Osibodu

#include "RoveDynamixel.h"

void DynamixelInit(Dynamixel* dyna, DynamixelType type, uint8_t id, uint8_t uartIndex, int baud) {
  dyna -> type = type;
  dyna -> id = id;
  dyna -> uart = roveBoard_UART_open(uartIndex, baud);
}

void DynamixelSendPacket(Dynamixel dyna, uint8_t length, uint8_t* instruction) {
  int i;
  uint8_t checksum;
  
  checksum = dyna.id + (length + 1);
  for(i=0; i < length; i++) {
    checksum += instruction[i];
  }
  checksum = ~checksum;
  
  uint8_t packet[length + 5];
  
  packet[0] = 0xFF;
  packet[1] = 0xFF;
  packet[2] = dyna.id;
  packet[3] = length + 1;
  memcpy(&(packet[4]), instruction, length);
  packet[length + 4] = checksum;
  
  roveBoard_UART_write(dyna.uart, packet, length + 5);
  wait(50);
  roveBoard_UART_read(dyna.uart, NULL, length + 5);
}

uint8_t DynamixelGetReturnPacket(Dynamixel dyna, uint8_t* data, size_t dataSize) {
  // To be fixed
  uint8_t id, length, error;
  uint8_t temp1, temp2;
  
  
  if(roveBoard_UART_available(dyna.uart) == true){
  roveBoard_UART_read(dyna.uart, &temp2, 1);
  
  while(roveBoard_UART_available(dyna.uart) == true) { 
    temp1 = temp2;
    roveBoard_UART_read(dyna.uart, &temp2, 1);
    if (temp1 == 255 && temp2 == 255) {
      roveBoard_UART_read(dyna.uart, &id, 1);
      roveBoard_UART_read(dyna.uart, &length, 1);
      roveBoard_UART_read(dyna.uart, &error, 1);
      if (dataSize + 2 != length) {
        roveBoard_UART_read(dyna.uart, NULL, length-2);
        return (error & DYNAMIXEL_ERROR_UNKNOWN);
      } else {
        roveBoard_UART_read(dyna.uart, data, length-2);
        roveBoard_UART_read(dyna.uart, NULL, 1);
        return error;
      }
    }
  }}
  return DYNAMIXEL_ERROR_UNKNOWN;
}

uint8_t DynamixelGetError(Dynamixel dyna) {
  return DynamixelGetReturnPacket(dyna, NULL, 0);
}

uint8_t DynamixelPing(Dynamixel dyna) {
  uint8_t msgLength = 1;
  uint8_t command = DYNAMIXEL_PING;
  
  DynamixelSendPacket(dyna, msgLength, &command);
  wait(TXDELAY);
  return DynamixelGetError(dyna);
}

uint8_t DynamixelSetId(Dynamixel* dyna, uint8_t id) {
  uint8_t msgLength = 3;
  uint8_t commands[msgLength];
  
  commands[0] = DYNAMIXEL_WRITE_DATA;
  commands[1] = DYNAMIXEL_ID;
  commands[2] = id;
  
  DynamixelSendPacket(*dyna, msgLength, commands);
  
  dyna -> id = id;
  
  wait(TXDELAY);
  return DynamixelGetError(*dyna);
}

uint8_t DynamixelRotateJoint(Dynamixel dyna, uint16_t position) {
  uint8_t msgLength = 4;
  uint8_t commands[msgLength];
  
  commands[0] = DYNAMIXEL_WRITE_DATA;
  commands[1] = DYNAMIXEL_GOAL_POSITION_L;
  commands[2] = position & 0x00FF;
  commands[3] = position >> 8;
  
  DynamixelSendPacket(dyna, msgLength, commands);
  
  wait(TXDELAY);
  return DynamixelGetError(dyna);
}

uint8_t DynamixelSpinWheel(Dynamixel dyna, uint16_t position, uint16_t speed) {
  uint8_t msgLength = 6;
  uint8_t commands[msgLength];
  
  commands[0] = DYNAMIXEL_WRITE_DATA;
  commands[1] = DYNAMIXEL_GOAL_POSITION_L;
  commands[2] = position & 0x00FF;
  commands[3] = position >> 8;
  commands[4] = speed & 0x00FF;
  commands[5] = speed >> 8;
  
  DynamixelSendPacket(dyna, msgLength, commands);
  
  wait(TXDELAY);
  return DynamixelGetError(dyna);
}

uint8_t DynamixelSetBaudRate(Dynamixel dyna, uint8_t baudByte) {
  uint8_t msgLength = 3;
  uint8_t commands[msgLength];
  
  commands[0] = DYNAMIXEL_WRITE_DATA;
  commands[1] = DYNAMIXEL_BAUD_RATE;
  commands[2] = baudByte;
  
  DynamixelSendPacket(dyna, msgLength, commands);
  
  wait(TXDELAY);
  return DynamixelGetError(dyna);
}

uint8_t DynamixelSetStatusReturnLevel(Dynamixel dyna, uint8_t level) {
  uint8_t msgLength = 3;
  uint8_t commands[msgLength];
  
  commands[0] = DYNAMIXEL_WRITE_DATA;
  commands[1] = DYNAMIXEL_RETURN_LEVEL;
  commands[2] = level;
  
  DynamixelSendPacket(dyna, msgLength, commands);
  
  wait(TXDELAY);
  return DynamixelGetError(dyna);
}

uint8_t DynamixelSetMode(Dynamixel dyna, DynamixelMode mode) {
  uint8_t msgLength = 6;
  uint8_t commands[msgLength];
  uint8_t ccwHighByte;
  
  commands[0] = DYNAMIXEL_WRITE_DATA;
  commands[1] = DYNAMIXEL_CW_ANGLE_LIMIT_L;
  
  switch (mode) {
    case Wheel:
      commands[2] = 0x00;
      commands[3] = 0x00;
      commands[4] = 0x00;
      commands[5] = 0x00;
      break;
    case Joint:
      if (dyna.type == MX) {
        ccwHighByte = 0xFF & MX_HIGH_BYTE_MASK;
      }
      
      if (dyna.type == AX) {
        ccwHighByte = 0xFF & AX_HIGH_BYTE_MASK;
      }
      
      commands[2] = 0x00;
      commands[3] = 0x00;
      commands[4] = 0xFF;
      commands[5] = ccwHighByte;
      break;
    case MultiTurn:
      if (dyna.type == AX)
        return DYNAMIXEL_ERROR_UNKNOWN;
      
      if (dyna.type == MX) {
        ccwHighByte = 0xFF & MX_HIGH_BYTE_MASK;
      }
      
      commands[2] = 0xFF;
      commands[3] = ccwHighByte;
      commands[4] = 0xFF;
      commands[5] = ccwHighByte;
      break;
    default:
      return DYNAMIXEL_ERROR_UNKNOWN;
  }
  
  DynamixelSendPacket(dyna, msgLength, commands);
  
  wait(TXDELAY);
  return DynamixelGetError(dyna);
}

uint8_t DynamixelGetMode(Dynamixel dyna, DynamixelMode* mode) {
  uint8_t msgLength = 3, dataSize = 4, error;
  uint8_t commands[msgLength], buffer[dataSize];
  uint16_t cwAngleLimit, ccwAngleLimit;
  
  commands[0] = DYNAMIXEL_READ_DATA;
  commands[1] = DYNAMIXEL_CW_ANGLE_LIMIT_L;
  commands[2] = dataSize;
  
  DynamixelSendPacket(dyna, msgLength, commands);
  
  wait(TXDELAY);
  error = DynamixelGetReturnPacket(dyna, buffer, dataSize);
  
  cwAngleLimit = buffer[1];
  cwAngleLimit = (cwAngleLimit << 8) | buffer[0];
  ccwAngleLimit = buffer[3];
  ccwAngleLimit = (ccwAngleLimit << 8) | buffer[2];
  
  if (cwAngleLimit == 0 && ccwAngleLimit == 0) {
    *mode = Wheel;
  }
  
  switch (dyna.type) {
    case AX:
      if (cwAngleLimit == 0 && ccwAngleLimit == 0x03FF) {
        *mode = Joint;
      }
      break;
    case MX:
      if (cwAngleLimit == 0 && ccwAngleLimit == 0x0FFF) {
        *mode = Joint;
      }
      if (cwAngleLimit == 0x0FFF && ccwAngleLimit == 0x0FFF) {
        *mode = MultiTurn;
      }
  }
  return error;
}

uint8_t DynamixelGetPresentPosition(Dynamixel dyna, uint16_t* pos) {
  uint8_t msgLength = 3, dataSize = 2, error;
  uint8_t commands[msgLength], buffer[dataSize];
  
  commands[0] = DYNAMIXEL_READ_DATA;
  commands[1] = DYNAMIXEL_PRESENT_POSITION_L;
  commands[2] = dataSize;
  
  DynamixelSendPacket(dyna, msgLength, commands);
  
  wait(TXDELAY);
  error = DynamixelGetReturnPacket(dyna, buffer, dataSize);
  
  *pos = buffer[1];
  *pos = (*pos << 8) | buffer[0];
  
  return  error;
}

uint8_t DynamixelGetPresentSpeed(Dynamixel dyna, uint16_t* speed) {
  uint8_t msgLength = 3, dataSize = 2, error;
  uint8_t commands[msgLength], buffer[dataSize];
  
  commands[0] = DYNAMIXEL_READ_DATA;
  commands[1] = DYNAMIXEL_PRESENT_SPEED_L;
  commands[2] = dataSize;
  
  DynamixelSendPacket(dyna, msgLength, commands);
  
  wait(TXDELAY);
  error = DynamixelGetReturnPacket(dyna, buffer, dataSize);
  
  *speed = buffer[1];
  *speed = (*speed << 8) | buffer[0];
  
  return error;
}

uint8_t DynamixelGetLoad(Dynamixel dyna, uint16_t* load) {
  uint8_t msgLength = 3, dataSize = 2, error;
  uint8_t commands[msgLength], buffer[dataSize];
  
  commands[0] = DYNAMIXEL_READ_DATA;
  commands[1] = DYNAMIXEL_PRESENT_LOAD_L;
  commands[2] = dataSize;
  
  DynamixelSendPacket(dyna, msgLength, commands);
  
  wait(TXDELAY);
  error = DynamixelGetReturnPacket(dyna, buffer, dataSize);
  
  *load = buffer[1];
  *load = (*load << 8) | buffer[0];
  
  return error;
}

uint8_t DynamixelGetVoltage(Dynamixel dyna, uint8_t* voltage) {
  uint8_t msgLength = 3, dataSize = 1;
  uint8_t commands[msgLength];
  
  commands[0] = DYNAMIXEL_READ_DATA;
  commands[1] = DYNAMIXEL_PRESENT_VOLTAGE;
  commands[2] = dataSize;
  
  DynamixelSendPacket(dyna, msgLength, commands);
  
  wait(TXDELAY);
  return DynamixelGetReturnPacket(dyna, voltage, dataSize);
}

uint8_t DynamixelGetTemperature(Dynamixel dyna, uint8_t* temp) {
  uint8_t msgLength = 3, dataSize = 1;
  uint8_t commands[msgLength], buffer[dataSize];
  
  commands[0] = DYNAMIXEL_READ_DATA;
  commands[1] = DYNAMIXEL_PRESENT_TEMPERATURE;
  commands[2] = dataSize;
  
  DynamixelSendPacket(dyna, msgLength, commands);
  
  wait(TXDELAY);
  return DynamixelGetReturnPacket(dyna, temp, dataSize);
}
