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

    // showInputPasscode();
    //  turn off oled after 2 minutes since last activity
    if (millis() - oled_timestamp > 60000)
    {
      // showMessage("Oled timeout");
      display.clearDisplay();
      display.display();
      oled_timestamp = millis();
      oled_active = LOW;

      setCursor_column = 0;
      keyValue = 0x00;
      // userInput_memset(); // reset the user_input char array
      fixedNumberOfInputs = 0;
    }

    if (keyValue)
    {
      showInputPasscode();
      oled_timestamp = millis(); // reset timestamp for oled every keypress
      if (oled_active == HIGH)
      {

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
          display.display();
          oled_timestamp = millis();

          showInputPasscode();
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
        // case 'D':

        //   receiveProcess();
        //   break;

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
          // }
        }
      }

      else
      {
        // turn oled on here >>>>
        oled_active = HIGH;
      }
    }
  }
}
