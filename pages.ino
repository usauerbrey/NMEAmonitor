// page 1
//  54°22.300'N
// 018°56.467'E
// Position Vessel’s position.
void DisplayPosition(char* lat, char* lon, int row) {
  lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "lat  %2.0f%c%6.3f'N", LAT_DEG_disp, (char)0xDF, LAT_MIN_disp);
  lcd.print(line);
  lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "lon %03.0f%c%6.3f'E", LONG_DEG_disp, (char)0xDF, LONG_MIN_disp);
  lcd.print(line);
  if ((row == 0) && (_LCDML_DISP_rows == 4)) {
	  DisplayTimeDate((char *)"", (char *)"", 2);
  }
}

// page 2
// Date 10.10.2015
// (UTC)  09:45:56
// Date & Time System date and time, format may be changed in the settings.
void DisplayTimeDate(char* time, char* date, int row) {
  lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "Date  %02d.%02d.%04d", DatumZeit_disp.day, DatumZeit_disp.month, DatumZeit_disp.year);
  lcd.print(line);
  lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "(UTC)   %02d:%02d:%02d", DatumZeit_disp.hour, DatumZeit_disp.minute, DatumZeit_disp.second);
  lcd.print(line);
  if ((row == 0) && (_LCDML_DISP_rows == 4)) {
	  DisplaySogCog((char *)"", (char *)"", 2);
  }
}

// page 3
// SOG        5.6kn
// COG      112.3°T
// COG and SOG Course Over Ground and Speed Over Ground.
void DisplaySogCog(char* SOG, char* COG, int row) {
  lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "SOG       %4.1fkn", SOG_disp);
  lcd.print(line);
  lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "COG      %5.1f%cT", COG_disp, (char)0xDF);
  lcd.print(line);
  if ((row == 0) && (_LCDML_DISP_rows == 4)) {
	  DisplayStwHeading((char *)"", (char *)"", 2);
  }
}

// page 4
// STW        4.8kn
// HDG      102.5°T
// STW/Heading Speed Through Water and Heading.
void DisplayStwHeading(char* stw, char* heading, int row) {
  lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "STW       %4.1fkn", STW_disp);
//sprintf(line, "                ");
  lcd.print(line);
  lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "HDG      %5.1f%cT", HDG_disp, (char)0xDF );
  lcd.print(line);
  if ((row == 0) && (_LCDML_DISP_rows == 4)) {
	  DisplayBftGwd((char *)"", (char *)"", 2);
  }
}

// page 5
// Bft         3 NW
// GWD       204.3°
// TWS and TWA True Wind Speed and True Wind Angle.
void DisplayBftGwd(char* bft, char* gwd, int row) {
  lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "Bft        %2.0f %s", Bft_disp, GwdRose_disp);
//sprintf(line, "                ");
  lcd.print(line);
  lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "GWD      %5.1f%cT", Gwd_disp, (char)0xDF);
  lcd.print(line);
  if ((row == 0) && (_LCDML_DISP_rows == 4)) {
	  DisplayTwsTwa((char *)"", (char *)"", 2);
  }
}

// page 6
// TWS        6.8kn
// TWA      204.3°T
// TWS and TWA True Wind Speed and True Wind Angle.
void DisplayTwsTwa(char* tws, char* twa, int row) {
	lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
	sprintf(line, "TWS       %4.1fkn", TWS_disp);
	//sprintf(line, "                ");
	lcd.print(line);
	lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
	sprintf(line, "TWD      %5.1f%cT", TWD_disp, (char)0xDF);
	lcd.print(line);
	if ((row == 0) && (_LCDML_DISP_rows == 4)) {
		DisplayAwsAwa((char *)"", (char *)"", 2);
	}
}

// page 7
// AWS        7.1kn
// AWA       61.0°T
// AWS and AWA Apparent Wind Speed and Apparent Wind Angle.
void DisplayAwsAwa(char* aws, char* awa, int row) {
  lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "AWS       %4.1fkn", AWS_disp);
//sprintf(line, "                ");
  lcd.print(line);
  lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "AWA      %5.1f%cT", AWA_disp, (char)0xDF);
  lcd.print(line);
  if ((row == 0) && (_LCDML_DISP_rows == 4)) {
	  DisplayWaterDepth((char *)"", (char *)"", 2);
  }
}

// page 8
// WATER     18.5°C
// DEPTH      3.2 m
// Water page Water temperature and depth.
void DisplayWaterDepth(char* waterTemp, char* depth, int row) {
  lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "WaterTemp %4.1f%cC", TempWater_disp, (char)0xDF);
//sprintf(line, "                ");
  lcd.print(line);
  lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "Depth     %4.1f m", Depth_disp);
  lcd.print(line);
  if ((row == 0) && (_LCDML_DISP_rows == 4)) {
	  DisplayAirPress((char *)"", (char *)"", 2);
  }
}

// page 9
// AIR TEMP  17.1°C
// ATM PRS 1019.4mb
// Air page Air temperature and atmospheric pressure.
void DisplayAirPress(char* airTemp, char* press, int row) {
  lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "AirTemp   %4.1f%cC", TempAirHum_disp, (char)0xDF);
//sprintf(line, "                ");
  lcd.print(line);
  lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "AtmPress%4.0fmBar", Press_disp);
  lcd.print(line);
  if ((row == 0) && (_LCDML_DISP_rows == 4)) {
	  DisplayLogTrip((char *)"", (char *)"", 2);
  }
}

// page 10
// LOG    1453.54nm
// TRIP    275.81nm
// Log & trip Log and trip distance.
void DisplayLogTrip(char* log, char* trip, int row) {
  lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "LOG  %9.2fnm", LOG_disp);
//sprintf(line, "                ");
  lcd.print(line);
  lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "TRIP   %7.2fnm", TRIP_disp);
  lcd.print(line);
  if ((row == 0) && (_LCDML_DISP_rows == 4)) {
	  DisplayBatt((char *)"", (char *)"", 2);
  }
}

// page 11
// BATTERY1   13.8V
// BATTERY2  ---.-V
// Battery Battery’s voltage.
void DisplayBatt(char* batt1, char* batt2, int row) {
  lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "BATT-1     13.8V");
//sprintf(line, "                ");
  lcd.print(line);
  lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "BATT-2     --.-V");
  lcd.print(line);
  if ((row == 0) && (_LCDML_DISP_rows == 4)) {
	  DisplayEngine((char *)"", (char *)"", 2);
  }
}

// page 12
// ENGINE PORT/STBD
// 1255 ----- rpm
// Engines Port and starboard engines revolutions.
void DisplayEngine(char* rpmPort, char* rpmStbd, int row) {
  lcd.setCursor(0, row);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "ENGINE PORT/STBD");
  lcd.print(line);
  lcd.setCursor(0, row+1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "   1255/---- rpm");
  lcd.print(line);
  if ((row == 0) && (_LCDML_DISP_rows == 4)) {
	  DisplayPressHum((char *)"", (char *)"", 2);
  }
}

// page 13
// Pressure/Humidity
void DisplayPressHum(char* pressure, char* humidity, int row) {
	lcd.setCursor(0, row);   // set the cursor to column 0, line 0  (line 0 is the first line)
	sprintf(line, "P= %4.0f,T=%2.1f%cC", Press_disp, TempAirPress_disp, (char)0xDF);
	lcd.print(line);
	lcd.setCursor(0, row+1);   // set the cursor to column 0, line 1  (line 1 is the second line)
	sprintf(line, "F=  %2.0f%%,T=%2.1f%cC", Hum_disp, TempAirHum_disp, (char)0xDF);
	lcd.print(line);
	if ((row == 0) && (_LCDML_DISP_rows == 4)) {
		DisplayPosition((char *)"", (char *)"", 2);
	}
}

// page 14
// All pages
// suppressed
void DisplaySuppressed() {
  lcd.setCursor(0, 0);     // set the cursor to column 0, line 0  (line 0 is the first line)
  sprintf(line, "All pages       ");
//sprintf(line, "                ");
  lcd.print(line);
  lcd.setCursor(0, 1);     // set the cursor to column 0, line 1  (line 1 is the second line)
  sprintf(line, "suppressed!     ");
  lcd.print(line);
  if (_LCDML_DISP_rows == 4) {
	  lcd.setCursor(0, 2);
	  lcd.print("                ");  // print third line to lcd display
	  lcd.setCursor(0, 3);
	  lcd.print("                ");  // print fourth line to lcd display
  }
}


