#include <LCDMenuLib.h>

// ============================================================
// 
// NMEAmonitor
// 
// Shows N2K values as COG, lat, lon, etc. displaying up to
// 12 data pages. These pages can be suppressed, if the data  
// is not interesting.
// 
// ============================================================  

#include <LiquidCrystal.h>
#include <LCDMenuLib.h>
#include <Arduino.h>
#include <EEPROM.h>

#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include <N2kMessages.h>
#include <NMEA2000.h>
#include <N2kMsg.h>
#include <N2kCANMsg.h>
#include <NMEA2000_teensy.h>
#include <FlexCAN.h>
#include <ActisenseReader.h>

#include <DHT.h>
  
// lib config
#define _LCDML_DISP_cfg_button_press_time          200    // button press time in ms
#define _LCDML_DISP_cfg_initscreen_time            5000  // enable initscreen time
#define _LCDML_DISP_cfg_scrollbar                  0      // enable a scrollbar
#define _LCDML_DISP_cfg_cursor                     0x7E   // cursor Symbol ->

// ********************************************************************* 
// LCDML TYPE SELECT
// *********************************************************************
// settings for lcd 
#define _LCDML_DISP_cols             16
#define _LCDML_DISP_rows              2  

#define PAGE_DISPLAY                  3               // each page is displayed 3 sec
#define INITIAL_BACKLIT             100               // initial backlit value
#define INITIAL_BACKLIT_TIMEOUT      15               // initial backlit timeout value 15 sec
#define INITIAL_CONTRAST             32               // initial contrast value

#define PIN_ENTER                    10               // button enter
#define PIN_DOWN                     11               // button down
#define PIN_UP                       12               // button up

#define USE_SERIAL                    0               // 1=use serial port, 0=don't use

#define PAGECOUNT                    13               // number of pages to be displayed

#define PROG_NAME    "N2K Monitor+TX  "
#define PROG_VER     "19.10.2018 v1.01"
#define SHOW_NAME_VER                 4               // show name and version for 4 sec

static int  flag[PAGECOUNT] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static char line[]          = "                ";

const int ledBuiltin = LED_BUILTIN;
const int ledFront   = 21;              // front LED

// Connections to LCD:
//
// VO contrast (LCD pin 3) to teensy pin 5 (PWM)
// rs (LCD pin 4) to teensy pin 23
// rw (LCD pin 5) to GND
// enable (LCD pin 6) to teensy pin 22
// Back LED+ (LCD pin 15) to teensy Vin via 100 Ohm
// Back LED- (LCD pin 16) via MOSFET to teensy pin 6 (PWM)
// d4 (LCD pin 11), d5 (LCD pin 12), d6 (LCD pin 13), d7 (LCD pin 14) to teensy pins 17, 16, 15, 14
// LiquidCrystal lcd(RS, Enable, D4, D5, D6, D7)
//
  LiquidCrystal lcd(23, 22, 17, 16, 15, 14);
  const int lcdContrastPin = 5;
  const int lcdBacklitPin  = 6;

  const uint8_t scroll_bar[5][8] = {
    {B10001, B10001, B10001, B10001, B10001, B10001, B10001, B10001}, // scrollbar top
    {B11111, B11111, B10001, B10001, B10001, B10001, B10001, B10001}, // scroll state 1 
    {B10001, B10001, B11111, B11111, B10001, B10001, B10001, B10001}, // scroll state 2
    {B10001, B10001, B10001, B10001, B11111, B11111, B10001, B10001}, // scroll state 3
    {B10001, B10001, B10001, B10001, B10001, B10001, B11111, B11111}  // scrollbar bottom
  }; 

  const uint8_t contrast_char[1][8] = {
    {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B00000}, // contrast symbol
  }; 

// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
  
  // LCDML_root        => layer 0 
  // LCDML_root_X      => layer 1 
  // LCDML_root_X_X    => layer 2 
  // LCDML_root_X_X_X  => layer 3 
  // LCDML_root_...    => layer ... 
  
  // create menu
  // menu element count - last element id
  // this value must be the same as the last menu element

  #define _LCDML_DISP_cnt    9
  
  // LCDML_root        => layer 0 
  // LCDML_root_X      => layer 1 
  // LCDML_root_X_X    => layer 2 
  // LCDML_root_X_X_X  => layer 3 
  // LCDML_root_...    => layer ... 
  
  // LCDMenuLib_add(id, group, prev_layer_element, new_element_num, lang_char_array, callback_function)
  LCDML_DISP_init(_LCDML_DISP_cnt);
  LCDML_DISP_add      (0  , _LCDML_G1  , LCDML_root        , 1  , "About"             , LCDML_FUNC_information);
  LCDML_DISP_add      (1  , _LCDML_G1  , LCDML_root        , 2  , "Settings"          , LCDML_FUNC);
  LCDML_DISP_add      (2  , _LCDML_G1  , LCDML_root        , 3  , "Display pages"     , LCDML_FUNC_initscreen);
  LCDML_DISP_add      (3  , _LCDML_G1  , LCDML_root_2      , 1  , "Contrast"          , LCDML_FUNC_contrast);
  LCDML_DISP_add      (4  , _LCDML_G1  , LCDML_root_2      , 2  , "Backlit value"     , LCDML_FUNC_backlit_value);
  LCDML_DISP_add      (5  , _LCDML_G1  , LCDML_root_2      , 3  , "Backlit timout"    , LCDML_FUNC_backlit_time);
  LCDML_DISP_add      (6  , _LCDML_G1  , LCDML_root_2      , 4  , "Reset pages"       , LCDML_FUNC_reset);
  LCDML_DISP_add      (7  , _LCDML_G1  , LCDML_root_2      , 5  , "Display pages"     , LCDML_FUNC_initscreen);
  LCDML_DISP_add      (8  , _LCDML_G1  , LCDML_root_2      , 6  , "back"              , LCDML_FUNC_back);
  LCDML_DISP_add      (9  , _LCDML_G7  , LCDML_root        , 6  , "DISPLAY"           , LCDML_FUNC_initscreen);      // in G7 => hidden
  LCDML_DISP_createMenu(_LCDML_DISP_cnt);


// ********************************************************************* 
// LCDML BACKEND (core of the menu, do not change here anything yet)
// ********************************************************************* 
  // define backend function  
  #define _LCDML_BACK_cnt    1  // last backend function id
  
  LCDML_BACK_init(_LCDML_BACK_cnt);
  LCDML_BACK_new_timebased_dynamic (0  , ( 20UL )         , _LCDML_start  , LCDML_BACKEND_control);
  LCDML_BACK_new_timebased_dynamic (1  , ( 1000UL )       , _LCDML_stop   , LCDML_BACKEND_menu);
  LCDML_BACK_create();

static int currentBacklit         = INITIAL_BACKLIT;
static int oldBacklit             = INITIAL_BACKLIT;
static int currentBacklitTimeout  = INITIAL_BACKLIT_TIMEOUT;
static int oldBacklitTimeout      = INITIAL_BACKLIT_TIMEOUT;
static int BackLitTimeout         = 0;

static int currentContrast        = INITIAL_CONTRAST;
static int oldContrast            = INITIAL_CONTRAST;

static double LAT_DEG_disp;
static double LAT_MIN_disp;
static double LONG_DEG_disp;
static double LONG_MIN_disp;

static double SOG_disp;
static double COG_disp;

static double STW_disp;
static double HDG_disp;

static double Hum_disp;
static double TempAirHum_disp;
static double Press_disp;
static double TempAirPress_disp;

static double LOG_disp;
static double TRIP_disp;

static double Bft_disp;
static double Gwd_disp;
static char   GwdRose_disp[] = "N ";
static double TWS_disp;
static double TWA_disp;
static double TWD_disp;
static double AWS_disp;
static double AWA_disp;
static double AWArel_disp;
static double AWD_disp;

typedef struct
{
	unsigned char second; // 0-59
	unsigned char minute; // 0-59
	unsigned char hour;   // 0-23
	unsigned char day;    // 1-31
	unsigned char month;  // 1-12
	unsigned int  year;   // 0-9999
}
date_time_t;

static date_time_t DatumZeit_disp;

static double TempWater_disp;
static double Depth_disp;


Stream *OutputStream;


// *********************************************************************
// SETUP
// *********************************************************************
  void setup()
  {  
    int i;
    
    // Set Product information
    NMEA2000.SetProductInformation("00000123", // Manufacturer's Model serial code
      100, // Manufacturer's product code
      PROG_NAME,                 // Manufacturer's Model ID
      PROG_VER,                  // Manufacturer's Software version code
      "sby Teensy-N2K-Device",   // Manufacturer's Model version
      1,                         // LoadEquivalency
      1300,                      // N2kVersion
      1                          // CertificationLevel
    );
    // Det device information
    NMEA2000.SetDeviceInformation(123456,   // Unique number. Use e.g. Serial number.
      130, // Device function=PC Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
      25,  // Device class=Inter/Intranetwork Device. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
      228  // Manufacturer Code=ZF Marine Electronics. Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
    );

#if USE_SERIAL == 1
    while(!Serial);                    // wait until serial ready
    Serial.begin(115200);
	OutputStream = &Serial;
	
	// serial init; only be needed if serial control is used 
    Serial.println(F(PROG_NAME)); // only for examples
    Serial.println(F(PROG_VER)); // only for examples
#else
    Serial.begin(115200);
	OutputStream = &Serial;
#endif

    NMEA2000.SetForwardStream(&Serial);
    //NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);             // Show in clear text. Leave commented for default Actisense format.
    NMEA2000.SetForwardOwnMessages();

    // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
    NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 23);       // 23=SRC-Addr on N2K-Bus
    // NMEA2000.EnableForward(false);                             // Disable all msg forwarding to USB (=Serial)
	NMEA2000.SetMsgHandler(HandleNMEA2000Msg);
	NMEA2000.Open();

    pinMode(ledBuiltin, OUTPUT);
    pinMode(ledFront, OUTPUT);

    // check if EEPROM contains a valid config
    // start reading from the first byte (address 0) of the EEPROM
    if (EEPROM.read(0) == 0xA5) {
      // EEPROM contains valid values for flag and contrast
      oldContrast = EEPROM.read(1);          // read contrast value from EEPROM
      oldBacklit = EEPROM.read(2);           // read backlit value from EEPROM
	  oldBacklitTimeout = EEPROM.read(3);    // read backlit timeout from EEPROM
										     // flag loop
      for ( i=0; i < PAGECOUNT; i++ ){
        flag[i] = EEPROM.read(i+4);
      }
    }
    else {
      // EEPROM contains NO valid values for flag and contrast
      EEPROM.write(0, 0xA5);                 // write signature into EEPROM
      EEPROM.write(1, oldContrast);          // write contrast value into EEPROM
      EEPROM.write(2, oldBacklit);           // write backlit value into EEPROM
	  EEPROM.write(3, oldBacklitTimeout);    // write backlit timeout into EEPROM
									         // flag loop
      for ( i=0; i < PAGECOUNT; i++ ){
        EEPROM.write(i+4, flag[i]);   // write flags into EEPROM
      }
    }

    pinMode(lcdContrastPin, OUTPUT);  
    analogWrite(lcdContrastPin, oldContrast);   

    pinMode(lcdBacklitPin, OUTPUT);  
    analogWrite(lcdBacklitPin, oldBacklit);   

    // LCD Begin
    lcd.begin(_LCDML_DISP_cols,_LCDML_DISP_rows); 
     
    // Up to eight characters of 5x8 pixels can be programmed (numbered 0 to 7)
    // set special chars for scrollbar
    lcd.createChar(0, (uint8_t*)scroll_bar[0]);
    lcd.createChar(1, (uint8_t*)scroll_bar[1]);
    lcd.createChar(2, (uint8_t*)scroll_bar[2]);
    lcd.createChar(3, (uint8_t*)scroll_bar[3]);
    lcd.createChar(4, (uint8_t*)scroll_bar[4]);

    // set special char for contrastbar
    lcd.createChar(5, (uint8_t*)contrast_char[0]);

    // Enable all items with _LCDML_G1
    LCDML_DISP_groupEnable(_LCDML_G1); // enable group 1
  
    // LCDMenu Setup
    LCDML_setup(_LCDML_BACK_cnt);  
       
    lcd.setCursor(0,0);
    lcd.print(F(PROG_NAME));  // print first line to lcd display
    lcd.setCursor(0,1);
    lcd.print(F(PROG_VER));  // print second line to lcd display

    delay(SHOW_NAME_VER*1000);
    
    setupPressHumTemp();

    LCDML_DISP_jumpToFunc(LCDML_FUNC_initscreen); // jump to initscreen     
}

// *********************************************************************
// LOOP
// *********************************************************************
#define PressHumTempUpdatePeriod 2000
static unsigned long PressHumTempUpdated = millis();

void loop()
  { 
	tN2kMsg N2kMsg;
	
	// example for init screen   
    if((millis() - g_lcdml_initscreen) >= _LCDML_DISP_cfg_initscreen_time) {
      g_lcdml_initscreen = millis(); // reset init screen time
      lcd.setCursor(0,1); // clear the secound line on lcd 
      lcd.print(F("      "));
      LCDML_DISP_jumpToFunc(LCDML_FUNC_initscreen); // jump to initscreen     
    }     

    ledFrontUpdate();
 
	if (PressHumTempUpdated + PressHumTempUpdatePeriod < millis()) {
		PressHumTempUpdated = millis();
		ReadHumTemp();
		ReadPressTemp();

		SetN2kEnvironmentalParameters(N2kMsg, 1, N2kts_OutsideTemperature, CToKelvin(TempAirHum_disp), N2khs_OutsideHumidity, Hum_disp, mBarToPascal(Press_disp));  // N2K: 130311 Environmental Parameters(Cabin Temp, Humidity, Pressure)
		NMEA2000.SendMsg(N2kMsg);                                                                                                         // i70: Air-temp, Humidity, Pressure
	}

    NMEA2000.ParseMessages();

    // this function must called here, do not delete it
    LCDML_run(_LCDML_priority); 
  }

#define ledFrontUpdatePeriod 1000

void ledFrontUpdate() {
  static unsigned long ledFrontUpdated=millis();

  if ( ledFrontUpdated+ledFrontUpdatePeriod<millis() ) {
    ledFrontUpdated=millis();
    digitalWrite(ledFront, HIGH);    //LED=on
    delay(100);
    digitalWrite(ledFront, LOW);     //LED=off
  }
}

// *********************************************************************
// check some errors - do not change here anything
// *********************************************************************
# if(_LCDML_DISP_rows > _LCDML_DISP_cfg_max_rows)
# error change value of _LCDML_DISP_cfg_max_rows in LCDMenuLib.h
# endif
# if(_LCDML_DISP_cols > _LCDML_DISP_cfg_max_string_length)
# error change value of _LCDML_DISP_cfg_max_string_length in LCDMenuLib.h
# endif
