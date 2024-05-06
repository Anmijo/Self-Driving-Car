// ********************************************************************************
// *                                                                              *
// *            Arduino Program to Control Johnny Hazbouns Robot Car              *
// *                                                                              *
// *         Submitted to Fuad Farraj Scientific Innovation Competition           *
// *                                                                    *
// ********************************************************************************

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>

// Assignments for Arduino Devices

// Alarm Pin Assignment

  int AlarmPin=3;

// Bluetooth Serial Module HC-06 Definitions

  #include <SoftwareSerial.h>
  const byte txPin = 2;                         // pin to transmit serial data
  const byte rxPin = 4;                         // pin to receive serial data

  SoftwareSerial BTSerial(rxPin, txPin);        // set up a new serial object for BlueTooth module
  
  String commands = "";                         // Bluetooth command string received
  String command1 = "";                         // Start string received
  String command2 = "";                         // Direction string received
  String command3 = "";                         // Speed string received
  String Result = "";                           // Status of BT command Received V(alid)/I(nvalid)
  String dir_ind = "";                          // Direction R(ight)/L(eft)/S(traight)
  int speed_ind = 0;                            // Speed 1 = Stop, 2 = Forward, 3 = Backward
  int direction = 0;                            // Direction value received
  int speed = 0;                                // Speed value received
  int oldDirection = 1;                         // Stores old direction value
  int oldSpeed = 1;                             // Stores old speed value
  int speedRight = 0;                           // Stores calculated right wheel speed
  int speedLeft = 0;                            // Stores calculated left wheel speed
  float work;                                   // Work field used for calculations

  int ind1, ind2, ind3;                         // Parameter location

// Variables will be used to determine the frequency at which the sensor readings are sent to the phone, and when the last command was received
 
  unsigned long timer0 = 2000;      //Stores the time (in milliseconds since execution started)
  unsigned long timer1 = 0;         //Stores the time when the last command was received from Bluetooth

// Ultrasonic Ranging Module HC-SR04 Definitions

  #define trigPin 5                 // Trigger Pin assignment
  #define echoPin 6                 // Echo Pin assignment

// Dual H-Bridge Motor Controller Module L298N Definitions

  // Right Motor Control Pins Assignment
  int enA = 9;                      // Speed control pin 0-255
  int in1 = 7;                      // Backward control pin
  int in2 = 8;                      // Forward control pin

  // Left Motor Control Pins Assignment
  int enB = 11;                     // Speed control pin 0-255
  int in3 = 12;                     // Backward control pin
  int in4 = 13;                     // Forward control pin

// Operational Parameters

  int randomDirection = 1;          // Stores random direction to go in case of obstacle (1 or 2)
  int safeRange = 25;               // Safe range needed
  int minimumRange = 10;            // Minimum range needed
  long duration = 0;                // Echo duration which is used to calculate distance
  long distance = 100;              // Stores calculated distance, initialized to a value beyond minimum range

  int correctionSpeed = 75;         // Speed to use while avoiding obstacle
  int backTime = 300;               // Time in mS to move backwards after obstacle is detected within the safe range

// ********************************************************************************
// *                                                                              *
// *                                 Program Setup                                *
// *                                                                              *
// ********************************************************************************

void setup() {

// set the Alarm control pin to output
  pinMode(AlarmPin, OUTPUT);

// set all the motor control pins to output
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

// set the Ultrasonic Module Trigger pin to Output and Echo pin
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

// Open serial communications for native USB port
  Serial.begin(9600);                 // Open serial port and set data rate to 9600 bps.
  while (!Serial) {
    ;                                 // wait for serial port to connect/open.
  }

// Open serial communications for Bluetooth Module (HC-06 defaults to 9600)
  BTSerial.begin(9600);

Serial.println("Arduino Program Started");

}

// ********************************************************************************
// *                                                                              *
// *                            Program Main Functions                            *
// *                                                                              *
// ********************************************************************************

void FwdRightWheel(int FRS) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, FRS);
}

void BckRightWheel(int BRS) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, BRS);
}

void StpRightWheel() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void FwdLeftWheel(int FLS) {
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, FLS);
}

void BckLeftWheel(int BLS) {
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, BLS);
}

void StpLeftWheel() {
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void ReadDistance() {
  /* Read distance of nearest object from Ultrasonic module
     The trigPin/echoPin cycle is used to determine the distance of the nearest
     object by bouncing soundwaves back to Ultrasonic module.                      */

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance (in cm) based on width of Echo signal divided by 58.2 for Centimeters

  distance = duration / 58.2;

}

// ********************************************************************************
// *                                                                              *
// *                             Program Exection Loop                            *
// *                                                                              *
// ********************************************************************************

void loop() {

// Check distance of near objects and take action

      ReadDistance();
   
  if ((distance <= minimumRange) && (distance != 0)) {

      // Car is too close to object; move back and turn in random direction

      digitalWrite(AlarmPin, HIGH);                 // Turn alarm ON
      BckRightWheel(correctionSpeed);               // Move Back
      BckLeftWheel(correctionSpeed);
      delay(backTime);
      
      randomDirection = random(1, 3);               // Pick a random direction

      if (randomDirection == 1) {
        do {
          FwdRightWheel(correctionSpeed);           // Turn left
          BckLeftWheel(correctionSpeed);
          delay(50);
          ReadDistance();
        } while (distance <= safeRange);
      }
      else {
        do {
          BckRightWheel(correctionSpeed);           // Turn right
          FwdLeftWheel(correctionSpeed);
          delay(50);
          ReadDistance();
        } while (distance <= safeRange);
      }
      digitalWrite(AlarmPin, LOW);                  // Turn alarm OFF and stop
      StpRightWheel();
      StpLeftWheel();
//    distance = safeRange + 1;                     // Correction ended let distance beyond safe range
  }

// Check Bluetooth command received

  if(BTSerial.available()){
          commands = "";
          commands = BTSerial.readStringUntil('\n');      // New command received

          ind1 = commands.indexOf(',');                   // Find location of first comma in BT string
          ind2 = commands.indexOf(',', ind1+1 );          // Find location of second comma in BT string
          ind3 = commands.indexOf(',', ind2+1 );          // Find location of third comma in BT string

          command1 = commands.substring(0, ind1);         // Get header string
          if (command1 == "RC") {Result = "V";}
               else {Result = "I";}
          
          command2 = commands.substring(ind1+1, ind2);    // Get direction string
          direction = command2.toInt();                   // Convert direction string value to signed integer
          if ((direction > -101) && (direction < 101)) {
            Result = "V";
            if (direction < 0) {
              dir_ind = "L";
              direction = direction * (-1);
            }
            else {
              if (direction == 0) {
                dir_ind = "S";
              }
              else {
                dir_ind ="R";
              }
            }
            }
            else {Result = "I";}

          command3 = commands.substring(ind2+1, ind3);    // Get speed string
          speed = command3.toInt();                       // Convert speed string value to signed integer

          if ((speed > -101) && (speed < 101)) {
            Result = "V";
            if (speed < 0) {
              speed_ind = 3;                              // Backward move
              speed = speed * (-1);
            }
            else {
                if (speed == 0) {
                  speed_ind = 1;                          // Stop
                }
                else {
                  speed_ind =2;                           // Forward move
                }
            }
          }
          else {Result = "I";}

    if ((direction == 0) && (oldDirection == 0) && (speed == 0) && (oldSpeed == 0)) {
      // Automatic mode started
      Result = "V";
      dir_ind = "S";                                      // Direction straight
      speed_ind = 2;                                      // Move forward
      speed=correctionSpeed;                              // Use correction speed
    }
    else {
      oldDirection=direction;
      oldSpeed=speed;
    }
    /*
    Serial.println(direction);
    Serial.println(speed);
    Serial.println(oldDirection);
    Serial.println(oldSpeed);
    Serial.println(Result);
    Serial.println(dir_ind);
    Serial.println(speed_ind);
    Serial.println(speedRight);
    Serial.println(speedLeft);
    Serial.println();
    */

  }

// Execute last command

  if (Result == "V") {                                    // Valid BT command
     switch (speed_ind) {
        case 1:                                           // Stop completely and turn alarm OFF
             speedRight = 0;
             speedLeft = 0;
             StpRightWheel();
             StpLeftWheel();
             digitalWrite(AlarmPin, LOW);
             break;

        case 2:
             if (dir_ind == "S") {                        // Drive forward straight
                speedRight = speed;
                speedLeft = speed;
                FwdRightWheel(speedRight);
                FwdLeftWheel(speedLeft);
             }
             else {
                if (dir_ind == "R") {                     // Drive forward right
                   work = (float)speed * ((400-(float)direction)/400);
                   speedRight = (int)work;
                   speedLeft = speed;
                   FwdRightWheel(speedRight);
                   FwdLeftWheel(speedLeft);
                }
                else {
                   if (dir_ind == "L") {                  // Drive forward left
                   speedRight = speed;
                   work = (float)speed * ((400-(float)direction)/400);
                   speedLeft = (int)work;
                   FwdRightWheel(speedRight);
                   FwdLeftWheel(speedLeft);
                   }
                }
             }
             break;

        case 3:
             if (dir_ind == "S") {                        // Drive backward straight
                speedRight = speed;
                speedLeft = speed;
                BckRightWheel(speedRight);
                BckLeftWheel(speedLeft);
             }
             else {
                if (dir_ind == "R") {                     // Drive backward right
                   work = (float)speed * ((400-(float)direction)/400);
                   speedRight = (int)work;
                   speedLeft = speed;
                   BckRightWheel(speedRight);
                   BckLeftWheel(speedLeft);
                }
                else {                                    // Drive backward left
                   speedRight = speed;
                   work = (float)speed * ((400-(float)direction)/400);
                   speedLeft = (int)work;
                   BckRightWheel(speedRight);
                   BckLeftWheel(speedLeft);
                }
             }                      
             break;
                      
     }

  }
  
  else {
    
     // Invalid command received reset the Bluetooth Serial port
     
     BTSerial.end();
     BTSerial.begin(9600);
  }

// Delay 50ms before next cycle

   delay(50);

}
