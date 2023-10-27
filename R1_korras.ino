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
#define right_servo_pin 5
#define left_servo_pin  6
#define right_led       7
#define left_led        8
#define left_qti        A0
#define middle_qti      A1
#define right_qti       A2
#define min_pulse       1300
#define max_pulse       1700
#define standstill      1500
#define qti_threshold   410 

/* Global variables ------------------------------------------ */
Servo g_left_wheel;
Servo g_right_wheel;
int ring = 0;											//Which lap is it (0/1)

/* Private functions ------------------------------------------------- */
byte readQti (byte qti) {                               // Function to read current position on map
  digitalWrite(qti, HIGH);                              // Send an infrared signal
  delayMicroseconds(1000);                               // Wait for 1ms, very important!
  digitalWrite(qti, LOW);                               // Set the pin low again
  return ( analogRead(qti) > qti_threshold ? 1 : 0);    // Return the converted result: if analog value more then 100 return 1, else 0
}

void setWheels(int delay_left = 1500, int delay_right = 1500) {
  g_left_wheel.writeMicroseconds(delay_left);
  g_right_wheel.writeMicroseconds(delay_right);
  delay(20);
}
void setLed(byte value_left = LOW, byte value_right = LOW) {
  digitalWrite(right_led, value_right);
  digitalWrite(left_led, value_left);
}


/* Makes the robot move backwards while the LEDs are off */
void tagasi() {
  setLed(LOW, LOW);
  setWheels(1450, 1550);
}

/* Makes the robot move forward while the LEDs are off */
void edasi() {
  setLed(LOW, LOW);
  setWheels(1550, 1450);
}

/* Makes the robot move right while the right LED is on */
void paremale() {
  setLed(LOW, HIGH);
  setWheels(1600, 1500);
}

/* Makes the robot move left while the left LED is on */
void vasakule() {
  setLed(HIGH, LOW);
  setWheels(1500, 1400);
}


/* Arduino functions ---------------------------------------------------------------- */
void setup() {

  /* Sets the lap count to 0 */
  ring = 0;
  
  /* Start serial monitor */
  Serial.begin(9600);

  /* Set the pin mode of LED pins as output */
  pinMode(right_led, OUTPUT);
  pinMode(left_led, OUTPUT);

  /* Attach servos to digital pins defined earlier */
  g_left_wheel.attach(left_servo_pin);
  g_right_wheel.attach(right_servo_pin);

  /* Initiate wheels to standstill */
  setWheels();

  /* Blinking LEDs for test */
  setLed(HIGH, HIGH);
  delay(500);
  setLed();
  delay(500);

}

void loop() {

  /* Start reading QTI values and adjust wheels accordingly */
  
  /* If both sensors see black then for the first lap it will stop for a moment blink its LEDs and continue moving
	 and if it's the second lap it will stop moving entirely */
  if (readQti(left_qti) && readQti(right_qti)) {
    if (ring == 0) {
      setWheels(1500, 1500);  
      setLed(HIGH, HIGH);
      delay(500);
      setLed();
      delay(500);
      ring++;
      edasi();
      delay(400);
    }
    else {
      setWheels(1500, 1500);        
      setLed(HIGH, HIGH);
    }
               
  } 
  
  /* If the left sensor sees white and the right sensor sees black then it will move back for a moment and then move right
     and if the right sensor instead sees white it will move forward  */
  else if (readQti(left_qti) == 0) {
    if (readQti(right_qti) == 1) {
      tagasi();
      delay(100);
      paremale();
      delay(150);
    }
    else {
      edasi();
    }
  }
  
  /* If the left sensor sees black and the right sensor sees white then it will move back for a moment and then move left */
  else if (readQti(left_qti) == 1) {
    if (readQti(right_qti) == 0) {
      tagasi();
      delay(100);
      vasakule();
      delay(200);
    }

  }

}
