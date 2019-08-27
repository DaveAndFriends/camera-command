/******************************************
  PURPOSE: Recieves commands from slider_sender.ino
           and controls a motor based on the info
  Created by: Davis Johnson
  DATE:   August 2018
*******************************************/

#include <AccelStepper.h>
#include <PacketSerial.h>
#include <SoftwareSerial.h>

#define DEBUG 1   //prints debug info to hardward serial port

// Define our maximum and minimum speed in steps per second (scale pot to these)
#define  MAX_SPEED 5000
#define  MIN_SPEED 0.1

#define LimitLeft 6  // Pin 6 connected to Limit switch left
#define LimitRight 7 // Pin 7 connected to Limit switch right

#define DELAY 0   //in case the commands are coming to fast, we can add a delay

//Serial vars
PacketSerial HC12;
SoftwareSerial hc12ss(3, 2); //RX TX

// Define the stepper and the pins it will use
AccelStepper stepper1(1, 4, 5); // 4= step ,5= direction

// packet format
// [step/jog/0, for/back/stop, value]
uint8_t packet[] = {'s', 0, 0};
uint8_t tempBuffer[sizeof(packet)];

boolean isStopped = false;

void setup() {
  //Initialize serial comms
  if (DEBUG) Serial.begin(9600);
  hc12ss.begin(9600);
  HC12.setStream(&hc12ss);
  HC12.setPacketHandler(&onPacketReceived);

  pinMode(LimitLeft, INPUT_PULLUP);
  pinMode(LimitRight, INPUT_PULLUP);
  // The only AccelStepper value we have to set here is the max speeed, which is higher than we'll ever go
  stepper1.setMaxSpeed(10000.0);
  if (DEBUG) Serial.println("Receiver ready");
}

void loop() {
  HC12.update();
  delay(DELAY);
}

void onPacketReceived(const uint8_t* buffer, size_t size) {
  uint8_t tempBuffer[size];
  memcpy(tempBuffer, buffer, size);
  //if(DEBUG) printPacket(tempBuffer);

  if (tempBuffer[0] == 0 || tempBuffer[1] == int('s')) {
    if (!isStopped) {
      stop_motor();
    }
  }
  else if (tempBuffer[1] == int('f')) { //direction right/cw
    if (digitalRead(LimitRight) == LOW) {
      if (!isStopped) {
        stop_motor();
      }
    }
    else if (tempBuffer[0] == int('s')) { //step
      jog_cw(tempBuffer[2]);
    }
  }
  else if (tempBuffer[1] == int('b')) { //direction left/ccw
    if (digitalRead(LimitLeft) == LOW) {
      if (!isStopped) {
        stop_motor();
      }
    }
    else if (tempBuffer[0] == int('s')) { //step back
      jog_ccw(tempBuffer[2]);
    }
  }
}


void printPacket(uint8_t packet[]) {
  Serial.print(char(packet[0]));
  Serial.print(",");
  Serial.print(char(packet[1]));
  Serial.print(",");
  Serial.print(int(packet[2]));
  Serial.println();
}

void stop_motor() {
  if (DEBUG) Serial.println("STOP");
  stepper1.setSpeed(0);
  stepper1.runSpeed();
  isStopped = true;
}

void jog_cw(int mtr_speed) {
  int current_speed = map(mtr_speed, 0, 254, MIN_SPEED, MAX_SPEED);
  if (DEBUG) Serial.println("Step fwd @ " + String(current_speed));
  stepper1.setSpeed(current_speed);
  stepper1.runSpeed();
  isStopped = false;
}

void jog_ccw(int mtr_speed) {
  int current_speed = -1 * map(mtr_speed, 0, 254, MIN_SPEED, MAX_SPEED);
  if (DEBUG) Serial.println("Step back @ " + String(current_speed));
  stepper1.setSpeed(current_speed);
  stepper1.runSpeed();
  isStopped = false;
}


