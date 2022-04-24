
/**Notice:
(function keys for the meter unit)
A - Enter key
B - Clear key

D - update button

**/

// library instantiations

// include libraries for the 128x32 oled display
#include <SPI.h> //has to be included according to the new library that will be used for the oled
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <EEPROM.h>
#include <Keypad.h>
#include <PZEM004Tv30.h>
#include <nRF24L01.h>
#include <RF24.h>
// #include <RTClib.h>
#include <DS3232RTC.h> // https://github.com/JChristensen/DS3232RTC

#define OLED_RESET 4        // dont know what this is for but it is important to be included
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define OLED_RESET 4        // dont know what this is for but it is important to be included
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// define the values to be used in the keypad
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {A0, A1, A2, A3};

// code for initializing object for RTC
DS3232RTC myRTC;
// code for initializing object using new library
Adafruit_SH1106 display(OLED_RESET);
// object for PZEM 004T
PZEM004Tv30 pzem(11, 12); // TX, RX
// object for RF module
RF24 radio(9, 10); // CE, CSN
// object for keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
// Setting the two addresses. One for transmitting and one for receiving
// const byte address[][6] = {"00001", "00002"};
const byte meterTOmain_address[6] = "00001";
const byte mainTOmeter_address[6] = "00002";
/**
 * 00001 - meter unit >> main unit
 * 00002 - main unit >> meter unit
 */
// object for real-time clock
// RTC_DS3231 rtc;

#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif

// Variable declarations ------------------------------------

#define relayPin 7 // used for the relay pin

char user_input[4];            // array for getting the user input; this is the array where the values are stored
int addressOnEEPROM = 0;       // variable used in determining the address when looping through the eeprom memory
short setCursor_column = 0;    // variable used as reference for the collumn when the user will be inputing the passcode
short fixedNumberOfInputs = 0; // variable used as reference to control the number of inputs from the user
char codeFromEEPROM[4];        // array used where the retrieved values from eeprom are stored
bool measureMode = false;      // variable indicator to measure mode or not

// char keyValue = customKeypad.getKey();  //used for getting value when pressing the keypad

// variables used for millis on measuring functions
const long thisInterval = 10000;
unsigned long previousMillis = 0;
// millis for determining the minute
const long relayCutoff_Interval = 60000;    // time limit for checking if there is an electrical load in the AC circuit
unsigned long determine_minuteInterval = 0; //

// variable to be used on the measuring energy function, for keeping a timestamp if the module's already recorded data
float timestamp_Energy = 0;
// boolean variable for determining if the unit should be sending data
boolean sendRF_mode = false;
String RF_message = "";           // contaianer variable for the message to send through RF
String this_userContact = "";     // container variable for the
boolean oled_active = true;       // trigger variable indicator for the oled display
unsigned long oled_timestamp = 0; // variable to capture the timestamp instance for oled

// variable declaration that deals with updating the user data : Main -> Meter transmittion
String passcodeReceived[20];
int receivePass_index = 0;    //variable used as index reference for receiving data from NRF

String registered_passcode[20]; // variable used to hold the retrieved data from the eeprom memory


// pre define passcodes: hardcoded
// String predef_passcodes[] = {"9664", "9333"};
String predef_passcodes[] = {"9644", "2923", "5373", "2267", "5906"};

//  = = = = = = = = = = = = set up and loop code begins here  = = = = = = = = = = = =
void setup()
{

  Wire.begin();
  Serial.begin(9600);

  radio.begin();
  radio.setAutoAck(false);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);

  // radio.openReadingPipe(1, address[0]);

  /**
   * code for meter >> main
   *     radio.openWritingPipe(address[1]);
   *     radio.stopListening();
   */

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  // RTC
  myRTC.begin();
  setSyncProvider(myRTC.get); // the function to get the time from the RTC
  if (timeStatus() != timeSet)
  {
    Serial.println("Unable to sync with the RTC");
  }
  else
  {
    Serial.println("RTC has set the system time");
  }

  showMeterUnit();
  fetchDataFromEEEPROM();
  pinMode(relayPin, OUTPUT); // setup from relay
  oled_timestamp = millis(); // for oled_timestamp reference
}
