//method that deals with eeprom related process are written here:

//method used to read stored codes in eeprom
void readCodesFromEEEPROM(){
  String code = "";
  int indexCounter = 0;
  addressOnEEPROM +=1;
  if(EEPROM.read(0) <= 0){
    Serial.println("No memories on eeprom");
  }else{
    while(EEPROM.read(addressOnEEPROM) > 0){
      char codesArray[EEPROM.read(addressOnEEPROM)];
      for(int x = 0; x < EEPROM.read(addressOnEEPROM); x++){
        codesArray[x] = EEPROM.read(addressOnEEPROM+(x+1));
      }
      Serial.println(String(codesArray));

      addressOnEEPROM+=5;
      indexCounter++;
    }
  }

  addressOnEEPROM = 1;
}

//method that returns the value from eeprom that is retrieved
String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++){
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
  return String(data);
}
