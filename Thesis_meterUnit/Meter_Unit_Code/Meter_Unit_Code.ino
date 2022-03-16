

/**Notice:
(function keys for the meter unit)
A - Enter key
B - Clear key

**/
// #include <OnewireKeypad.h>
//include libraries for the 128x32 oled display
#include <SPI.h>    //has to be included according to the new library that will be used for the oled
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <EEPROM.h>
#include <Keypad.h>
#include <PZEM004Tv30.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RTClib.h>//needs library

/**supporting codes------------**/
//code to support counting the array size
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])
/**------------------------------**/
#define OLED_RESET 4   //dont know what this is for but it is important to be included
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// define the values to be used in the keypad
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {A0, A1, A2, A3};

//code for initializing object using new library
Adafruit_SH1106 display(OLED_RESET); 
// object for PZEM 004T 
PZEM004Tv30 pzem(11, 12); //TX, RX
//object for RF module
RF24 radio(9, 10); // CE, CSN
//object for keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
//Setting the two addresses. One for transmitting and one for receiving
const byte addresses [][6] = {"00001", "00002"};  
//object for real-time clock
RTC_DS3231 rtc;

#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif


//Variable declarations ------------------------------------

#define relayPin 7  //used for the relay pin

char user_input[4];   //array for getting the user input; this is the array where the values are stored
int addressOnEEPROM= 0; //variable used in determining the address when looping through the eeprom memory
short setCursor_column = 0;   //variable used as reference for the collumn when the user will be inputing the passcode
short fixedNumberOfInputs = 0;  //variable used as reference to control the number of inputs from the user
char codeFromEEPROM[4];   //array used where the retrieved values from eeprom are stored
bool measureMode = false; //variable indicator to measure mode or not

//variables used for millis on measuring functions
const long thisInterval = 2000;
unsigned long previousMillis = 0;
  //millis for determining the minute
  const long relayCutoff_Interval = 20000;
  unsigned long determine_minuteInterval = 0;

//variable to be used on the measuring energy function
float timestamp_Energy = 0;
//boolean variable for determining if the unit should be sending data
boolean sendRF_mode = false;
String RF_message = ""; //vessel variable for the message to send through RF
String this_userContact = "";

 
//PRE DEFINED VALUES: {"1157","3727","6501","6698"}


//method to send the energy consumption data into the main unit



//  = = = = = = = = = = = = set up and loop code begins here  = = = = = = = = = = = =
void setup() {
  Wire.begin();
  Serial.begin(9600);
  nrfSetup();
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  showMeterUnit();
  readCodesFromEEEPROM();
  pinMode(relayPin, OUTPUT);  //setup from relay
}
