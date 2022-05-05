// methods that are responsible for background process functions are written here:


// method for resetting the memory of the user_input array
void userInput_memset()
{
  for (int x = 0; x < sizeof(user_input); x++)
  {
    user_input[x] = "";
  }
}

// this method gets the user input and compares it to the passcode values retrieved from the eeprom
void checkInputAndDecide()
{
  String input;
  for (int x = 0; x < sizeof(user_input); x++)
  {
    input += user_input[x];
  }
  boolean matchTrigger = LOW;

  // for (int x = 0; x < sizeof(registered_passcode); x++)
  // {
  for (int y = 0; y < 20; y++)
  {
    // if (String(user_input) == predef_passcodes[y])
    // if (input == registered_passcode[x])
    String var = readStringFromEEPROM(y * 4);
    if (input == var)
    {
      matchTrigger = HIGH;
      previousMillis = millis();
      // input = this_userContact;
      this_userContact = input;
      break;
      break;
    }
    else if (input == "DDDD")
    {
      // clear eeprom
      for (int i = 0; i < EEPROM.length(); i++)
      {
        EEPROM.write(i, 0);
      }
      
      receiveProcess();
      break;
    }
    else
    {
      matchTrigger = LOW;
    }
  }


  // deciding action to determine the mode of the meter unit (Measure mode: ON / OFF)
  if (matchTrigger == LOW)
  {
    showMessage("Error");
    display.clearDisplay();
    display.display();
    showInputPasscode();
    // addressOnEEPROM = 1;
    measureMode = LOW;
    // memset(user_input, 0, sizeof(user_input));
  }
  else if (matchTrigger == HIGH)
  {
    digitalWrite(relayPin, HIGH);
    showMessage("Matched");
    // addressOnEEPROM = 1;
    measureMode = HIGH;

    // input = this_userContact;

    // get the accumulated measurement from the internal memory of the PZEM 004T
    delay(1000);
    timestamp_Energy = pzem.energy();
    // snapshot millis time for checking every minute
  }
}

// method code for PZEM 004T to display the energy computed from the library
void measureEnergy(float current, float voltage, float power, float energy)
{

  // display the readed values on oled
  showEnergy_onOled(String(energy, 3), String(voltage, 5), String(power, 5), String(current, 5));

  // checks the power on the circuit if there is load connected
  if (millis() - previousMillis >= 10000)
  {
    if (power <= 0)
    {
      // RF_setupSend(); // setup the RF for the sending functionality
      // turn the relays off after 10 seconds
      showMessage("No Load connected."); // show text message on OLED
      showMessage("No Device connected.");

      String currentEnergy = String(energy, 3); // process to concatinate the energy consumption data into the RF message
      concatDateTime(currentEnergy, this_userContact);

      digitalWrite(relayPin, LOW); // turn off the Relay (cut the connection on the AC circuit)
      sendTo_main(RF_message);     // call function to send the produced message to the main unit

      showMessage(RF_message); // show the constructed message to the OLED DISPLAY
      // reset the modes and variables that were involved in the measuring mode of the unit
      display.clearDisplay();
      measureMode = LOW;
      RF_message = "";
      this_userContact = "";

      // reset the variables and memories involved in the input passcode process
      display.clearDisplay();
      // resetForReMeasure();
      setCursor_column = 0;
      memset(user_input, 0, sizeof(user_input));
      fixedNumberOfInputs = 0;
      display.clearDisplay();
      display.display();
    }
    previousMillis = millis();
  }
}

// code for concatinating the time and date into the RFmessage variable
void concatDateTime(String energy_consumed, String passcode)
{

  RF_message += passcode;
  RF_message += "_"; // buffer character

  RF_message += String(month()); // adding the date
  RF_message += "/";
  RF_message += String(day());
  RF_message += "/";
  RF_message += String(year());

  RF_message += "_"; // buffer character

  RF_message += String(hour()); // adding the time
  RF_message += ":";
  RF_message += String(minute());

  RF_message += "_";             // buffer character
  RF_message += energy_consumed; // concat the current energy consumed data

  
}

// method for sending data over RF to main unit
void sendTo_main(String message)
{
  radio.stopListening();
  unsigned long broadcastStart = millis(); // timestamp broadcast start time

  int messageLength = message.length();
  char data[messageLength];

  /**
     gets data from global message variable and copies it to data
     array for sending through RF
  */
  for (int x = 0; x < messageLength; x++)
  {
    data[x] = message[x];
  }

  while (millis() - broadcastStart <= 2000)
  {
    radio.write(&data, sizeof(data)); // Sending the data
    delay(100);
  }
  oled_timestamp = millis();
}


// method for reset index and array function
void resetPass_arrayxindex()
{
  // clear the received array
  for (int x = 0; x < sizeof(passcodeReceived); x++)
  {
    passcodeReceived[x] = "";
  }
  // load data from eeprom
  // fetchDataFromEEEPROM();
}

// reset function
void (*resetFunc)(void) = 0;