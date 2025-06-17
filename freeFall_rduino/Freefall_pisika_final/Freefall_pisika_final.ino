//Motor Pins (From Arduino to tb6600 Motor Driver)
#define enaPin 7 //ena+
#define dirPin 9 //dir+ in tb6600
#define stepPin 8 //PUL+ in tb6600

//Pins for vibration sensor/sw420 and IRF520 mosfet
#define mosfetPin 2
#define vibPin 12

// Value of the steps required for each distance.
const int dist1val = 4545; //value for dist 1 - .9m
const int dist2val = 5530; //value for dist 2 - 1.1m
const int dist3val = 6560; //value for dist 3 - 1.3m

//Web user input variables
int userDist = 0; //----Maiistore ang web user input----(height)
float userInput; //---Maiistore ang web user input---(command)
float timeFreeFalling; //---Maiistore ang web user input---(time)

// other variables to store
float newSteps = 0;
float startMillis;
float endMillis; 

//Boolean states
bool distChosen = false;
bool pressedStart = false;
bool canDropState = false;

//Define custom constants for specifically-defined states for looping
enum State {
  WAITING_FOR_DISTANCE,
  WAITING_FOR_COMMAND
};

State currentState = WAITING_FOR_DISTANCE;  // Declare default or Initial state


void setup() {
  pinMode(vibPin, INPUT);
  pinMode(enaPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(mosfetPin, OUTPUT);
 
  digitalWrite(enaPin, HIGH);
  digitalWrite(mosfetPin, LOW);
  
  // Set up Serial communication
  Serial.begin(9600);
  Serial.println("Choose a number for distance(meter):\n#1  .9m\n#2  1.1 m\n#3  1.3 m\n");
}


void loop() {
  //-------------FOR DISTANCE INPUT-----------
  // Check if user input is available
  while (Serial.available()) {
    char serialData = Serial.read();
    switch (currentState) {  //(Distance or Command) case switching of state
      case WAITING_FOR_DISTANCE:
      
        if (serialData =='H'){ 
          userDist = Serial.parseFloat(); //--------Change stored to web user input-----
          newSteps = convertToSec(userDist);
          Serial.println(newSteps);
          Serial.println("C1 for start, C2 for reset, C3 for drop");
          serialData = Serial.read();
        }
        // Read the user input as an integer for distance
        break;
  //-------------FOR START/DROP/RESET INPUT-----------
      case WAITING_FOR_COMMAND:
        // Read the user input as a string for command
        if (serialData =='C'){ 
          userInput = Serial.parseFloat();  //-------Change stored to web user input----
          
          // Check if the user input is "start", "drop", or "reset"
          userCommand(userInput, newSteps);
        }
        break;
    }
  }
}



//------------------DEFINED FUNCTIONS-----------------------------
void moveMotor(float turns){
  
  //move motor according to user input
  for(float i=0; i < turns; i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(800);
  }
}


void resetValues() {
  startMillis = 0;
  endMillis = 0;
  timeFreeFalling = 0;
  distChosen = false;
  pressedStart = false;
  canDropState = false;
  currentState = WAITING_FOR_DISTANCE;
}


void toInitialState (float num) {
  digitalWrite(dirPin, HIGH); //clockwise or going down
  moveMotor(num);

  digitalWrite(enaPin, HIGH);
  digitalWrite(dirPin,LOW);
  digitalWrite(mosfetPin, LOW);

  //Call resetValues function to reset variables
  resetValues();
  Serial.println("Choose a number for distance(meter):\n#1  .9m\n#2  1.1 m\n#3  1.3 m\n");
}


int convertToSec(int choice) {
  switch (choice) {
    case 1:
      // returns designated seconds as new steps for stepper
      Serial.println("You chose .9 meter.");
      distChosen = true;
      currentState = WAITING_FOR_COMMAND;
      return dist1val; //
    case 2:
      // 
      Serial.println("You chose 1.1 meter.");
      distChosen = true;
      currentState = WAITING_FOR_COMMAND;
      return dist2val;
    case 3:
      // 
      Serial.println("You chose 1.3 meter.");
      distChosen = true;
      currentState = WAITING_FOR_COMMAND;
      return dist3val;
    default:
      currentState = WAITING_FOR_DISTANCE;
      break;
  }
  return 0;
}


int userCommand(int command, float dist) {
  switch (command) {
    case 1:         //case when command input is start 
      if (dist == 0) {
        Serial.println("Returning to angle inputs");
        currentState = WAITING_FOR_DISTANCE; //ask for angle input again
      } else {
        if (pressedStart == false) {
          digitalWrite(enaPin, LOW);
          digitalWrite(dirPin, LOW); //counterclockwise or going up
          digitalWrite(mosfetPin, HIGH);
          delay(1000);
          moveMotor(dist);
          pressedStart = true;
          canDropState = true;
          Serial.println("Ready to Drop");
        }
      }
      break;

    case 2:        //case when command input is reset
      if (pressedStart){
        toInitialState(dist); //calls function to reset the motor state and system
      } else {    //Changes the states only and returns to inputting distance
        distChosen = false;
        canDropState = false;
        currentState = WAITING_FOR_DISTANCE;
        Serial.println("Choose a number for distance(meter):\n#1  .9m\n#2  1.1 m\n#3  1.3 m\n");
      }
      break;
           
    case 3:        //case when command input is roll
      if (canDropState) {
      // IR 1 senses ball, starts counting
        digitalWrite(mosfetPin, LOW);
        startMillis = millis();
            
        while (digitalRead(vibPin) == 0) {}  // Continue vibration sensing loop until vibrated
              
        // Vibration sensed when ball dropped (TRUE state)
        endMillis = millis();
        timeFreeFalling = (endMillis - startMillis) / 1000;
              
        // Export timeFreeFalling to lms
        Serial.print("T");
        Serial.println(timeFreeFalling); //in seconds
              
        delay(1500);
        toInitialState(dist);  // Reset to the initial state

        } else {
          Serial.println("Not yet in position");
          Serial.println("C1 for start, C2 for reset, C3 for drop");
        }
        break;
       
    default:
      Serial.println("C1 for start, C2 for reset, C3 for drop");
      break;
  }
}
