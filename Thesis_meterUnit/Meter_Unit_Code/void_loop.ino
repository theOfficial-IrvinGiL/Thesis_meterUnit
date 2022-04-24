void loop()
{

  // // put your main code here, to run repeatedly:
  char keyValue = customKeypad.getKey();

  // setting mode for the system to execute
  if (measureMode == true)
  {
    float current = pzem.current();
    float voltage = pzem.voltage();
    float power = pzem.power();

    // float energy = pzem.energy();
    float energy = (pzem.energy() - timestamp_Energy);

    // code for dealing with millis rollover that may cause possible bugs
    if (millis() < thisInterval)
    {
      previousMillis = 0;
    }

    measureEnergy(current, voltage, power, energy);
  }

  // accept passcode mode
  else
  {
    radio.stopListening();
    if (keyValue)
    {

      if (oled_active == HIGH)
      {
        oled_timestamp = millis(); // reset timestamp for oled every keypress
        switch (keyValue)
        {
        case 'B':
          /**clear the oled display and reset all the variables
           that were used on the inputting passcode event */
          display.clearDisplay();
          // resetForReMeasure();
          setCursor_column = 0;
          keyValue = 0x00;
          // memset(user_input, 0, sizeof(user_input));
          userInput_memset(); // reset the user_input char array
          fixedNumberOfInputs = 0;
          display.clearDisplay();
          oled_timestamp = millis();

          break;

        case 'A':
          /**
           * pressing #A key will trigger the system to check the user
           * input
           */
          checkInputAndDecide();
          setCursor_column = 0;
          keyValue = 0x00;
          // memset(user_input, 0, sizeof(user_input));
          userInput_memset(); // reset the user_input char array
          fixedNumberOfInputs = 0;
          oled_timestamp = millis();

          break;

        // pressing *D means to end the measuring process of the unit
        case 'D':

          boolean update_trig = false;
          // when pressed, interupts the unit into update mode
          // write code for updating eeprom memory here
          showMessage("System updating!");
          // loop until the update trig is returned true
          while (update_trig == false)
          {
            if (update_trig == HIGH)
            {
              break;
            }
            else
            {
              update_trig = RF_Listen();
            }
          }

          // write code for writting received values into the eeprom
          for (int x = 0; x < sizeof(passcodeReceived); x++)
          {
            writeStringIntoEEPROM((x * 4), passcodeReceived[x]);
          }

          resetPass_arrayxindex();
          showMessage("Done updating!");
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
      else
      {
        // turn oled on here >>>>
        oled_active = HIGH;
      }
    }

    else if (oled_active == HIGH) // show text if oled trigger is high
    {
      showInputPasscode();
      //  turn off oled after 2 minutes since last activity
      if (millis() - oled_timestamp > 120000)
      {
        showMessage("Oled timeout");
        display.clearDisplay();
        display.display();
        oled_timestamp = millis();
        oled_active = LOW;

        setCursor_column = 0;
        keyValue = 0x00;
        userInput_memset(); // reset the user_input char array
        fixedNumberOfInputs = 0;
      }
    }
    //
    // else if ((millis() - oled_timestamp) > 2000)
    // {
    //   showMessage("Oled timeout");
    //   display.clearDisplay();
    //   display.display();
    //   oled_timestamp = millis();
    //   oled_active = LOW;

    //   setCursor_column = 0;
    //   keyValue = 0x00;
    //   userInput_memset(); // reset the user_input char array
    //   fixedNumberOfInputs = 0;
    // }
  }
}
