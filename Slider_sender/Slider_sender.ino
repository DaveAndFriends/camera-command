/******************************************
  PURPOSE: Sends commands to slider_recv.ino
           using input from several buttons
           and a potentiometer
  Created by: Davis Johnson
  DATE:   August 2018
*******************************************/

#include <PacketSerial.h>
#include <SoftwareSerial.h>

#define DEBUG 1   //prints debug info to hardward serial port
#define DELAY 10  //delay in each loop for sending

// Define our three input button pins
#define  STOP_PIN  10           //attach stop button to this pin
#define  LEFT_JOG_PIN 11        //attach left jog button to this pin
#define  RIGHT_JOG_PIN 12       //attach right jog button to this pin

//Used to hold values of digital reads
int left_read, left_jog_read, right_read, right_jog_read, stop_read;

// Define our analog pot input pin
#define  SPEED_PIN 0

// Define our maximum and minimum speed in steps per second (scale pot to these)
#define  MAX_SPEED 5000
#define  MIN_SPEED 0.1

//Serial vars
PacketSerial HC12;
SoftwareSerial hc12ss(3, 2); //RX TX

// Packet format
// [step/jog/0, for/back/stop, speed]
uint8_t packet[] = {'0', 's', 0};
uint8_t lastPacket[sizeof(packet)];

int maxAnalogRead;
boolean allButtonsOpen = true;
boolean sendPacket = false;

void setup() {

  //Initialize serial comms
  if (DEBUG) Serial.begin(9600);
  hc12ss.begin(9600);
  HC12.setStream(&hc12ss);

  right_read = 1;  //do not change or remove
  left_read = 1;   //do not change or remove

  // Set up the three button inputs
  pinMode(STOP_PIN, INPUT_PULLUP);
  pinMode(LEFT_JOG_PIN, INPUT_PULLUP);
  pinMode(RIGHT_JOG_PIN, INPUT_PULLUP);

  maxAnalogRead = analogRead(SPEED_PIN);
  if (DEBUG) Serial.println("Sender ready...");
}

void loop() {
  //read pin values
  right_jog_read = digitalRead(RIGHT_JOG_PIN);
  left_jog_read = digitalRead(LEFT_JOG_PIN);
  stop_read = digitalRead(STOP_PIN);

  //check if all buttons are open
  allButtonsOpen = allSwitchesOpen(left_read, left_jog_read, right_read, right_jog_read, stop_read);
  //if(DEBUG) printSwitchStates(left_read, left_jog_read,right_read, right_jog_read, stop_read);

  sendPacket = false;
  if (!allButtonsOpen) { //if a button is pressed
    if (stop_read == 0) { //if stop button is pressed
      packet[0] = '0';
      packet[1] = 's';
      packet[2] = 0;
      sendPacket = true;
    }
    else if (left_jog_read == 0) {   //if jog left is pressed
      packet[0] = 's';
      packet[1] = 'b';
      packet[2] = map(analogRead(SPEED_PIN), 0, maxAnalogRead, 0, 254);
      sendPacket = true;
    }
    else if (right_jog_read == 0) { //if jog right is pressed
      packet[0] = 's';
      packet[1] = 'f';
      packet[2] = map(analogRead(SPEED_PIN), 0, maxAnalogRead, 0, 254);
      sendPacket = true;
    }
  }
  else if (!isStopPacket(lastPacket)) { //if a button was released, send a stop command
    packet[0] = '0';
    packet[1] = 's';
    packet[2] = 0;
    if (DEBUG) printPacket(packet);
    HC12.send(packet, sizeof(packet)); //this is here to send the stop packet twice on button lift...just to be sure
    sendPacket = true;
  }

  if (sendPacket) {
    lastPacket[0] = packet[0];
    lastPacket[1] = packet[1];
    lastPacket[2] = packet[2];
    if (DEBUG) printPacket(packet);
    HC12.send(packet, sizeof(packet));
  }
  delay(DELAY);
}

void printPacket(uint8_t packet[]) {
  Serial.print(char(packet[0]));
  Serial.print(",");
  Serial.print(char(packet[1]));
  Serial.print(",");
  Serial.print(int(packet[2]));
  Serial.println();
}

void printSwitchStates(int left, int left_jog, int right, int right_jog, int stop_read) {
  Serial.print("Left: " + String(left));
  Serial.print(",");
  Serial.print("LeftJ: " + String(left_jog));
  Serial.print(",");
  Serial.print("Stop: " + String(stop_read));
  Serial.print(",");
  Serial.print("Right: " + String(right));
  Serial.print(",");
  Serial.print("RightJ: " + String(right_jog));
  Serial.println();
}

boolean arePacketsSame(uint8_t packet[], uint8_t lastPacket[]) {
  if (packet[0] == lastPacket[0] &&
      packet[1] == lastPacket[1] &&
      packet[2] == lastPacket[2])
    return true;
  else
    return false;
}

boolean isStopPacket(uint8_t packet[]) {
  if (packet[0] == '0' && packet[1] == 's' && packet[2] == 0)
    return true;
  else
    return false;
}

boolean allSwitchesOpen(int left, int left_jog, int right, int right_jog, int stop_read) {
  if (left && left_jog && right && right_jog && stop_read) {
    return true;
  }
  else return false;
}

