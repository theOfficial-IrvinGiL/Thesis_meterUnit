// methods that are responsible for background process functions are written here:

// void resetForReMeasure()
// {
//   setCursor_column = 0;
//   keyValue = 0x00;
//   memset(user_input, 0, sizeof(user_input));
//   fixedNumberOfInputs = 0;
// }

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
  for (int y = 0; y < sizeof(predef_passcodes); y++)
  {
    // if (String(user_input) == predef_passcodes[y])
    // if (input == registered_passcode[x])
    String var = predef_passcodes[y];
    if (input == var)
    {
      matchTrigger = HIGH;
      previousMillis = millis();
      // input = this_userContact;
      this_userContact = input;
      break;
      break;
    }
    else
    {
      matchTrigger = LOW;
    }
  }
  // }

  // while (EEPROM.read(addressOnEEPROM) > 0)
  // {
  //   compareThis = readStringFromEEPROM(addressOnEEPROM);
  //   // if (compareThis == input)
  //   if (input == "1157")
  //   {
  //     matchTrigger = true;
  //     previousMillis = millis();
  //     break;
  //   }
  //   addressOnEEPROM += 5;
  // }

  // deciding action to determine the mode of the meter unit (Measure mode: ON / OFF)
  if (matchTrigger == LOW)
  {
    showMessage("Error");
    display.clearDisplay();
    // addressOnEEPROM = 1;
    measureMode = LOW;
    // memset(user_input, 0, sizeof(user_input));
  }
  else
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
  // for voltage display on serial
  // if (voltage != NAN)
  // {
  //   Serial.print("Voltage: ");
  //   Serial.print(voltage, 9);
  //   Serial.println("V");
  // }
  // else
  // {
  //   Serial.println("Error reading voltage");
  // }

  // // for Power display on serial
  // if (current != NAN)
  // {
  //   Serial.print("Power: ");
  //   Serial.print(power, 9);
  //   Serial.println("W");
  // }
  // else
  // {
  //   Serial.println("Error reading power");
  // }

  // // for current display on serial
  // if (current != NAN)
  // {
  //   Serial.print("Current: ");
  //   Serial.print(current, 9);
  //   Serial.println("A");
  // }
  // else
  // {
  //   Serial.println("Error reading current");
  // }

  // // for current energy display on serial
  // if (current != NAN)
  // {
  //   Serial.print("Current Energy consumption: ");
  //   Serial.print(energy, 9);
  //   Serial.println("kWh");
  // }
  // else
  // {
  //   Serial.println("Error current reading energy");
  // }
  // // for accumulated energy display on serial
  // if (current != NAN)
  // {
  //   Serial.print("Accumulated Energy consumption: ");
  //   Serial.print(energy + timestamp_Energy, 9);
  //   Serial.println("kWh");
  // }
  // else
  // {
  //   Serial.println("Error current reading energy");
  // }

  // Serial.println();
  // delay(2000);

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
    }
    previousMillis = millis();
  }
}

// code for concatinating the time and date into the RFmessage variable
void concatDateTime(String energy_consumed, String passcode)
{
  // DateTime Thisnow = rtc.now();
  /**
  *digital clock display of the time
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(' ');
    Serial.print(day());
    Serial.print(' ');
    Serial.print(month());
    Serial.print(' ');
    Serial.print(year());
    Serial.println();
  */

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

  // RF_message += "_"; // buffer character
  // RF_message += passcode;
  // RF_message += "1157";
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

  //  const char data[] = "First Data";
  /**
  *const char text[] = "WASSUP";
    radio.write(&text, sizeof(text));
    Serial.println(text);
    delay(800);
  */
  while (millis() - broadcastStart <= 2000)
  {
    radio.write(&data, sizeof(data)); // Sending the data
    delay(100);
  }
  oled_timestamp = millis();
}

/**
 * method to recieve RF message from the main unit
 */
boolean RF_Listen()
{
  boolean endThis = false;
  radio.startListening(); // switch into radio listening function
  if (radio.available())
  {
    char text[32] = "";
    radio.read(&text, 32); // get value from NRF
    if (String(text) == "A")
    {
      endThis = HIGH;
    }
    else
    {
      passcodeReceived[receivePass_index];
      endThis = LOW;
      receivePass_index++;
    }
    radio.flush_rx(); // flush the radio buffer memory
  }
  return endThis;
}

// method for reset index and array function
void resetPass_arrayxindex()
{
  receivePass_index = 0;
  for (int x = 0; x < sizeof(passcodeReceived); x++)
  {
    passcodeReceived[x] = "";
  }
}