
/**Notice:

(function keys for the meter unit)
A - Enter key
B - Clear key
C - End operations
D - Update

**/
// #include <OnewireKeypad.h>
//include libraries for the 128x32 oled display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <Keypad.h>
/**supporting codes------------**/
//code to support counting the array size
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])
/**------------------------------**/
//define the screen resolutions
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1   //dont know what this is for but it is important to be included
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
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
//object for the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//object for keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
//Variable declarations ------------------------------------

char user_input[4];
int addressOnEEPROM= 0;
short   setCursor_column = 0;
short fixedNumberOfInputs = 0;
char codeFromEEPROM[4];



//memthods
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
void showMessage(String message){
display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(4,10);
  display.print(message);
  display.display();
  delay(4000);
  display.clearDisplay();
}

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
  if(matchTrigger == false){
    showMessage("Error");
    addressOnEEPROM =1;
  }else{
    showMessage("Matched");
    addressOnEEPROM =1;
  }

}
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

void setup() {
  Wire.begin();
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  showMeterUnit();
  readCodesFromEEEPROM();
}

void loop() {

  clearScreen:
  // put your main code here, to run repeatedly:
  showInputPasscode();
  char keyValue = customKeypad.getKey();
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

      default:
        if(fixedNumberOfInputs < 4){
            user_input[fixedNumberOfInputs] = keyValue;
            showsUser_input(keyValue);
            keyValue = 0x00;
            fixedNumberOfInputs++;
        }
      break;
    }
  }


}
