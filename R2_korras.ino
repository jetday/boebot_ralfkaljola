/**
  This program is free software: you can redistribute it and / or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see < https : //www.gnu.org/licenses/>.
**/


/* Includes -------------------------------------------------- */
#include <Servo.h>

/* Defines ------------------------------------------------------------------ */
#define button_pin      2
#define sonic_echo_pin  3
#define sonic_trig_pin  4
#define right_servo_pin 5
#define left_servo_pin  6
#define right_led       7
#define left_led        8
#define left_qti        A0
#define middle_qti      A1
#define right_qti       A2
#define ir_receiver     A3
#define min_pulse       1300
#define max_pulse       1700
#define standstill      1500
#define qti_threshold   0

/* Global variables ------------------------------------------ */
Servo g_left_wheel;
Servo g_right_wheel;
signed int g_ir_buf[32];    // Infrared data buffer
signed int g_ir_index = 0;  // Infrared loop counter variable
bool online = true;
unsigned long g_last_command;

/* Private functions ------------------------------------------------- */
int irReceive () {
  while (pulseIn(ir_receiver, HIGH) > 4000) { // Wait for a start bit
    Serial.println("Start bit received");
    for (byte i = 0; i <= 31; i++) {
      g_ir_buf[i] = pulseIn(ir_receiver, HIGH); // Write all the infrared signal data to variable
    }
    Serial.println("IR buffer populated with timings");
    g_ir_index = 0;
    /* Only for serious debugging */
    /*
      for (byte i = 0; i <= 31; i++) {
      Serial.println(g_ir_buf[i]);
      }
    */
    for (byte i = 0; i <= 31; i++) {
      if (g_ir_buf[i] > 1000) {
        g_ir_buf[i] = 1;
        g_ir_index++;
      } else if (g_ir_buf[i] > 0) {
        g_ir_buf[i] = 0;
        g_ir_index++;
      } else {
        Serial.println("Wrong bit received");
      }
    }
    
    Serial.println("IR buffer translated to binary");
    Serial.print("Number of bits received: ");
    Serial.println(g_ir_index);
    
    /* Check if determining what button was pressed was successful and if it was then stop the loop,
	   otherwise display error and continue the loop */
    if (check()) {
      return 1;
    }
    else {
      Serial.println("check() was not sucessful");
    }
    Serial.println("Waiting for start bit");
  }

  return 0;
}

void irSerialPrint(int bits[]) {
  Serial.println("------ Bit Correction Check ------");
  for (byte i = 0; i <= 7; i++) { // Print out the range of 16-31 bits of infrared signal buffer
    
	/*
	Serial.print("Bit ");
    Serial.print(i + 16);
    Serial.print(" = ");
    Serial.println(bits[i]);
    */
	
    Serial.print("Bit ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(g_ir_buf[i]);
    Serial.print(" | ");
    Serial.println(g_ir_buf[i + 8]);
    
    
  }
}

bool check() { // Check the received bits

    int button = 0;
    int bits[8];
    
	/* Chech the bits 16-23 and check if they are opposite of bits 24-31 */
    for (byte i = 16; i <= 23; i++) {
      if (g_ir_buf[i] != g_ir_buf[i + 8]) {
        bits[i - 16] = 1;
      }
      else {
        bits[i - 16] = 0;
      }
    }
	
    irSerialPrint(bits); //Print out the bits
	
	/* Determine what button on the remote was pressed */
    //Mute
    if (bits[4] == 1 && bits[6] == 1) {
      Serial.println("[Mute] button");
      button = 1;
    }
    //TV
    else if (bits[4] == 1 && bits[5] == 1) {
      Serial.println("[TV] button");
      button = 1;
    }
    //OnOff
    else if (bits[4] == 1 && bits[6] == 0) {
      Serial.println("[ON/OFF] button");
      button = 1;
    }
    //CH UP
    else if (bits[2] == 1 && bits[6] == 1) {
      Serial.println("[Channel up] button");
      button = 1;
    }
    //CH Down
    else if (bits[2] == 1 && bits[6] == 0) {
      Serial.println("[Channel down] button");
      button = 1;
    }
    //Vol left
    else if (bits[3] == 1 && bits[6] == 1) {
      Serial.println("[Volume Left] button");
      button = 1;
    }
    //Vol right
    else if (bits[3] == 1 && bits[6] == 0) {
      Serial.println("[Volume Right] button");
      button = 1;
    }
     

  return button; // return 1 if the the button was successfuly determined
}

/* Arduino functions ---------------------------------------------------------------- */
void setup() {
  /* Start serial monitor */
  Serial.begin(9600);

  /* Set infrared receiver pin to input mode*/
  pinMode(ir_receiver, INPUT);

  Serial.println("Starting irReceive()");
  Serial.println("Waiting for start bit");


}


void loop() {
  irReceive();
}
