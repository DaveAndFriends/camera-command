/******************************************
  PURPOSE: Running 2 28BYJ-48 Stepper Motor as Pan/Tilt with the Arduino
  Created by: Jan Schaloske
  DATE:   August 2018
*******************************************/
#include <SoftwareSerial.h>
#include <PacketSerial.h>
#include <Stepper.h>

#define pin1Pan  4 //these are the Arduino pins that we use to activate coils 1-4 of the Pan stepper motor
#define pin2Pan  5
#define pin3Pan  6
#define pin4Pan  7
#define pin1Tilt  8 //these are the Arduino pins that we use to activate coils 1-4 of the Tilt stepper motor
#define pin2Tilt  9
#define pin3Tilt  10
#define pin4Tilt  11

int panPins[4] = {pin1Pan, pin2Pan, pin3Pan, pin4Pan};
int tiltPins[4] = {pin1Tilt, pin2Tilt, pin3Tilt, pin4Tilt};

const byte homeSwitchPan = 2;
byte hSPan;   // new variable
const byte homeSwitchTilt = 3;
byte hSTilt;   // new variable

/**** Joystick Vars ****/
#define PAD 100
int x_init;
int y_init;
String xpos = "NEUTRAL";
String ypos = "NEUTRAL";


PacketSerial HC12;
SoftwareSerial hc12ss(10, 11);
uint8_t packet[] = {0, 0};
boolean sendPacket = false;

// Pan - Tilt Steppers
#define delaytime 3
int steptemp;
int numberOfSteps = 400;

const int stepsPerRevolution = 64;  // change this to fit the number of steps per revolution for your motor

// initialize the stepper library for both steppers:
Stepper tilt_stepper(stepsPerRevolution, pin1Tilt, pin2Tilt, pin3Tilt, pin4Tilt);
Stepper pan_stepper(stepsPerRevolution, pin1Pan, pin2Pan, pin3Pan, pin4Pan);

void setup() {
  //initialize serial comms
  hc12ss.begin(9600);
  HC12.setStream(&hc12ss);
  HC12.setPacketHandler(&onPacketReceived);

  //initialize joystick
  x_init = analogRead(A0);
  y_init = analogRead(A1);

  // initialize the 8 pins as an output:
  pinMode(pin1Pan, OUTPUT);
  pinMode(pin2Pan, OUTPUT);
  pinMode(pin3Pan, OUTPUT);
  pinMode(pin4Pan, OUTPUT);
  pinMode(pin1Tilt, OUTPUT);
  pinMode(pin2Tilt, OUTPUT);
  pinMode(pin3Tilt, OUTPUT);
  pinMode(pin4Tilt, OUTPUT);
  pinMode(homeSwitchPan, INPUT_PULLUP);
  pinMode(homeSwitchTilt, INPUT_PULLUP);
  Serial.begin(9600);
  
  Serial.print("xinit: " + String(x_init));
  Serial.println(" | yinit: " + String(y_init));
  
  Serial.println("Homing......");
  stepperHome(panPins, homeSwitchPan);
  stepperHome(tiltPins, homeSwitchTilt);
  Serial.println("Homed to Open (Drain) position");
  Serial.println(" ");

  // set the speed of the motors
  tilt_stepper.setSpeed(200);    // set first stepper speed
  pan_stepper.setSpeed(200);   // set second stepper speed
}

void loop() {
  HC12.update();

  xpos = get_xpos(analogRead(A0)); // read value from joystick X-axis
  ypos = get_ypos(analogRead(A1)); // read value from joystick Y-axis

  Serial.print("xpos: " + String(xpos));
  Serial.println(" | ypos: " + String(ypos));

  if (xpos.equals("LEFT")) {
    forward(tiltPins);  // step left
    packet[0] = int('l');
    packet[1] = 0;
    sendPacket = true;
  }
  else if (xpos.equals("RIGHT")) {
    backward(tiltPins);  // step right
    packet[0] = int('r');
    packet[1] = 0;
    sendPacket = true;
  }
  else if (xpos.equals("NEUTRAL")) {
    step_OFF(tiltPins);  // stop
    packet[0] = int('s');
    packet[1] = int('t');
    sendPacket = true;
  }

  if (sendPacket) {
    HC12.send(packet, sizeof(packet));
    sendPacket = false;
  }

  if (ypos.equals("UP")) {
    forward(panPins);  // step forward
    packet[0] = int('f');
    packet[1] = 0;
    sendPacket = true;
  }
  else if (ypos.equals("DOWN")) {
    backward(panPins);  // step backward
    packet[0] = int('b');
    packet[1] = int('p');
    sendPacket = true;
  }
  else if (ypos.equals("NEUTRAL")) {
    step_OFF(panPins);  // //power all Tiltcoils down
    packet[0] = int('s');
    packet[1] = 1;
    sendPacket = true;
  }

  if (sendPacket) {
    HC12.send(packet, sizeof(packet));
  }

  sendPacket = false;
  delay(50);

}

void onPacketReceived(const uint8_t* buffer, size_t size)
{
  uint8_t tempBuffer[size];
  memcpy(tempBuffer, buffer, size);
  //TODO

}

void OpenDrain(int pins[]) {

  step_OFF(pins);         //turning all coils off
  steptemp = numberOfSteps;
  while (steptemp > 0) {
    forward(pins);        //going forward
    steptemp -- ;//counting down the number of steps
  }
  step_OFF(pins);

}

void CloseDrain(int pins[], int steptemp) {

  step_OFF(pins);         //turning all coils off
  // steptemp = numberOfSteps;
  while (steptemp > 0) {
    backward(pins);       //going backward
    steptemp -- ;//counting down the number of steps
  }
  step_OFF(pins);

}

void stepperHome(int pins[], int homeSwitch)
{
  hSPan = digitalRead(homeSwitch);
  int hm = 0;
  while (hSPan == HIGH )
    //while ( hm < 200 )
  {
    //backwards slowly till it hits the switch and stops
    forward(pins);
    digitalWrite(12, LOW);
    hSPan = digitalRead(homeSwitch);
    hm ++;
    Serial.println(hm);
    if ( hm > 600) {  //Timed exit if switch malfunctions
      break;
    }
  }
  digitalWrite(12, HIGH); //
  delay(400);
  CloseDrain(pins, 20);
  digitalWrite(12, LOW);
  //
}



//these functions set the pin settings for each of the four steps per rotation of the motor (keep in mind that the motor in the kit is geared down,
//i.e. there are many steps necessary per rotation

void Step_A(int pins[]) {
  digitalWrite(pins[0], HIGH);//turn on coil 1
  digitalWrite(pins[1], LOW);
  digitalWrite(pins[2], LOW);
  digitalWrite(pins[3], LOW);

}
void Step_B(int pins[]) {
  digitalWrite(pins[0], LOW);
  digitalWrite(pins[1], HIGH); //turn on coil 2
  digitalWrite(pins[2], LOW);
  digitalWrite(pins[3], LOW);

}
void Step_C(int pins[]) {
  digitalWrite(pins[0], LOW);
  digitalWrite(pins[1], LOW);
  digitalWrite(pins[2], HIGH); //turn on coil 3
  digitalWrite(pins[3], LOW);

}
void Step_D(int pins[]) {
  digitalWrite(pins[0], LOW);
  digitalWrite(pins[1], LOW);
  digitalWrite(pins[2], LOW);
  digitalWrite(pins[3], HIGH); //turn on coil 4

}
void step_OFF(int pins[]) {
  digitalWrite(pins[0], LOW);
  digitalWrite(pins[1], LOW);
  digitalWrite(pins[2], LOW);
  digitalWrite(pins[3], LOW);
}

//these functions run the above configurations in forward and reverse order
//the direction of a stepper motor depends on the order in which the coils are turned on.
void forward(int pins[]) { //one tooth forward
  Step_A(pins);
  delay(delaytime);
  Step_B(pins);
  delay(delaytime);
  Step_C(pins);
  delay(delaytime);
  Step_D(pins);
  delay(delaytime);
}

void backward(int pins[]) { //one tooth backward
  Step_D(pins);
  delay(delaytime);
  Step_C(pins);
  delay(delaytime);
  Step_B(pins);
  delay(delaytime);
  Step_A(pins);
  delay(delaytime);
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

