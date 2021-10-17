




// #include <OnewireKeypad.h>
//include libraries for the 128x32 oled display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Keypad.h>

//define the screen resolutions
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define OLED_RESET -1
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




//Variable declarations
char keyValue;

int setCursor_column = 0;




void setup() {
  Wire.begin();


  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

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

/**Reminder: in the code setCursor(0,0). The placement is represented as Columns/Rows*/

void loop() {
  if_clearPasscode:


  introText();

  keyValue= customKeypad.getKey();

  if (keyValue){
    switch (keyValue) {
      case 'C':
      display.clearDisplay();
      setCursor_column = 0;

      //goto clearpasscode label
      goto if_clearPasscode;
      break;

      default:
      show_UserInput(keyValue);
      break;
    }

  }





  display.display();
}

//display on screen
void show_UserInput(char key){

  // //set text color
  // display.setTextColor(WHITE);
  //set text size
  display.setTextSize(2);
  //set the cursor coordinates
  display.setCursor(setCursor_column,15);
  display.print(key);
  setCursor_column += 12;

}

//method for intro text
void introText(){
  // display.clearDisplay();
  //set Text size
  display.setTextSize(1);
  //set the cursor coordinates
  display.setCursor(0,0);
  display.println("Input Passcode:");

}
