// codes for processing and dealing with updating user data functionalities are written in here...

/**
 * method for processing the data from the radio buffer
 */

/**
 * method to reset the receive passcode array
 */
// void resetPasscodeArray()
// {
//     for (int x = 0; x < sizeof(passcodeReceived); x++)
//     {
//         passcodeReceived[x] = "";
//     }
// }
void receiveProcess()
{
    radio.startListening();
    showMessage("System updating!");
    // loop until the update trig is returned true
    int receiveIndex = 0;
    

    while (true)
    {
        if (radio.available())
        {
            char text[32] = "";
            radio.read(&text, sizeof(text));
            if (String(text) == "A")
            {
                break;
            }
            // else if (String(text) == "")
            // {
            //     continue;
            // }

            writeStringIntoEEPROM((receiveIndex * 4), String(text));
            receiveIndex++;
        }
    }
    radio.flush_rx();
    
    // write code for writting received values into the eeprom
    showMessage("Done updating!");
    resetFunc();
    display.clearDisplay();
    display.display();
}
