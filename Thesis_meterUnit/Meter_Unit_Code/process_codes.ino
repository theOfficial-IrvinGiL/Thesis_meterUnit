// methods that are responsible for background process functions are written here:

// declaration for nrf module.(run once)
void nrfSetup()
{
  radio.begin(); // Starting the radio communication
  Serial.begin(9600);
  radio.openWritingPipe(addresses[1]);    // Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[0]); // Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MAX);          // You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
}

// this method gets the user input and compares it to the passcode values retrieved from the eeprom
void checkInputAndDecide()
{
  String compareThis, input;
  for (int x = 0; x < ARRAY_SIZE(user_input); x++)
  {
    input += user_input[x];
  }
  boolean matchTrigger = false;
  while (EEPROM.read(addressOnEEPROM) > 0)
  {
    compareThis = readStringFromEEPROM(addressOnEEPROM);
    // if (compareThis == input)
    if (input == "1157")
    {
      matchTrigger = true;
      break;
    }
    addressOnEEPROM += 5;
  }
  // deciding action to determine the mode of the meter unit (Measure mode: ON / OFF)
  if (matchTrigger == false)
  {
    showMessage("Error");
    addressOnEEPROM = 1;
    measureMode = false;
  }
  else
  {
    digitalWrite(relayPin, HIGH);
    showMessage("Matched");
    addressOnEEPROM = 1;
    measureMode = true;
    delay(1000);
    // get the accumulated measurement from the internal memory of the PZEM 004T
    timestamp_Energy = pzem.energy();
    // snapshot millis time for checking every minute
  }
}

// method code for PZEM 004T to display the energy computed from the library
void measureEnergy(float current, float voltage, float power, float energy)
{
  // for voltage display on serial
  if (voltage != NAN)
  {
    Serial.print("Voltage: ");
    Serial.print(voltage, 9);
    Serial.println("V");
  }
  else
  {
    Serial.println("Error reading voltage");
  }

  // for Power display on serial
  if (current != NAN)
  {
    Serial.print("Power: ");
    Serial.print(power, 9);
    Serial.println("W");
  }
  else
  {
    Serial.println("Error reading power");
  }

  // for current display on serial
  if (current != NAN)
  {
    Serial.print("Current: ");
    Serial.print(current, 9);
    Serial.println("A");
  }
  else
  {
    Serial.println("Error reading current");
  }

  // for current energy display on serial
  if (current != NAN)
  {
    Serial.print("Current Energy consumption: ");
    Serial.print(energy, 9);
    Serial.println("kWh");
  }
  else
  {
    Serial.println("Error current reading energy");
  }
  // for accumulated energy display on serial
  if (current != NAN)
  {
    Serial.print("Accumulated Energy consumption: ");
    Serial.print(energy + timestamp_Energy, 9);
    Serial.println("kWh");
  }
  else
  {
    Serial.println("Error current reading energy");
  }

  Serial.println();
  // delay(2000);

  // display the readed values on oled
  showEnergy_onOled(String(energy, 7), String(voltage, 5), String(power, 5), String(current, 5));

  // code to determine if the energy has changed for 1 minute
  if (millis() < relayCutoff_Interval)
  {
    determine_minuteInterval = millis();
  }
  else if (millis() - determine_minuteInterval >= relayCutoff_Interval)
  {
    determine_minuteInterval = millis();
    // if the energy consumption remains unchanged for a minute, then the system trips the relay
    if (power <= 0)
    {

      // write code for determining if there is energy consumed
      if (energy != 0)
      {
        showMessage("Energy consumed:"+ String(energy, 7));
        showMessage("No Load connected...");
        showMessage("No Device connected...");

        // write code for storing the data values into a string variable ready for sending
        String currentEnergy = String(energy, 7);
        concatDateTime(currentEnergy);
        sendTo_main(RF_message);
        

        showMessage(RF_message);
      }
      else
      {
        // show oled message that no load/energy has been measured
        showMessage("No energy consumed...");
        showMessage("No Load connected...");
        showMessage("No Device connected...");
      }
      // trip the relay off
      Serial.print("Cutoff time has passed!");
      digitalWrite(relayPin, LOW);
      measureMode = false;
      sendRF_mode = true;
      display.clearDisplay();
      RF_message = ""; // clear the global message variable
    }
  }
}

// code for concatinating the time and date into the RFmessage variable
void concatDateTime(String energy_consumed)
{
  DateTime Thisnow = rtc.now();

  RF_message += String(Thisnow.month(), DEC); // adding the date
  RF_message += "/";
  RF_message += String(Thisnow.day(), DEC);
  RF_message += "/";
  RF_message += String(Thisnow.year(), DEC);

  RF_message += "_"; // buffer character

  RF_message += String(Thisnow.hour(), DEC); // adding the time
  RF_message += ":";
  RF_message += String(Thisnow.minute(), DEC);

  RF_message += "_";             // buffer character
  RF_message += energy_consumed; // concat the current energy consumed data

  RF_message += "_"; // buffer character
  RF_message += String(user_input);
}

// method for sending data over RF to main unit
void sendTo_main(String message)
{
  // radio.stopListening(); // This sets the module as transmitter
  int messageLength = message.length();
  char data[messageLength];

  /**
   * gets data from global message variable and copies it to data
   * array for sending through RF
   */
  for (int x = 0; x < messageLength; x++)
  {
    data[x] = message[x];
  }

  // const char data[] = "First Data";
  radio.write(&data, sizeof(data)); // Sending the data
  delay(10);
}
