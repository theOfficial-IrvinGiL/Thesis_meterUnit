// method that deals with eeprom related process are written here:

/**
 * Note: in dealing with the data that will be broadcasted from the main unit
 * only the unique passcode of the users will be send and stored in eeprom to
 * keep simplicity and avoid complex bugs and code design.
 *
 * - EEPROM is programmed to have a <= 300 address for the systems utilization
 */

// method to read existing data from the internal eeprom memory
String readStringFromEEPROM(int address)
{
  int len = 4;
  char data[len + 1];
  for (int i = 0; i < len; i++)
  {
    data[i] = EEPROM.read(address + i);
  }
  data[len] = '\0';
  return String(data);
}

// method used to read stored codes in eeprom
// void fetchDataFromEEEPROM()
// {
//   String code = "";

//   if (EEPROM.read(0) <= 0)
//   {
//     // Serial.println("No memories on eeprom");
//     showMessage("No Data!");
//     display.clearDisplay();
//     display.display();
//   }
//   else
//   {
//     for (int x = 0; x < sizeof(registered_passcode); x++)
//     {
//       String loaded_data = readStringFromEEPROM(x * 4);

//       registered_passcode[x] = loaded_data;
//     }
//   }
// }

// method that returns the value from eeprom that is retrieved
// String readStringFromEEPROM(int addrOffset)
// {
//   int newStrLen = EEPROM.read(addrOffset);
//   char data[newStrLen + 1];
//   for (int i = 0; i < newStrLen; i++)
//   {
//     data[i] = EEPROM.read(addrOffset + 1 + i);
//   }
//   data[newStrLen] = '\0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
//   return String(data);
// }

void writeStringIntoEEPROM(int address, String str)
{
  byte len = str.length();
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(address + i, str[i]);
  }
}

/**
 * void function to write new passcode into the eeprom memory
 */
void writeData_toEEPROM()
{
  int target_eeprom = 0; // serve as index reference for target address on eeprom
  for (int x = 0; x < sizeof(registered_passcode); x++)
  {
    if (registered_passcode[x] == "")
    {
      break;
    }
    else
    {
      // String buffer_var = registered_passcode[x];
      // for (int y = 0; y < 4; y++)
      // {
      //   EEPROM.write(target_eeprom, buffer_var[y]);

      //   target_eeprom++;
      // }
      writeStringIntoEEPROM(target_eeprom, registered_passcode[x]);
      target_eeprom += 4;
    }
  }
}