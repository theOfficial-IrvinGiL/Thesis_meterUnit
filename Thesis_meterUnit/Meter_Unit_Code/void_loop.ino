void loop()
{

  // put your main code here, to run repeatedly:
  char keyValue = customKeypad.getKey();

  // setting mode for the system to execute
  if (measureMode == true)
  {
    float current = pzem.current();
    float voltage = pzem.voltage();
    float power = pzem.power();

    float energy = (pzem.energy() - timestamp_Energy);

    // code for dealing with millis rollover that may cause possible bugs
    if (millis() < thisInterval)
    {
      previousMillis = 0;
    }
    // checks the AC circuit every 1 minute to monitor if there is load connected
    else if (millis() - previousMillis >= 60000)
    {
      if (power <= 0 && energy > 0)
      {
        RF_setupSend(); // setup the RF for the sending functionality
        // turn the relays off after 10 seconds
        showMessage("No Load connected..."); // show text message on OLED
        showMessage("No Device connected...");
        String currentEnergy = String(energy, 10); // process to concatinate the energy consumption data into the RF message
        concatDateTime(currentEnergy);
        sendTo_main(RF_message); // call function to send the produced message to the main unit

        digitalWrite(relayPin, LOW); // turn off the Relay (cut the connection on the AC circuit)
        showMessage(RF_message);     // show the constructed message to the OLED DISPLAY
        // reset the modes and variables that were involved in the measuring mode of the unit
        measureMode = LOW;
        RF_message = "";
      }
    }
    // else if (millis() - previousMillis >= thisInterval){
    else
    {
      // call measuring method and show the values into the OLED display
      measureEnergy(current, voltage, power, energy);
    }
  }

  // accept passcode mode
  else
  {

    showInputPasscode();
    if (keyValue)
    {
      switch (keyValue)
      {
      case 'B':
        /**clear the oled display and reset all the variables
         that were used on the inputting passcode event */
        display.clearDisplay();
        setCursor_column = 0;
        keyValue = 0x00;
        memset(user_input, 0, sizeof(user_input));
        fixedNumberOfInputs = 0;
        display.clearDisplay();

        break;

      case 'A':
        /**
         * pressing #A key will trigger the system to check the user
         * input
         */
        checkInputAndDecide();
        setCursor_column = 0;
        keyValue = 0x00;
        memset(user_input, 0, sizeof(user_input));
        fixedNumberOfInputs = 0;

        break;

      // pressing *D means to end the measuring process of the unit
      case 'D':
        // when pressed, interupts the unit into update mode
        // write code for updating eeprom memory here
        showMessage("System updating!");
        RF_setupListen();
        processRFdata();
        writeData_toEEPROM(); // write data into the eeprom
        resetPasscodeArray(); // reset the passcode array
        break;

      default:
        // will not accept user input from user once the fixedNumberOfInputs reaches 4
        if (fixedNumberOfInputs < 4)
        {
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
