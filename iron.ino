#include <VarSpeedServo.h>

#include<Bounce2>
#include<ButtonEvents.h>

// Declare pin setting
const int servo1Pin = 9; // set the PWM pin for servo 1
const int servo2Pin = 10; //set the PWM pin for servo 2

const int pirPin = 2; // set the pin for pir sensor

// led control pin
const int leftEyePin = 6; // left eye LEDs (PWM pin)
const int rightEyePin = 3; // Right eye LEDs (PWM pin)

// Declare servo objects
VarSpeedServo servo1; // create servo object to control servo 1
VarSpeedServo servo2; // create servo object to control servo 2

// Declare variables for servo speed control
const int servoCloseSpeed = 100; // set the speed of the servo close function
const int servoOpenSpeed = 255; // set the speed of the servo opening recommend set to max speed to aid in lift

// In Dual Servo Configuration the servos move in opposing directions, so the angles of the servos will be opposite to each other. 
// Normal Servo range is 0° ~ 180°, for initial setup the range has been adjusted to 20° ~ 160°, this allows for a 20° adjustment at both ends of the servo range.
const int servo1_OpenPos = 0; // set the open position of servo 1
const int servo2_OpenPos = 180; // set the open position of servo 2
const int servo1_ClosePos = 170; // set the closed position of servo 1
const int servo2_ClosePos = 10; // set the closed position of servo 2

// button pin
const int buttonPin = 4;

//Declare variables for pir sensor
int pirRead;

int HelmetStat = -1;

// Declare variables for setup special effects (applies to LED eyes only for now)
#define SETUP_NONE 0 // No special effects, just turn on the LED eyes
#define SETUP_MOVIE_BLINK 1 // Blink LED eyes on setup, sequence based on Avengers Movie
#define SETUP_FADE_ON 2 // Slowly brighten LED eyes until fully lit

// Uncomment this line if you don't want any special effect during setup, comment this line to disable this effect
// const int setupFx = SETUP_NONE;

// Uncomment this line if you want the movie blink special effect during setup, comment this line to disable this effect
const int setupFx = SETUP_MOVIE_BLINK;

// Uncomment this line if you want the fade on special effect during setup, comment this line to disable this effect
// const int setupFx = SETUP_FADE_ON;

// Declare variables for LED eyes special effects (applies to LED eyes only for now)
#define EYES_NONE 0 // No special effects, just turn on the LED eyes
#define EYES_MOVIE_BLINK 1 // Blink LED eyes on setup, sequence based on Avengers Movie
#define EYES_FADE_ON 2 // Slowly brighten LED eyes until fully lit

// Uncomment this line if you don't want any special effect during setup, comment this line to disable this effect
 const int eyesFx = EYES_NONE;

// Uncomment this line if you want the movie blink special effect during setup, comment this line to disable this effect
// const int eyesFx = EYES_MOVIE_BLINK;

// Declare variables for LED control
unsigned long fadeDelay = .1; //speed of the eye 'fade'
unsigned long callDelay = 10; //length to wait to start eye flicker after face plate comes down
unsigned long blinkSpeed = 60; //delay between init blink on/off
unsigned long currentPWM = 0; // keep track of where the current PWM level is at
bool isOpen = true; // keep track of whether or not the faceplate is open

// define object for primary button to handle
// multiple button press features:
// 1. single tap
// 2. double tap
ButtonEvents primaryButton = Buttonevents();

// State of the faceplate 1 = open, 0 = closed
#define FACEPLATE_CLOSED 0
#define FACEPLATE_OPEN 1
int facePlateCurMode = FACEPLATE_OPEN; // Keep track if the faceplate is open or closed


// State of the LED eyes 1 = on, 2 = off
#define LED_EYES_OFF 0
#define LED_EYES_ON 1

// State of the LED eyes for dimming/brightening 1 = brighten, 2 = dim
#define LED_EYES_DIM_MODE 0
#define LED_EYES_BRIGHTEN_MODE 1

int ledEyesCurMode = LED_EYES_DIM_MODE; // Keep track if we're dimming or brightening
int ledEyesCurPwm = 0; // Tracking the level of the LED eyes for dim/brighten feature
const int ledEyesIncrement = 15; // Define the increments to brighten or dim the LED eyes


void simDelay(long period){
  long delayMillis = millis() + period;
  while (millis() <= delayMillis)
  {
    int x = 0; // dummy variable, does nothing
  }
}

/**
 * Set the brightness of the LED eyes
 * 
 * @param[out] pwmValue - the PWM value (0-255) for the LED brightness
 */
void setLedEyes(int pwmValue){
  analogWrite(rightEyePin, pwmValue);
  analogWrite(leftEyePin, pwmValue);
  ledEyesCurPwm = pwmValue;
}


/**
 * Simulate the eyes slowly blinking until fully lit
 */ 
void movieblink(){

  // pause for effect...
  simDelay(300);

  int lowValue = 21;
  int delayInterval[] = { 210, 126, 84 };
  int delayVal = 0;

  // First blink on
  for (int i = 0; i <= lowValue; i++){
    setLedEyes(i);
    delayVal = delayInterval[0]/lowValue;
    simDelay(delayVal);
  }

  // Turn off
  setLedEyes(0);
  simDelay(delayInterval[0]);

  // Second blink on
  for (int i = 0; i <= lowValue; i++){
    setLedEyes(i);
    delayVal = delayInterval[1]/lowValue;
    simDelay(delayVal);
  }

  // Turn off
  setLedEyes(0);
  simDelay(delayInterval[1]);

  // Third blink on
  setLedEyes(lowValue);
  simDelay(delayInterval[2]);

  // Turn off
  setLedEyes(0);
  simDelay(delayInterval[2]);

  // All on
  setLedEyes(255);
}


/**
 * Method to dim or brighten both LED eyes
 */
void ledEyesDim(){

  ledEyesCurPwm = ledEyesCurPwm - ledEyesIncrement; // Decrease the brightness

  // Make sure we don't go over the limit
  if(ledEyesCurPwm <= 0){
    ledEyesCurPwm = 0;
  }
}

void ledEyesBrighten(){

  ledEyesCurPwm = ledEyesCurPwm + ledEyesIncrement; // Increase the brightness

  // Make sure we don't go over the limit
  if(ledEyesCurPwm >= 255){
    ledEyesCurPwm = 255;
  }
}


/*
 * Simulate LED eyes slowly brightening until fully lit
 */
 void fadeEyesOn(){
  ledEyesCurMode = LED_EYES_BRIGHTEN_MODE;

  // loop until fully lit
  while (ledEyesCurPwm < 255){
    setLedEyes(ledEyesCurPwm);
  
    simDelay(200);
    ledEyesBrighten();
  }
  
 }

 /**
 * Method to open face plate
 */
 void facePlateOpen(){

  // Re-attach the servos to their pins
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);

  // Send data to the servos for movement
    
  servo1.write(servo1_OpenPos, servoOpenSpeed, true);
  servo2.write(servo2_OpenPos, servoOpenSpeed, true);
  
  simDelay(1000);

  // Detach so motors don't "idle"
  servo1.detach();
  servo2.detach();

  facePlateCurMode = FACEPLATE_OPEN;
 }

/**
  * Method to close face plate
  */
 void facePlateClose(){

  // Re-attach the servos to their pins
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);

  
  servo1.write(servo1_ClosePos, servoCloseSpeed, true);
  servo2.write(servo2_ClosePos, servoCloseSpeed, true);

  simDelay(1000);

  // Detach so motors don't "idle"
  servo1.detach();
  servo2.detach();

  facePlateCurMode = FACEPLATE_CLOSED;
 }

/**
 * Method to turn on LED eyes
 */
void ledEyesOn(){
  
  setLedEyes(255);
  
  ledEyesCurMode = LED_EYES_DIM_MODE;
}

/**
 * Method to turn off LED eyes
 */
void ledEyesOff(){
  
  setLedEyes(0);

  ledEyesCurMode = LED_EYES_BRIGHTEN_MODE;
}

/**
 * Method to turn LED eyes on/off
 */
void ledEyesOnOff(){
  // LED eyes stay off when faceplate is open
  if(facePlateCurMode == FACEPLATE_CLOSED){
    if (ledEyesCurPwm > 0){
      ledEyesOff();
    } else {
      ledEyesOn();
    }
  }
}

/**
 * Method to dim or brighten both LED eyes
 */
void ledEyesFade(){
  if(ledEyesCurPwm == 255){
    ledEyesCurMode = LED_EYES_DIM_MODE;
  } else if(ledEyesCurPwm == 0){
    ledEyesCurMode = LED_EYES_BRIGHTEN_MODE;
  }
  
  if(ledEyesCurMode == LED_EYES_BRIGHTEN_MODE){
    ledEyesBrighten();
  } else {
    ledEyesDim();
  }

  setLedEyes(ledEyesCurPwm);

  simDelay(200);
}

/**
 * Method to run sequence of sppecial effects when system first starts or sets up
 */
void startupFx(){
  //facePlateClose();

  facePlateClose();

  switch(setupFx){
    case SETUP_NONE:
      ledEyesOn();
      break;
    case SETUP_MOVIE_BLINK:
      movieblink();
      break;
    case SETUP_FADE_ON:
      fadeEyesOn();
      break;
  }
}

/**
 * Method to execute special effects when the faceplate opens
 */
void facePlateOpenFx(){
  // TODO: See if we need delays in between fx

  ledEyesOff();

  facePlateOpen();
}

/**
 * Method to execute special effects when the faceplate closes
 */
void facePlateCloseFx(){

  facePlateClose();

  switch(eyesFx){
    case EYES_NONE:
      ledEyesOn();
      break;
    case EYES_MOVIE_BLINK:
      movieblink();
      break;
    case EYES_FADE_ON:
      fadeEyesOn();
      break;
  }
}

/**
 * Handle faceplate special effects
 */
void facePlateFx(){
  if (facePlateCurMode == FACEPLATE_OPEN){
    facePlateCloseFx();
  } else {
    facePlateOpenFx();
  }
}

/**
 * Event handler for when the primary button is tapped once
 */
void handlePrimaryButtonSingleTap(){
  movieblink();
}

/**
 * Event handler for when the primary button is double tapped
 */
void handlePrimaryButtonDoubleTap(){
  ledEyesOff();
}

/**
 * Initializes the primary button for multi-functions
 */
void initPrimaryButton(){
  // Attach the button to the pin on the board
  primaryButton.attach(buttonPin, INPUT_PULLUP);
  // Initialize button features...
  primaryButton.activeLow();
  primaryButton.debounceTime(15);
  primaryButton.doubleTapTime(250);
  primaryButton.holdTime(2000);
}

/**
 * Monitor for when the primary button is pushed
 */
void monitorPrimaryButton(){
  bool changed = primaryButton.update();

  // Was the button pushed?
  if (changed){
    int event = primaryButton.event(); // Get how the button was pushed

    switch(event){
      case(tap):
        handlePrimaryButtonSingleTap();
        break;
      case (doubleTap):
        handlePrimaryButtonDoubleTap();
        break;
    }
  }
}


/**
 * Initialization method called by the Arduino library when the board boots up
 */
void setup() {
  // Set up serial port
  //Serial.begin(115200);  
  
  simDelay(2000); // Give the serial service time to initialize

  startupFx(); // Run the initial features
}

void loop(){
  pirRead = digitalRead(pirPin);
  monitorPrimaryButton();

  if (pirRead == HIGH){
    HelmetStat =  HelmetStat * -1;
    if (HelmetStat>0){
      //facePlateClose();   //Close the faceplate without any LED effect
      facePlateCloseFx();
    }else{
      //facePlateOpen();  //Open the faceplate without any LED effect
      facePlateOpenFx();   //Open the faceplate with any LED effect
    }
  }
  simDelay(100);
}
