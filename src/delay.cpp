/* 
Delay commands tell the board to do nothing for a set period of time.
In this project, the delay value is set at the SmartWorks IoT Platform by entering a value in a property. 
-. Arduino.h: includes Arduino specific commands for coding

Reference: arduino.cc
*/

#include <Arduino.h>

static int delay_value; //Global variable to store the delay value set at the platform.

// init_delay funciton initializes the delay value as specify in the main
void init_delay(int time){
  delay_value = time;
}

//parse_delay function receives the delay value from the platform and stores it in the global variable delay_value
void parse_delay(char* subscribeTopic, byte* payload, unsigned int length){


  char receivedChar; //To store the required information from the payload received 
  int numLength = length - 1 - 9; //Store the length of the delay value 
  char numChar[numLength]; // Create a char variable with the length of the delay value
  int j =0;
  Serial.print("\n Payload:");
  
  // Store the payload message received on the variables defined
  for (unsigned int i = 9 ; i < length-1; i++){
    receivedChar = (char)payload[i]; 
    numChar[j] = receivedChar;
    Serial.print((char)payload[i]);
    j++;
  }
  //Convert char variable to integer
  delay_value = atoi(numChar);
  Serial.print("\n Delay value:");
  Serial.print(delay_value);
}

// redturn_delay function provides the delay_value
int return_delay(void){
  return delay_value;
}