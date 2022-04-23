//methods that has functions as oled message displays are written here:

void showMeterUnit() {
  //clear the screen first
  display.clearDisplay();
  //set text color
  display.setTextColor(WHITE);
  //set text size
  display.setTextSize(2);
  //set the cursor coordinates
  display.setCursor(4, 10);
  display.print("Meter Unit");
  //display the "meter unit" text
  display.display();
  delay(4000);
  //clear the oled display
  display.clearDisplay();
}

void showInputPasscode() {
  // display.clearDisplay();
  //set Text size
  display.setTextSize(1);
  //set the cursor coordinates
  display.setCursor(0, 0);
  display.println("Input Passcode:");
  display.display();
}

//method used to display text on the oled display to show the inputs from the user
void showsUser_input(char key) {
  // //set text color
  display.setTextColor(WHITE);
  //set text size
  display.setTextSize(2);
  //set the cursor coordinates
  display.setCursor(setCursor_column, 15);
  display.print(key);
  setCursor_column += 12;
  // user_Input += key;
  display.display();
}

//This method is called when displaying messages: (Access granted and Access Denied)
void showMessage(String message) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(4, 4);
  display.print(message);
  display.display();
  delay(2000);
  
}

//the function of this method is to display the readed values of energy into the oled
void showEnergy_onOled(String this_energy, String this_voltage, String this_power, String this_current) {
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(2, 1);
  display.print("Energy: " + this_energy + " kWh");

  display.setTextSize(1);
  display.setCursor(2, 15);
  display.print("Voltage: " + this_voltage + " V");

  display.setTextSize(1);
  display.setCursor(2, 28);
  display.print("Power: " + this_power + " W");

  display.setTextSize(1);
  display.setCursor(2, 41);
  display.print("Current: " + this_current + " A");

  display.display();
}
