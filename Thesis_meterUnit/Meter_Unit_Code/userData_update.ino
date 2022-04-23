// codes for processing and dealing with updating user data functionalities are written in here...

/**
 * method for processing the data from the radio buffer
 */
void processRFdata()
{
    unsigned int passcodeCount = 0;
    unsigned long now_millis = millis();
    while ((millis() - now_millis) < 300000)
    {
        if (radio.available())
        {
            char text[32] = "";
            radio.read(&text, sizeof(text));
            if (passcodeCount == 0)
            {
                passcodeReceived[passcodeCount] = String(text);
                passcodeCount++;
            }
            else
            {

                if (String(text) == "")
                {
                    break;
                    break;
                }
                else
                {
                    passcodeReceived[passcodeCount] = String(text);
                    passcodeCount++;
                }
            }
        }
    }
}

/**
 * method to reset the receive passcode array
 */
void resetPasscodeArray()
{
    for (int x = 0; x < sizeof(passcodeReceived); x++)
    {
        passcodeReceived[x] = "";
    }
}
