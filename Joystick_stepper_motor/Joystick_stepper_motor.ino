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
const byte homeSwitchPan = 2;
byte hSPan;   // new variable
const byte homeSwitchTilt = 3;
byte hSTilt;   // new variable

PacketSerial HC12;
SoftwareSerial hc12ss(10,11);


// Pan - Tilt Steppers
#define delaytime 3
int steptemp;
int numberOfSteps = 400;

const int stepsPerRevolution = 64;  // change this to fit the number of steps per revolution for your motor

// initialize the stepper library for both steppers:
Stepper small_stepper(stepsPerRevolution, 8, 10, 9, 11);
Stepper small_stepper2(stepsPerRevolution, 4, 6, 5, 7);

void setup() {
  //initialize serial comms
  hc12ss.begin(9600);
  HC12.setStream(&hc12ss);
  HC12.setPacketHandler(&onPacketReceived);
  
  
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

  Serial.println("Homing......");
  stepperHomePan();
  stepperHomeTilt();
  Serial.println("Homed to Open (Drain) position");
  Serial.println(" ");

  // set the speed of the motors
  small_stepper.setSpeed(400);    // set first stepper speed
  small_stepper2.setSpeed(200);   // set second stepper speed
}

void loop() {

  int sensorReading = analogRead(A0); // read value from joystick X-axis

  if (sensorReading < 490) {
    small_stepper.step(1);  // step left
  }
  if (sensorReading > 540) {
    small_stepper.step(-1);  // step right
  }
  if (sensorReading >= 490 && sensorReading <= 540) {
    step_OFFTilt();  // //power all Tiltcoils down
  }

  int sensorReading2 = analogRead(A1); // read value from joystick Y-axis

  if (sensorReading2 < 490) {
    small_stepper2.step(1);  // step forward
  }
  if (sensorReading2 > 540) {
    small_stepper2.step(-1);  // step backward
  }
  if (sensorReading2 >= 490 && sensorReading2 <= 540) {
    step_OFFPan();  // //power all Pancoils down
  }


}

void onPacketReceived(const uint8_t* buffer, size_t size)
{
    // Process your decoded incoming packet here.
}

void OpenDrain() {

  step_OFFPan();         //turning all coils off
  steptemp = numberOfSteps;
  while (steptemp > 0) {
    forwardPan();        //going forward
    steptemp -- ;//counting down the number of steps
  }
  step_OFFPan();

}

void CloseDrain(int steptemp) {

  step_OFFPan();         //turning all coils off
  // steptemp = numberOfSteps;
  while (steptemp > 0) {
    backwardPan();       //going backward
    steptemp -- ;//counting down the number of steps
  }
  step_OFFPan();

}

void stepperHomePan()
{
  hSPan = digitalRead(homeSwitchPan);
  int hm = 0;
  while (hSPan == HIGH )
    //while ( hm < 200 )
  {
    //backwards slowly till it hits the switch and stops
    forwardPan();
    digitalWrite(12, LOW);
    hSPan = digitalRead(homeSwitchPan);
    hm ++;
    Serial.println(hm);
    if ( hm > 600) {  //Timed exit if switch malfunctions
      break;
    }
  }
  digitalWrite(12, HIGH); //
  delay(400);
  CloseDrain(20);
  digitalWrite(12, LOW);
  //
}

void OpenDrainTilt() {

  step_OFFTilt();         //turning all coils off
  steptemp = numberOfSteps;
  while (steptemp > 0) {
    forwardTilt();        //Tilt going forward
    steptemp -- ;//counting down the number of steps
  }
  step_OFFTilt();

}

void CloseDrainTilt(int steptemp) {

  step_OFFTilt();         //turning all coils off
  // steptemp = numberOfSteps;
  while (steptemp > 0) {
    backwardTilt();       //Tilt going backward
    steptemp -- ;//counting down the number of steps
  }
  step_OFFTilt();

}

void stepperHomeTilt()
{
  hSTilt = digitalRead(homeSwitchTilt);
  int hm = 0;
  while (hSTilt == HIGH )
    //while ( hm < 200 )
  {
    //backwards slowly till it hits the switch and stops
    forwardTilt();
    digitalWrite(13, LOW);
    hSTilt = digitalRead(homeSwitchTilt);
    hm ++;
    Serial.println(hm);
    if ( hm > 600) {  //Timed exit if switch malfunctions
      break;
    }
  }
  digitalWrite(13, HIGH); //
  delay(400);
  CloseDrainTilt(20);
  digitalWrite(13, LOW);
  //
}



//these functions set the pin settings for each of the four steps per rotation of the motor (keep in mind that the motor in the kit is geared down,
//i.e. there are many steps necessary per rotation

void Step_APan() {
  digitalWrite(pin1Pan, HIGH);//turn on coil 1
  digitalWrite(pin2Pan, LOW);
  digitalWrite(pin3Pan, LOW);
  digitalWrite(pin4Pan, LOW);

}
void Step_BPan() {
  digitalWrite(pin1Pan, LOW);
  digitalWrite(pin2Pan, HIGH);//turn on coil 2
  digitalWrite(pin3Pan, LOW);
  digitalWrite(pin4Pan, LOW);

}
void Step_CPan() {
  digitalWrite(pin1Pan, LOW);
  digitalWrite(pin2Pan, LOW);
  digitalWrite(pin3Pan, HIGH); //turn on coil 3
  digitalWrite(pin4Pan, LOW);

}
void Step_DPan() {
  digitalWrite(pin1Pan, LOW);
  digitalWrite(pin2Pan, LOW);
  digitalWrite(pin3Pan, LOW);
  digitalWrite(pin4Pan, HIGH); //turn on coil 4

}
void step_OFFPan() {
  digitalWrite(pin1Pan, LOW); //power all coils down
  digitalWrite(pin2Pan, LOW);
  digitalWrite(pin3Pan, LOW);
  digitalWrite(pin4Pan, LOW);

}

void Step_ATilt() {
  digitalWrite(pin1Tilt, HIGH);//turn on coil 1
  digitalWrite(pin2Tilt, LOW);
  digitalWrite(pin3Tilt, LOW);
  digitalWrite(pin4Tilt, LOW);
}
void Step_BTilt() {
  digitalWrite(pin1Tilt, LOW);
  digitalWrite(pin2Tilt, HIGH);//turn on coil 2
  digitalWrite(pin3Tilt, LOW);
  digitalWrite(pin4Tilt, LOW);
}
void Step_CTilt() {
  digitalWrite(pin1Tilt, LOW);
  digitalWrite(pin2Tilt, LOW);
  digitalWrite(pin3Tilt, HIGH); //turn on coil 3
  digitalWrite(pin4Tilt, LOW);
}
void Step_DTilt() {
  digitalWrite(pin1Tilt, LOW);
  digitalWrite(pin2Tilt, LOW);
  digitalWrite(pin3Tilt, LOW);
  digitalWrite(pin4Tilt, HIGH); //turn on coil 4
}
void step_OFFTilt() {
  digitalWrite(pin1Tilt, LOW); //power all coils down
  digitalWrite(pin2Tilt, LOW);
  digitalWrite(pin3Tilt, LOW);
  digitalWrite(pin4Tilt, LOW);
}

//these functions run the above configurations in forward and reverse order
//the direction of a stepper motor depends on the order in which the coils are turned on.
void forwardPan() { //one tooth forward
  Step_APan();
  delay(delaytime);
  Step_BPan();
  delay(delaytime);
  Step_CPan();
  delay(delaytime);
  Step_DPan();
  delay(delaytime);
}

void backwardPan() { //one tooth backward
  Step_DPan();
  delay(delaytime);
  Step_CPan();
  delay(delaytime);
  Step_BPan();
  delay(delaytime);
  Step_APan();
  delay(delaytime);

}

void forwardTilt() { //one tooth forward
  Step_ATilt();
  delay(delaytime);
  Step_BTilt();
  delay(delaytime);
  Step_CTilt();
  delay(delaytime);
  Step_DTilt();
  delay(delaytime);
}

void backwardTilt() { //one tooth backward
  Step_DTilt();
  delay(delaytime);
  Step_CTilt();
  delay(delaytime);
  Step_BTilt();
  delay(delaytime);
  Step_ATilt();
  delay(delaytime);

}
