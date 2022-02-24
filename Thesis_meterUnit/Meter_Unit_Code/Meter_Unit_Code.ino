/**Notice:
(function keys for the meter unit)
A - Enter key
B - Clear key
C - End operations
D - Update
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

#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif


//object for keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
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


 
//PRE DEFINED VALUES: {"1157","3727","6501","6698"}


//methods
void showMeterUnit(){
  //clear the screen first
  display.clearDisplay();
  //set text color
  display.setTextColor(WHITE);
  //set text size
  display.setTextSize(2);
  //set the cursor coordinates
  display.setCursor(4,10);
  display.print("Meter Unit");
  //display the "meter unit" text
  display.display();
  delay(4000);
  //clear the oled display
  display.clearDisplay();
}

void showInputPasscode(){
  // display.clearDisplay();
  //set Text size
  display.setTextSize(1);
  //set the cursor coordinates
  display.setCursor(0,0);
  display.println("Input Passcode:");
  display.display();
}

//method used to read stored codes in eeprom
void readCodesFromEEEPROM(){
  String code = "";
  int indexCounter = 0;
  addressOnEEPROM +=1;
  if(EEPROM.read(0) <= 0){
    Serial.println("No memories on eeprom");
  }else{
    while(EEPROM.read(addressOnEEPROM) > 0){
      char codesArray[EEPROM.read(addressOnEEPROM)];
      for(int x = 0; x < EEPROM.read(addressOnEEPROM); x++){
        codesArray[x] = EEPROM.read(addressOnEEPROM+(x+1));
      }
      Serial.println(String(codesArray));

      addressOnEEPROM+=5;
      indexCounter++;
    }
  }

  addressOnEEPROM = 1;
}

//method used to display text on the oled display to show the inputs from the user
void showsUser_input(char key){
  // //set text color
  display.setTextColor(WHITE);
  //set text size
  display.setTextSize(2);
  //set the cursor coordinates
  display.setCursor(setCursor_column,15);
  display.print(key);
  setCursor_column += 12;
  // user_Input += key;
  display.display();
}

//This method is called when displaying messages: (Access granted and Access Denied)
void showMessage(String message){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(4,10);
  display.print(message);
  display.display();
  delay(2000);
  display.clearDisplay();
}

// this method gets the user input and compares it to the passcode values retrieved from the eeprom
void checkInputAndDecide(){
  String compareThis, input;
  for(int x=0; x < ARRAY_SIZE(user_input); x++){
    input += user_input[x];
  }
  boolean matchTrigger = false;
  while(EEPROM.read(addressOnEEPROM)> 0){
    compareThis = readStringFromEEPROM(addressOnEEPROM);
    if(compareThis == input){
      matchTrigger = true;
      break;
    }
    addressOnEEPROM+=5;
  }

  //deciding action to determine the mode of the meter unit (Measure mode: ON / OFF)
  if(matchTrigger == false){
    showMessage("Error");
    addressOnEEPROM =1;
    measureMode = false;
    
  }else{
    digitalWrite(relayPin, HIGH);
    showMessage("Matched");
    addressOnEEPROM =1;
    measureMode =true;
    delay(1000);
    //get the accumulated measurement from the internal memory of the PZEM 004T
    timestamp_Energy = pzem.energy();
    //snapshot millis time for checking every minute
    
    
    
  }

}
//method that returns the value from eeprom that is retrieved
String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++){
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
  return String(data);
}

//method to send the energy consumption data into the main unit

//method code for PZEM 004T to display the energy computed from the library
void measureEnergy(float current, float voltage, float power, float energy){
  //for voltage display on serial
  if(voltage != NAN){
    Serial.print("Voltage: ");
    Serial.print(voltage,9);
    Serial.println("V");
  } else {
    Serial.println("Error reading voltage");
  }

  //for Power display on serial
  if(current != NAN){
    Serial.print("Power: "); 
    Serial.print(power,9); 
    Serial.println("W");
  } else {
    Serial.println("Error reading power");
  }

  //for current display on serial
  if(current != NAN){
    Serial.print("Current: ");
    Serial.print(current,9);
    Serial.println("A");
  } else {
    Serial.println("Error reading current");
  }

  //for current energy display on serial
  if(current != NAN){
    Serial.print("Current Energy consumption: ");
    Serial.print(energy,9); 
    Serial.println("kWh");
  } else {
    Serial.println("Error current reading energy");
  }
  //for accumulated energy display on serial
  if(current != NAN){
    Serial.print("Accumulated Energy consumption: ");
    Serial.print(energy + timestamp_Energy,9); 
    Serial.println("kWh");
  } else {
    Serial.println("Error current reading energy");
  }

  Serial.println();
  // delay(2000);

  //displaying the values on OLED
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(2,1);
  display.print("Energy: "+String(energy,7)+" kWh");

  display.setTextSize(1);
  display.setCursor(2,15);
  display.print("Voltage: "+String(voltage,5)+" V");

  display.setTextSize(1);
  display.setCursor(2,28);
  display.print("Power: "+String(power,5)+" W");

  display.setTextSize(1);
  display.setCursor(2,41);
  display.print("Current: "+String(current,5)+" A");

  display.display();

  //code to determine if the energy has changed for 1 minute
  if (millis() < relayCutoff_Interval){
      determine_minuteInterval = millis();
  }
  else if (millis() - determine_minuteInterval >= relayCutoff_Interval){
    determine_minuteInterval = millis();  
    //if the energy consumption remains unchanged for a minute, then the system trips the relay
    if(power < 1){
      //trip the relay off
      Serial.print("Cutoff time has passed!");
      digitalWrite(relayPin, LOW);
      measureMode = false;
      display.clearDisplay();
    }       
  }     
    
}



//  = = = = = = = = = = = = set up and loop code begins here  = = = = = = = = = = = =
void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  showMeterUnit();
  readCodesFromEEEPROM();
  pinMode(relayPin, OUTPUT);  //setup from relay
}



void loop() {

  clearScreen:
  // put your main code here, to run repeatedly:
  char keyValue = customKeypad.getKey();

  //setting mode for the system to execute
  if (measureMode == true ){ 
    float current = pzem.current();
    float voltage = pzem.voltage();
    float power = pzem.power();

    float energy = (pzem.energy() - timestamp_Energy);
   
   //code for dealing with millis rollover that may cause possible bugs
    if (millis() < thisInterval){
      previousMillis = 0;
    }
    else if (millis() - previousMillis >= thisInterval){
      previousMillis = millis();
      measureEnergy(current, voltage, power, energy);

    }
    
  }
  else{
    showInputPasscode();
    if(keyValue){
      switch(keyValue){
        case 'B':
            display.clearDisplay();
            setCursor_column = 0;
            keyValue = 0x00;
            memset(user_input, 0, sizeof(user_input));
            fixedNumberOfInputs =0;
            goto clearScreen;
        break;

        case 'A':
            checkInputAndDecide();
            setCursor_column = 0;
            keyValue = 0x00;
            memset(user_input, 0, sizeof(user_input));
            fixedNumberOfInputs =0;
            goto clearScreen;
        break;
        
        //pressing *D means to end the measuring process of the unit
        case 'D':
            // //Reset all values
            // display.clearDisplay();
            // setCursor_column = 0;
            // keyValue = 0x00;
            // memset(user_input, 0, sizeof(user_input));
            // fixedNumberOfInputs =0;
            // measureMode = false;
            // previousMillis = 0;


            // //Enter code for sending final data here
            // goto clearScreen;

        break;

        default:
          // will not accept user input from user once the fixedNumberOfInputs reaches 4
          if(fixedNumberOfInputs < 4){
              user_input[fixedNumberOfInputs] = keyValue;
              showsUser_input(keyValue);
              keyValue = 0x00;
              fixedNumberOfInputs++;
          }
        break;
      }
    }
    // digitalWrite(relayPin, LOW);
  }


}
