// codes for processing and dealing with updating user data functionalities are written in here...

void processRFdata()
{
    unsigned int passcodeCount = 0;
    bool continueLoop = LOW;
    while (continueLoop == LOW)
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

                if (String(text) == "A")
                {
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
void resetPasscodeArray(){
    for(int x = 0; x < sizeof(passcodeReceived); x++){
        passcodeReceived[x] = "";
    }
}