#define DHTPIN 0      //SIGnal (DHT pin 2) to teensy pin 0
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22
 
DHT dht(DHTPIN, DHTTYPE);
 
#include <SFE_BMP180.h>
#include <Wire.h>

// You will need to create an SFE_BMP180 object, here called "pressure":

SFE_BMP180 pressure;

#define ALTITUDE 750.0 // Altitude of Sonthofen in meters

void setupPressHumTemp() {
  dht.begin();
  if (!pressure.begin())
  {
    lcd.setCursor(0,0);
    lcd.print(F("BMP180 fail"));
    delay(5000);
  }
}

void ReadHumTemp() {
  double T, H;

  H = dht.readHumidity();     //Luftfeuchte auslesen
//  H = H + 30.0;      // Korrektur, humidity zu niedrig
  Hum_disp = H;

  T = dht.readTemperature();  //Temperatur auslesen
//  T = T - 2.0;      // Korrektur, temp zu hoch
  TempAirHum_disp = T;
 
  // Prüfen ob eine gültige Zahl zurückgegeben wird. Wenn NaN (not a number) zurückgegeben wird, dann Fehler ausgeben.
  if (isnan(TempAirHum_disp) || isnan(Hum_disp)) 
  {
    Serial.println("DHT22 konnte nicht ausgelesen werden");
  } 
}

void ReadPressTemp() {
  char status;
  double T, P, P0;

    // If you want sea-level-compensated pressure, as used in weather reports,
    // you will need to know the altitude at which your measurements are taken.
    // We're using a constant called ALTITUDE in this sketch:
  
    // If you want to measure altitude, and not pressure, you will instead need
    // to provide a known baseline pressure. This is shown at the end of the sketch.

    // You must first get a temperature measurement to perform a pressure reading. 
  
    // Start a temperature measurement:
    // If request is successful, the number of ms to wait is returned.
    // If request is unsuccessful, 0 is returned.

    status = pressure.startTemperature();
    if (status != 0)
    {
      // Wait for the measurement to complete:
      delay(status);

      // Retrieve the completed temperature measurement:
      // Note that the measurement is stored in the variable T.
      // Function returns 1 if successful, 0 if failure.

      status = pressure.getTemperature(T);
      if (status != 0)
      {
        // Start a pressure measurement:
        // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
        // If request is successful, the number of ms to wait is returned.
        // If request is unsuccessful, 0 is returned.

        status = pressure.startPressure(3);
        if (status != 0)
        {
          // Wait for the measurement to complete:
          delay(status);

          // Retrieve the completed pressure measurement:
          // Note that the measurement is stored in the variable P.
          // Note also that the function requires the previous temperature measurement (T).
          // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
          // Function returns 1 if successful, 0 if failure.

          status = pressure.getPressure(P,T);
          if (status != 0)
          {
//            T = T - 4.5;      // Korrektur, temp zu hoch, aber für Pressure muss der unkorrigierte wert genommen werden
			  P0 = pressure.sealevel(P, ALTITUDE); // we're at 1655 meters (Boulder, CO)
			  TempAirPress_disp = T;
//			  Press_disp = P;
			  Press_disp = P0;
		  }
          else Serial.println("error retrieving pressure measurement\n");
        }
        else Serial.println("error starting pressure measurement\n");
      }
      else Serial.println("error retrieving temperature measurement\n");
    }
    else Serial.println("error starting temperature measurement\n");
}
