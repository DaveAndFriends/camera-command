#include <AccelStepper.h>

const int buttonPin = 12; //Jog Button clockwise
const int buttonPin1 = 11; //Jog Button Counter clockwise
int buttonState = 0;

// Define the stepper and the pins it will use
AccelStepper stepper1(1, 4, 5); // 4= step ,5= direction

// Define our three input button pins
#define  LEFT_PIN  8
#define  STOP_PIN  9
#define  RIGHT_PIN 10

//Define Limit Switchs
#define LimitLeft 6  // Pin 6 connected to Limit switch out
#define LimitRight 7  // Pin 7 connected to Limit switch out

// Define our analog pot input pin
#define  SPEED_PIN 0

// Define our maximum and minimum speed in steps per second (scale pot to these)
#define  MAX_SPEED 5000
#define  MIN_SPEED 0.1

void setup() {
   
  // The only AccelStepper value we have to set here is the max speeed, which is higher than we'll ever go 
  stepper1.setMaxSpeed(10000.0);
  
  // Set up the three button inputs, with pullups and three pull down resisters
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(STOP_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(buttonPin,INPUT);
  pinMode(buttonPin1,INPUT);
  pinMode(LimitLeft, INPUT_PULLUP);
  pinMode(LimitRight, INPUT_PULLUP);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  static float current_speed = 0.0;         // Holds current motor speed in steps/second
  static int analog_read_counter = 1000;    // Counts down to 0 to fire analog read
  static char sign = 0;                     // Holds -1, 1 or 0 to turn the motor on/off and control direction
  static int analog_value = 0;              // Holds raw analog value.
  
  // If limit switch is activated (low), dont run else continue
  //if (digitalRead(LimitLeft) == HIGH && (digitalRead(RIGHT_PIN) == 0)) // check if limit switch is activated
  //  sign = 0;
  //} 
  // If a switch is pushed down (low), set the sign value appropriately  
  
  if ((digitalRead(LimitRight) == LOW) && (digitalRead(LEFT_PIN) == 0)) {    
    sign = 1; //Turn Stepper Left
  }
  else if ((digitalRead(LimitLeft) == LOW) && (digitalRead(RIGHT_PIN) == 0)) {    
    sign = -1; //Turn Stepper Right
  }
  else if ((digitalRead(LimitLeft) == HIGH) || (digitalRead(LimitRight) == HIGH) || (digitalRead(STOP_PIN) == 0)) {
    sign = 0; //Stop Stepper
  }
  
  // We only want to read the pot every so often (because it takes a long time we don't
  // want to do it every time through the main loop).  
  if (analog_read_counter > 0) {
    analog_read_counter--;
  }
  else {
    analog_read_counter = 3000;
    // Now read the pot (from 0 to 1023)
    analog_value = analogRead(SPEED_PIN);
    // Give the stepper a chance to step if it needs to
    stepper1.runSpeed();
    //  And scale the pot's value from min to max speeds
    current_speed = sign * (((analog_value/1023.0) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED);
    // Update the stepper to run at this new speed
    stepper1.setSpeed(current_speed);
  }

  // This will run the stepper at a constant speed
  stepper1.runSpeed();

  //Jog Button Clockwise
 if (digitalRead(buttonPin) == HIGH){
      current_speed = (((analog_value/1023.0) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED);
     stepper1.setSpeed(-current_speed);
     stepper1.runSpeed();
 }
 //Jog Button Counter clockwise
 if (digitalRead(buttonPin1) == HIGH){
      current_speed = (((analog_value/1023.0) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED);
     stepper1.setSpeed(current_speed);
     stepper1.runSpeed();
 } 
}