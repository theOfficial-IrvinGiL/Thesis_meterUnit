

#include <OnewireKeypad.h>  /*the library for the keypad adapter component*/

/*key values declarations*/
char KEYS[] = {
  '1', '2', '3', 'A',
  '4', '5', '6', 'B',
  '7', '8', '9', 'C',
  '*', '0', '#', 'D'
};


OnewireKeypad <Print, 16 > KP2(Serial, KEYS, 4, 4, A0, 4700, 1000, 10000);


/*--------define the values here---------------*/

String defaultUser = "09260220908", defaultPassword = "2354";




/*---------------------------------------------*/

/*setup method*/
void setup () {
  Serial.begin(115200);

  KP2.SetHoldTime(0);
  // This method is set in the constructor with a default value of 5.0
  // You only need to include this method if your Arduino is not supplying 5v to
  // the keypad. ie. ~4.7v or even with 3.3v Arduino boards too.


  KP2.SetKeypadVoltage(5.0);
  //KP2.SetKeypadVoltage(4.95);
}


/*loop method*/
void loop(){
  keypadFunctions();
}

/*method for the keypad function*/
void keypadFunctions(){
  /*the key value gets the value from the KEY declarations above*/
  if ( char key = KP2.Getkey() ) {
    switch (KP2.Key_State()) {
      case PRESSED:
      Serial << "Key: " << key << " State: ";
      Serial.println("PRESSED");
      break;
//      case RELEASED:
//      Serial << "Key: " << key << " State: ";
//      Serial.println("RELEASED");
//      break;
//      case HELD:
//      Serial << "Key: " << key << " State: ";
//      Serial.println("HOLDING");
//      break;
    }

}

}
