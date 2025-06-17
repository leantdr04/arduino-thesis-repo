//Importing libraries
#include <Servo.h>

//Motor Pins (From Arduino to Motor Driver)
#define enaPin 7 //ena pin of tb6600
#define stepPin 8 // pul pin of tb6600
#define dirPin 9 // dir pin of tb6600

//Pins for IR and microservo
#define ir1Pin 2
#define mservoPin 12

//Define steps for required angle
const int angle1 = 36;
const int angle2 = 46;
const int angle3 = 52;

//Web user input variables
int userAngle; //----Maiistore ang web user input----(angle)
float userInput; //---Maiistore ang web user input---(command)
float timeRolling; //---Maiistore ang web user input---(time)

// other variables to store
float newSteps = 0;
float startMillis;
float endMillis;

//Boolean states
bool angleChosen = false;
bool pressedStart = false;
bool canRollState = false;

//Define custom constants for specifically-defined states for looping
enum State {
  WAITING_FOR_ANGLE,
  WAITING_FOR_COMMAND
};

// Create instance of servo and the main loop state
Servo myservo;
State currentState = WAITING_FOR_ANGLE;


void setup() {
  pinMode(ir1Pin, INPUT);
  pinMode(enaPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  digitalWrite(enaPin, HIGH);

  myservo.attach(mservoPin);
  myservo.write(145);

  // Set up Serial communication
  Serial.begin(9600);
  Serial.println("Choose a number for angle:\n#1  30 deg\n#2  45 deg\n#3  60 deg\n");
}

void loop() {
  //-------------FOR ANGLE INPUT-----------
  // Check if user input is available
  while (Serial.available()) {
    char serialData = Serial.read();
    switch (currentState) {  //(Angle or Command) case switching of state
      case WAITING_FOR_ANGLE:

        if (serialData == 'H') {
          userAngle = Serial.parseFloat(); //--------Change stored to web user input-----
          newSteps = convertToSec(userAngle);
          Serial.println(newSteps);
          Serial.println("C1 for start, C2 for reset, C3 for roll");
          serialData = Serial.read();
        }
        // Read the user input as an integer for angle
        break;
      //-------------FOR START/ROLL/RESET INPUT-----------
      case WAITING_FOR_COMMAND:
        // Read the user input as a string for command
        if (serialData == 'C') {
          userInput = Serial.parseFloat();  //-------Change stored to web user input----

          // Check if the user input is "start", "reset", or "roll"
          userCommand(userInput, newSteps);
        }
        break;
    }
  }
}



//------------------DEFINED FUNCTIONS-----------------------------
void moveMotor(float turns) {
  Serial.println("moving");

  for (float i = 0; i < turns; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(70);
    digitalWrite(stepPin, LOW);
    delay(70);
  }
}


void resetValues() {
  startMillis = 0;
  endMillis = 0;
  timeRolling = 0;
  angleChosen = false;
  pressedStart = false;
  canRollState = false;
  currentState = WAITING_FOR_ANGLE;
}


void toInitialState (float backSteps) {
  digitalWrite(dirPin, LOW); //Opposite direction
  delay(500);
  moveMotor(backSteps);

  //return and Call resetValues to default variables
  digitalWrite(enaPin, HIGH);
  resetValues();
  Serial.println("Choose a number for angle:\n#1  30 deg\n#2  45 deg\n#3  60 deg\n");
}


int convertToSec(int choice) {
  switch (choice) {
    case 1:
      // returns designated seconds as new steps for stepper
      Serial.println("You chose 30 degrees.");
      angleChosen = true;
      currentState = WAITING_FOR_COMMAND;
      return angle1; //
    case 2:
      //
      Serial.println("You chose 45 degrees.");
      angleChosen = true;
      currentState = WAITING_FOR_COMMAND;
      return angle2;
    case 3:
      Serial.println("You chose 60 degrees.");
      angleChosen = true;
      currentState = WAITING_FOR_COMMAND;
      return angle3;
    default:
      Serial.println("Choose a number for angle:\n#1  30 deg\n#2  45 deg\n#3  60 deg\n");
      break;
  }
  return 0;
}


int userCommand(int command, float angle) {
  switch (command) {
    case 1:         //case when command input is start
      if (angle == 0) {
        Serial.println("Returning to angle inputs");
        currentState = WAITING_FOR_ANGLE; //ask for angle input again
      } else {
        if (pressedStart == false) {
          digitalWrite(dirPin, HIGH);
          digitalWrite(enaPin, LOW);
          delay(500);

          moveMotor(angle);
          pressedStart = true;
          canRollState = true;
          Serial.println("Good to Roll");
        }
      }
      break;

    case 2:        //case when command input is reset
      if (pressedStart) {
        toInitialState(angle); //calls function to reset the motor state and system
      } else {    //Changes the states only and returns to inputting distance
        angleChosen = false;
        canRollState = false;
        currentState = WAITING_FOR_ANGLE;
        Serial.println("Choose a number for angle:\n#1  30 deg\n#2  45 deg\n#3  60 deg\n");
      }
      break;

    case 3:        //case when command input is roll
      if (canRollState) {
        // IR 1 senses ball, starts counting
        myservo.write(50);
        startMillis = millis();

        while (digitalRead(ir1Pin) == HIGH) {} // Continue IR sensing loop until ball is sensed

        // IR sensor detected the ball (LOW state)
        endMillis = millis();
        timeRolling = (endMillis - startMillis) / 1000;

        // Export timeRolling to lms
        Serial.print("T");
        Serial.println(timeRolling); //in seconds

        delay(1000);
        toInitialState(angle);
        delay(2000);
        myservo.write(145);
      } else {
        Serial.println("Not yet in position");
        Serial.println("C1 for start, C2 for reset, C3 for roll");
      }
      break;

    default:
      Serial.println("C1 for start, C2 for reset, C3 for roll");
      break;
  }
}
