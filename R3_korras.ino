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

/* Includes ----------------------------------------------------- */
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24Network.h>
#include <RF24.h>




/* User defines ------------------------------------------------------ */
#define node_address 051
#define radio_channel 127
#define qti_threshold   230
#define DEBUG

/* Defines ------------------------------------------------------ */
#define button_pin 2
#define sonic_echo_pin 3
#define sonic_trig_pin 4
#define right_servo_pin 5
#define left_servo_pin 6
#define right_led 7
#define left_led 8
#define left_qti A0
#define middle_qti A1
#define right_qti A2
//#define ir_receiver A3
#define buzzer_pin A3
#define min_pulse 1300
#define max_pulse 1700
#define standstill 1500


/* Global variables -------------------------------------------- */
byte g_last_button_state = 0;
byte g_button_state = 0;
unsigned long g_last_debounce_time = 0;
unsigned long g_debounce_delay = 100;
uint16_t incomingData = 0;
/* Possibly unneeded variables */
bool g_online = false;
unsigned long g_last_command;
float g_distance_in_cm = 0;
bool g_repeat = true;
byte g_pin_states[] = {0, 0};
float distance = 0;
/* Possibly needed */
byte tempo = 250; // Initial song speed
int i = 0;
bool beginning = false;
char start = 0;

int index = 0;
int note = 0;
int duration = 0;
int node = 0;
int checksum = 0;
int buf[11];
char startdelay = 0;
char packet = 0;
long freq = 0;
bool led = false;
int len;
char gotmsg[32] = "";

//LCD variables
byte charcount = 0;
boolean secondline = false;
byte textCounter = 0;

/*
    Create an RF24 instance called "radio" by calling a constructor RF24
    The instance uses pins CE = 9 and CSN = 10
    Library functions are available from https://tmrh20.github.io/RF24/classRF24.html
*/
RF24 g_radio(9, 10);               // Pin setup for UNO
RF24Network network(g_radio);      // Include the radio in the network


/* Address through which the module communicates */
const uint16_t this_node = node_address;   // Address of our node in Octal! format ( 04,031, etc)
const uint16_t master = 00;    // Address of the other node in Octal! format

/* Songs -------------------------------------------------------- */

/* Private functions -------------------------------------------- */


void setLed(byte value_left = LOW, byte value_right = LOW)
{
  g_pin_states[1] = value_right;
  digitalWrite(right_led, value_right);
  g_pin_states[0] = value_left;
  digitalWrite(left_led, value_left);
}


/* Radio instance setup function, don't change them */
bool radioSetup() {
  /* Start the radio instance */
  if (!g_radio.begin()) return 0;
  /* Checking if the radio is connected */
  if (!g_radio.isChipConnected()) return 0;

  network.begin(radio_channel, this_node);  //(channel, node address)
  g_radio.setDataRate(RF24_250KBPS); // RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  g_radio.setPALevel(RF24_PA_LOW); // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  g_radio.setAutoAck(false);

  return 1;
}


/* Arduino functions -------------------------------------------- */
void setup()
{


  /* Start serial monitor */
  Serial.begin(9600);
  /* Initialize buzzer pin */
  pinMode(buzzer_pin, OUTPUT);
  /* Set the pin mode of LED pins as output */
  pinMode(right_led, OUTPUT);
  pinMode(left_led, OUTPUT);

  /* Set button pin as input */
  pinMode(button_pin, INPUT);

  /* Call radio setup, if failed then infinite loop */
  if (!radioSetup()) while (1);
#ifdef DEBUG
  Serial.println("Radio initialized.");
#endif

  /* Inform about the end of the setup() function */
  Serial.println("--- Ardunio ready ---\n");
}

void loop()
{
  /* To keep the radio network running */
  network.update();
  /* Setting default header, don't change */
  RF24NetworkHeader header_master;

  
  while (network.available() ) {
    network.read(header_master, &incomingData, sizeof(incomingData)); // Read the incoming data to integer
	
	/* Check if the incoming data is the index */
    if (incomingData < 120)
    {
      index = incomingData;
      Serial.println(index);
    }
	
	/* Check if the sum of the audio packet and the index match the sum recived from the incoming data */
    else if (node + index == incomingData || node - index == incomingData)
    {
      Serial.println("Õige");	//Inform if the correct index was displayed
      Serial.println("");
	  
      decoder(node); 	//Decode the audio packet
      playToneRadio(note, duration * tempo);	//Play the note
	  
	  /* Change LEDs whenever a new note is played*/
      if (led == false)
      {
        setLed(LOW, HIGH);
        led = true;
      }
      else
      {
        setLed(HIGH, LOW);
        led = false;
      }
    }
	
	/* Check if the incoming data is the audio packet or sum of index and audio packet */
    else if (incomingData >= 8192)
    {
      node = incomingData;
    }

  }

}

/* A function for decoding the data */
bool decoder(uint16_t packet) {
  uint16_t byte1 = (packet >> 12);
  uint16_t byte2 = packet & 0xfff;
  duration = byte1;
  note = byte2;
  return 1;
}

/* A function for playing the note */
void playToneRadio(int noteTone, int duration)
{
  tone(buzzer_pin, noteTone, duration);

}
