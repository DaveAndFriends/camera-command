/******************************************
  PURPOSE: Read analog joystick data and send the information to a HC12 transceiver
  Created by: Davis Johnson
  DATE:   August 2018
*******************************************/
#include <SoftwareSerial.h>
#include <PacketSerial.h>

/**** Joystick Vars ****/
#define PAD 200
int x_init;
int y_init;
String old_xpos = "NEUTRAL";
String old_ypos = "NEUTRAL";
String xpos = "";
String ypos = "";

/**** Serial Vars ****/
PacketSerial HC12;
SoftwareSerial hc12ss(3, 2); //RX TX

/**** Data packet ****/
/* Format: tilt,f/b/s,pan,f/b/s */
uint8_t packet[] = {int('t'), int('s'), int('p'), int('s')};
boolean sendPacket = false;


void setup() {
  //Initialize serial comms
  hc12ss.begin(9600);
  HC12.setStream(&hc12ss);
  HC12.setPacketHandler(&onPacketReceived);
  
  //Initialize joystick
  x_init = analogRead(A0);
  y_init = analogRead(A1);

  Serial.begin(9600);
  
  //Serial.print("xinit: " + String(x_init));
  //Serial.println(" | yinit: " + String(y_init));
}

void loop() {
  
  xpos = get_xpos(analogRead(A0)); // read value from joystick X-axis
  ypos = get_ypos(analogRead(A1)); // read value from joystick Y-axis

  //Serial.print("xpos: " + String(xpos));
  //Serial.println(" | ypos: " + String(ypos));

  if (xpos.equals("LEFT")) {
    packet[0] = int('t');
    packet[1] = int('b');
    sendPacket = true;
  }
  else if (xpos.equals("RIGHT")) {
    packet[0] = int('t');
    packet[1] = int('f');
    sendPacket=true;
  }
  else if (xpos.equals("NEUTRAL") && !old_xpos.equals("NEUTRAL") ){
    packet[0] = int('t');
    packet[1] = int('s');
    sendPacket=true;
  }

  if (ypos.equals("DOWN")) {
    packet[2] = int('p');
    packet[3] = int('b');
    sendPacket = true;
  }
  else if (ypos.equals("UP")) {
    packet[2] = int('p');
    packet[3] = int('f');
    sendPacket=true;
  }
  else if (ypos.equals("NEUTRAL") && !old_ypos.equals("NEUTRAL") ) {
    packet[2] = int('p');
    packet[3] = int('s');
    sendPacket = true;
  }

  //Serial.print(String(sendPacket) + "|");
  //printPacket(packet);
  
  if(sendPacket){
    HC12.send(packet, sizeof(packet));
    sendPacket=false;
  }

  old_xpos = xpos;
  old_ypos = ypos;
  
  delay(10);
}

void onPacketReceived(const uint8_t* buffer, size_t size)
{
  uint8_t tempBuffer[size];
  memcpy(tempBuffer, buffer, size);
  //TODO implement call/response
}

void printPacket(uint8_t packet[]){
  Serial.print(char(packet[0]));
  Serial.print(",");
  Serial.print(char(packet[1]));
  Serial.print(",");
  Serial.print(char(packet[2]));
  Serial.print(",");
  Serial.print(char(packet[3]));
  Serial.println();
}


//Get Y position of joystick
String get_ypos(int reading) {
  String result = "";
  if (reading < (y_init + PAD) && reading > (y_init - PAD)) {
    result = "NEUTRAL";
  }
  if (reading < (y_init - PAD)) {
    result = "DOWN";
  }
  else if (reading > (y_init + PAD)) {
    result = "UP";
  }
  return result;
}

//Get X postition of joystick
String get_xpos(int reading) {
  String result = "";
  if (reading < (x_init + PAD) && reading > (x_init - PAD)) {
    result = "NEUTRAL";
  }
  if (reading < (x_init - PAD)) {
    result = "LEFT";
  }
  else if (reading > (x_init + PAD)) {
    result = "RIGHT";
  }
  return result;
}

