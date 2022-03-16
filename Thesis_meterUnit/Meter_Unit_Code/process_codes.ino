//methods that are responsible for background process functions are written here:

// this method gets the user input and compares it to the passcode values retrieved from the eeprom
void checkInputAndDecide(){
  String compareThis, input;
  for(int x=0; x < ARRAY_SIZE(user_input); x++){
    input += user_input[x];
  }
  boolean matchTrigger = false;
  while(EEPROM.read(addressOnEEPROM)> 0){
    compareThis = readStringFromEEPROM(addressOnEEPROM);
    if(compareThis == input){
      matchTrigger = true;
      break;
    }
    addressOnEEPROM+=5;
  }

  //deciding action to determine the mode of the meter unit (Measure mode: ON / OFF)
  if(matchTrigger == false){
    showMessage("Error");
    addressOnEEPROM =1;
    measureMode = false;
    
  }else{
    digitalWrite(relayPin, HIGH);
    showMessage("Matched");
    addressOnEEPROM =1;
    measureMode =true;
    delay(1000);
    //get the accumulated measurement from the internal memory of the PZEM 004T
    timestamp_Energy = pzem.energy();
    //snapshot millis time for checking every minute
    
    
    
  }

}

//method code for PZEM 004T to display the energy computed from the library
void measureEnergy(float current, float voltage, float power, float energy){
  //for voltage display on serial
  if(voltage != NAN){
    Serial.print("Voltage: ");
    Serial.print(voltage,9);
    Serial.println("V");
  } else {
    Serial.println("Error reading voltage");
  }

  //for Power display on serial
  if(current != NAN){
    Serial.print("Power: "); 
    Serial.print(power,9); 
    Serial.println("W");
  } else {
    Serial.println("Error reading power");
  }

  //for current display on serial
  if(current != NAN){
    Serial.print("Current: ");
    Serial.print(current,9);
    Serial.println("A");
  } else {
    Serial.println("Error reading current");
  }

  //for current energy display on serial
  if(current != NAN){
    Serial.print("Current Energy consumption: ");
    Serial.print(energy,9); 
    Serial.println("kWh");
  } else {
    Serial.println("Error current reading energy");
  }
  //for accumulated energy display on serial
  if(current != NAN){
    Serial.print("Accumulated Energy consumption: ");
    Serial.print(energy + timestamp_Energy,9); 
    Serial.println("kWh");
  } else {
    Serial.println("Error current reading energy");
  }

  Serial.println();
  // delay(2000);

  //displaying the values on OLED
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(2,1);
  display.print("Energy: "+String(energy,7)+" kWh");

  display.setTextSize(1);
  display.setCursor(2,15);
  display.print("Voltage: "+String(voltage,5)+" V");

  display.setTextSize(1);
  display.setCursor(2,28);
  display.print("Power: "+String(power,5)+" W");

  display.setTextSize(1);
  display.setCursor(2,41);
  display.print("Current: "+String(current,5)+" A");

  display.display();

  //code to determine if the energy has changed for 1 minute
  if (millis() < relayCutoff_Interval){
      determine_minuteInterval = millis();
  }
  else if (millis() - determine_minuteInterval >= relayCutoff_Interval){
    determine_minuteInterval = millis();  
    //if the energy consumption remains unchanged for a minute, then the system trips the relay
    if(power < 1){
      //trip the relay off
      Serial.print("Cutoff time has passed!");
      digitalWrite(relayPin, LOW);
      measureMode = false;
      display.clearDisplay();
    }       
  }     
    
}
