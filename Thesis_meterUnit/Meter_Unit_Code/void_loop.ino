void loop() {

  clearScreen:
  // put your main code here, to run repeatedly:
  char keyValue = customKeypad.getKey();

  //setting mode for the system to execute
  if (measureMode == true ){ 
    float current = pzem.current();
    float voltage = pzem.voltage();
    float power = pzem.power();

    float energy = (pzem.energy() - timestamp_Energy);
   
   //code for dealing with millis rollover that may cause possible bugs
    if (millis() < thisInterval){
      previousMillis = 0;
    }
    else if (millis() - previousMillis >= thisInterval){
      previousMillis = millis();
      measureEnergy(current, voltage, power, energy);

    }
    
  }
  else{
    showInputPasscode();
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
        
        //pressing *D means to end the measuring process of the unit
        case 'D':
            // //Reset all values
            // display.clearDisplay();
            // setCursor_column = 0;
            // keyValue = 0x00;
            // memset(user_input, 0, sizeof(user_input));
            // fixedNumberOfInputs =0;
            // measureMode = false;
            // previousMillis = 0;


            // //Enter code for sending final data here
            // goto clearScreen;

        break;

        default:
          // will not accept user input from user once the fixedNumberOfInputs reaches 4
          if(fixedNumberOfInputs < 4){
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