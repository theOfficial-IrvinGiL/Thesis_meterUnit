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

    // if(millis() - previousMillis >= relayCutoff_Interval){ //turn off measuring mode if the power is zero under 10 seconds
    //   if(power < 1){

    //     if(energy > 0){
    //       showMessage("Energy consumed:" + String(energy, 7));
    //     }
    //     RF_setupSend();
    //     showMessage("No Load connected...");
    //     showMessage("No Device connected...");
    //     String currentEnergy = String(energy, 7);
    //     concatDateTime(currentEnergy);
    //     sendTo_main(RF_message);

    //     digitalWrite(relayPin, LOW);
    //     showMessage(RF_message);
    //     measureMode = LOW;
    //     RF_message = "";
    //   }

    // }

    // code for dealing with millis rollover that may cause possible bugs
    if (millis() < thisInterval)
    {
      previousMillis = 0;
    }
    else if (millis() - previousMillis >= 10000)
    {
      RF_setupSend();
      // turn the relays off after 10 seconds
      showMessage("No Load connected...");
      showMessage("No Device connected...");
      String currentEnergy = String(energy, 7);
      concatDateTime(currentEnergy);
      sendTo_main(RF_message);

      digitalWrite(relayPin, LOW);
      showMessage(RF_message);
      measureMode = LOW;
      RF_message = "";
    }
    // else if (millis() - previousMillis >= thisInterval){
    else
    {

      measureEnergy(current, voltage, power, energy);
    }
  }
  else
  {
    showInputPasscode();
    if (keyValue)
    {
      switch (keyValue)
      {
      case 'B':

        //          radio.stopListening();

        display.clearDisplay();
        setCursor_column = 0;
        keyValue = 0x00;
        memset(user_input, 0, sizeof(user_input));
        fixedNumberOfInputs = 0;

        break;

      case 'A':

        //          radio.stopListening();

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
        RF_setupListen();
        processRFdata();


        resetPasscodeArray(); //reset the passcode array
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
