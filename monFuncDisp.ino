/* ===================================================================== *
 *                                                                       *
 * DISPLAY SYSTEM                                                        *
 *                                                                       *
 * ===================================================================== *
 * every "disp menu function" needs three functions 
 * - void LCDML_DISP_setup(func_name)    
 * - void LCDML_DISP_loop(func_name)     
 * - void LCDML_DISP_loop_end(func_name)
 *
 * EXAMPLE CODE:
    void LCDML_DISP_setup(..menu_func_name..) 
    {
      // setup
      // is called only if it is started

      // starts a trigger event for the loop function every 100 millisecounds
      LCDML_DISP_triggerMenu(100);  
    }
    
    void LCDML_DISP_loop(..menu_func_name..)
    { 
      // loop
      // is called when it is triggert
      // - with LCDML_DISP_triggerMenu( millisecounds ) 
      // - with every button status change

      // check if any button is pressed (enter, up, down, left, right)
      if(LCDML_BUTTON_checkAny()) {         
        LCDML_DISP_funcend();
      } 
    }
    
    void LCDML_DISP_loop_end(..menu_func_name..)
    {
      // loop end
      // this functions is ever called when a DISP function is quit
      // you can here reset some global vars or do nothing  
    } 
 * ===================================================================== *
 */

// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_information)
// *********************************************************************
{
  // setup function 
    lcd.setCursor(0,0);
    lcd.print(F(PROG_NAME));  // print first line to lcd display
    lcd.setCursor(0,1);
    lcd.print(F(PROG_VER));  // print second line to lcd display
}

void LCDML_DISP_loop(LCDML_FUNC_information) 
{
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function

  if(LCDML_BUTTON_checkAny()) { // check if any button is presed (enter, up, down, left, right)
	  LCDML_DISP_funcend();   // LCDML_DISP_funcend calls the loop_end function
  } 
}

void LCDML_DISP_loop_end(LCDML_FUNC_information)
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing  
}  


// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_contrast)
// *********************************************************************
{ 
  int i;
  // setup function

  currentContrast = oldContrast;

  // print lcd content
  lcd.setCursor(0, 0);
  lcd.print(F("Contrast setting"));
  lcd.setCursor(0, 1);
  
  for ( i=0; i < currentContrast/8; i++ ){
    lcd.print((char)0x05);  
  }
}

void LCDML_DISP_loop(LCDML_FUNC_contrast)
{
  int i;
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function 
  
  if (LCDML_BUTTON_checkAny()) // check if any button is pressed (enter, up, down, left, right)
  {
    if (LCDML_BUTTON_checkUp()) // check if button up is pressed
    {
      LCDML_BUTTON_resetUp();   // reset the up button
      
      currentContrast = currentContrast + 8;
      if ( currentContrast > 128 ) {
        currentContrast = 128;
      }
      analogWrite(lcdContrastPin, currentContrast);   
      
      // update lcd content
      lcd.setCursor(0, 1);
      for ( i=0; i < currentContrast/8; i++ ){
        lcd.print((char)0x05);  
      }
    }    
  
    if (LCDML_BUTTON_checkDown()) // check if button down is pressed
    {
      LCDML_BUTTON_resetDown();   // reset the down button
      
      currentContrast = currentContrast - 8;
      if ( currentContrast < 0 ) {
        currentContrast = 0;
      }
      analogWrite(lcdContrastPin, currentContrast);   
      
      // update lcd content
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      for ( i=0; i < currentContrast/8; i++ ){
        lcd.print((char)0x05);  
      }
    }    
  
    if (LCDML_BUTTON_checkRight() || LCDML_BUTTON_checkEnter()) // check if button right is pressed
    {
      // end function for callback
      oldContrast = currentContrast;
      EEPROM.write(1, oldContrast);   // write contrast value into EEPROM
      LCDML_DISP_funcend();   
    } 
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_contrast) 
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing

  analogWrite(lcdContrastPin, oldContrast);   
}

// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_backlit_value)
// *********************************************************************
{ 
  int i;
  // setup function

  currentBacklit = oldBacklit;

  // print lcd content
  lcd.setCursor(0, 0);
  lcd.print(F("Backlit setting"));
  lcd.setCursor(0, 1);
  
  for ( i=0; i < currentBacklit/8; i++ ){
    lcd.print((char)0x05);  
  }
}

void LCDML_DISP_loop(LCDML_FUNC_backlit_value)
{
  int i;
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function 
  
  if (LCDML_BUTTON_checkAny()) // check if any button is pressed (enter, up, down, left, right)
  {
    if (LCDML_BUTTON_checkUp()) // check if button up is pressed
    {
      LCDML_BUTTON_resetUp();   // reset the up button
      
      currentBacklit = currentBacklit + 8;
      if ( currentBacklit > 128 ) {
        currentBacklit = 128;
      }
      analogWrite(lcdBacklitPin, currentBacklit);   
      
      // update lcd content
      lcd.setCursor(0, 1);
      for ( i=0; i < currentBacklit/8; i++ ){
        lcd.print((char)0x05);  
      }
    }    
  
    if (LCDML_BUTTON_checkDown()) // check if button down is pressed
    {
      LCDML_BUTTON_resetDown();   // reset the down button
      
      currentBacklit = currentBacklit - 8;
      if ( currentBacklit < 0 ) {
        currentBacklit = 0;
      }
      analogWrite(lcdBacklitPin, currentBacklit);   
      
      // update lcd content
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
      lcd.setCursor(0, 1);
      for ( i=0; i < currentBacklit/8; i++ ){
        lcd.print((char)0x05);  
      }
    }    
  
    if (LCDML_BUTTON_checkRight() || LCDML_BUTTON_checkEnter()) // check if button right is pressed
    {
      // end function for callback
      oldBacklit = currentBacklit;
      EEPROM.write(2, oldBacklit);   // write backlit value into EEPROM
      LCDML_DISP_funcend();   
    } 
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_backlit_value) 
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing

  analogWrite(lcdBacklitPin, oldBacklit);   
}


// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_backlit_time)
// *********************************************************************
{ 
  // setup function

  currentBacklitTimeout = oldBacklitTimeout;

  // print lcd content
  lcd.setCursor(0, 0);
  lcd.print(F("Backlit timeout"));
  lcd.setCursor(0, 1);
  sprintf(line, "%3d sec", currentBacklitTimeout);
  lcd.print(line);
}

void LCDML_DISP_loop(LCDML_FUNC_backlit_time)
{
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function 
  
  if (LCDML_BUTTON_checkAny()) // check if any button is pressed (enter, up, down, left, right)
  {
    if (LCDML_BUTTON_checkUp()) // check if button up is pressed
    {
      LCDML_BUTTON_resetUp();   // reset the up button
      
	  currentBacklitTimeout += 1;
      if (currentBacklitTimeout > 99 ) {
		  currentBacklitTimeout = 99;
      }
	  lcd.setCursor(0, 1);
	  sprintf(line, "%3d sec", currentBacklitTimeout);
	  lcd.print(line);
    }    

    if (LCDML_BUTTON_checkDown()) // check if button down is pressed
    {
      LCDML_BUTTON_resetDown();   // reset the down button
      
	  currentBacklitTimeout -= 1;
	  if (currentBacklitTimeout < 0) {
		  currentBacklitTimeout = 0;
	  }
	  lcd.setCursor(0, 1);
	  sprintf(line, "%3d sec", currentBacklitTimeout);
	  lcd.print(line);
    }    
  
    if (LCDML_BUTTON_checkRight() || LCDML_BUTTON_checkEnter()) // check if button right is pressed
    {
      // end function for callback
      oldBacklitTimeout = currentBacklitTimeout;
      EEPROM.write(3, oldBacklitTimeout);   // write backlit timeout into EEPROM

	  if (oldBacklitTimeout == 0) {
		  analogWrite(lcdBacklitPin, 0);  // switch backlit off
	  }
	  else {
		  analogWrite(lcdBacklitPin, currentBacklit); // switch backlit on
	  }
	  lcd.setCursor(0, 1);
	  sprintf(line, "set to %3d sec", currentBacklitTimeout);
	  lcd.print(line);
	  delay(500);

	  LCDML_DISP_funcend();
    } 
  }
}

void LCDML_DISP_loop_end(LCDML_FUNC_backlit_time) 
{
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
}


// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_back)
// *********************************************************************
{ 
  // setup function
}

void LCDML_DISP_loop(LCDML_FUNC_back)
{ 
  // loop
  // is called when it is triggert
  LCDML_DISP_resetIsTimer(); // reset the initscreen timer
  LCDML.goBack();            // go back
  LCDML_DISP_funcend();      // LCDML_DISP_funcend calls the loop_end function
}
    
void LCDML_DISP_loop_end(LCDML_FUNC_back)
{
  // loop end
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing  
}


// *********************************************************************
//
// standbyscreen/initscreen is shown when no menu actions are executed
//
// *********************************************************************

void LCDML_DISP_setup(LCDML_FUNC_initscreen)
{
  // setup function
  LCDML_DISP_triggerMenu(1000); // set trigger for this function to 1000 millisecounds
}

void ledBuiltinToggle() {
static unsigned long ledBuiltinOnOff=0;

  if ( ledBuiltinOnOff == 0 ) {
    ledBuiltinOnOff = 1;
    digitalWrite(ledBuiltin, HIGH);  //LED=on
  }
  else
  {
    ledBuiltinOnOff = 0;
    digitalWrite(ledBuiltin, LOW);  //LED=off
  }
}

void DisplayPosition(char* lat, char* lon, int row);
void DisplayTimeDate(char* time, char* date, int row);
void DisplaySogCog(char* SOG, char* COG, int row);
void DisplayStwHeading(char* stw, char* heading, int row);
void DisplayBftGwd(char* bft, char* gwd, int row);
void DisplayTwsTwa(char* tws, char* twa, int row);
void DisplayAwsAwa(char* aws, char* awa, int row);
void DisplayWaterDepth(char* waterTemp, char* depth, int row);
void DisplayAirPress(char* airTemp, char* press, int row);
void DisplayLogTrip(char* log, char* trip, int row);
void DisplayBatt(char* batt1, char* batt2, int row);
void DisplayEngine(char* rpmPort, char* rpmStbd, int row);
void DisplayPressHum(char* pressure, char* humidity, int row);
     
static void (*p[PAGECOUNT]) (char*, char*, int) {DisplayPressHum, DisplayPosition, DisplayTimeDate, DisplaySogCog, DisplayStwHeading,
                               DisplayBftGwd, DisplayTwsTwa, DisplayAwsAwa, DisplayWaterDepth, DisplayAirPress, DisplayLogTrip,
                               DisplayBatt, DisplayEngine};

void LCDML_DISP_loop(LCDML_FUNC_initscreen)
{
  // loop function, can be run in a loop when LCDML_DISP_triggerMenu(xx) is set
  // the quit button works in every DISP function without any checks; it starts the loop_end function 
static int pagedisplayed=0;
static int displaysec=PAGE_DISPLAY;
static int suppressCount;

  ledBuiltinToggle();

  g_lcdml_initscreen = millis();      // reset initscreen timer

  if (oldBacklitTimeout == 0) {
	  analogWrite(lcdBacklitPin, currentBacklit); // switch backlit on
//	  lcd.setCursor(0, 1);         // set the cursor to column 0, line 1  (line 1 is the second line)
//	  sprintf(line, "Sw BackLit on perm");
//	  lcd.print(line);
//	  delay(200);
  }
  else {
	  BackLitTimeout += 1;
	  if (BackLitTimeout > oldBacklitTimeout) {
		  BackLitTimeout = oldBacklitTimeout;
		  analogWrite(lcdBacklitPin, 0);  // switch backlit off
//		  lcd.setCursor(0, 1);         // set the cursor to column 0, line 1  (line 1 is the second line)
//		  sprintf(line, "Sw BackLit off  ");
//		  lcd.print(line);
//		  delay(200);
	  }
	  else {
		  analogWrite(lcdBacklitPin, currentBacklit); // switch backlit on
//		  lcd.setCursor(0, 1);         // set the cursor to column 0, line 1  (line 1 is the second line)
//		  sprintf(line, "Sw BackLit on   ");
//		  lcd.print(line);
//		  delay(200);
	  }
  }

  displaysec--;
  if (displaysec == 0) {
    pagedisplayed++;
    if (pagedisplayed >= PAGECOUNT) {
      pagedisplayed = 0;
    }
    displaysec = PAGE_DISPLAY;
  }

  if (flag[pagedisplayed] == 1){
    (*p[pagedisplayed]) ((char *)"",(char *)"", 0);      // call the page display function
  }
  else {
    suppressCount=PAGECOUNT;
    while (flag[pagedisplayed] == 0 && suppressCount > 0) {
      suppressCount--;
      pagedisplayed++;
      if ( pagedisplayed >= PAGECOUNT ) {
        pagedisplayed = 0;
      }
    }
    if (suppressCount == 0) {
      DisplaySuppressed();
    }
    else {
      (*p[pagedisplayed]) ((char *)"", (char *)"", 0);      // call the page display function
    }
  }

  if (LCDML_BUTTON_checkAny()) { // check if any button is pressed
	BackLitTimeout = 0;
	analogWrite(lcdBacklitPin, currentBacklit); // switch backlit on
//	lcd.setCursor(0, 1);         // set the cursor to column 0, line 1  (line 1 is the second line)
//	sprintf(line, "Sw BackLit on, again");
//	lcd.print(line);
//	delay(500);

	if (LCDML_BUTTON_checkRight() || LCDML_BUTTON_checkEnter()) // check if button right is pressed
    {
      LCDML_BUTTON_resetRight();   // reset the right button
      LCDML_BUTTON_resetEnter();   // reset the enter button

      flag[pagedisplayed] = 0;
      EEPROM.write(pagedisplayed+4, 0);   // write flag into EEPROM
      
      lcd.setCursor(0, 1);         // set the cursor to column 0, line 1  (line 1 is the second line)
      sprintf(line, "Page suppressed ");
      lcd.print(line);
    } 

    if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) // check if button up or down is pressed
    {
      // end function for callback
      LCDML_DISP_funcend();   
    } 
  }   
}

void LCDML_DISP_loop_end(LCDML_FUNC_initscreen) 
{  
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing
  LCDML.goRoot(); // go to root element (first element of this menu with id=0)

  digitalWrite(ledBuiltin, LOW);  //LED=off
}

// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_reset)
// *********************************************************************
{ 
  // setup function
  // print lcd content
  lcd.setCursor(0, 0);
  lcd.print(F("reset all pages?"));  
  lcd.setCursor(0, 1);
  lcd.print(F("enter for yes   "));  
}

void LCDML_DISP_loop(LCDML_FUNC_reset)
{ 
  int i;
  // loop
  // is called when it is triggert
  if (LCDML_BUTTON_checkAny()) // check if any button is pressed (enter, up, down, left, right)
  {
    if (LCDML_BUTTON_checkRight() || LCDML_BUTTON_checkEnter()) // check if button right is pressed
    {
      for ( i=0; i < PAGECOUNT; i++ ){
        flag[i] = 1;
        EEPROM.write(i+4, 1);   // write flags into EEPROM
      }
    }
    LCDML_DISP_funcend();      // end function for callback
  }
}
    
void LCDML_DISP_loop_end(LCDML_FUNC_reset)
{
  // loop end
  // this functions is ever called when a DISP function is quit
  // you can here reset some global vars or do nothing 
}


