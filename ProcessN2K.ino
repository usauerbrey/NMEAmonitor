#include <N2kMessages.h>
#include <N2kMessagesEnumToStr.h>

typedef struct {
  unsigned long PGN;
  void (*Handler)(const tN2kMsg &N2kMsg); 
} tNMEA2000Handler;

void SystemTime(const tN2kMsg &N2kMsg);
void Rudder(const tN2kMsg &N2kMsg);
void VesselHeading(const tN2kMsg &N2kMsg);
void EngineRapid(const tN2kMsg &N2kMsg);
void TransmissionParameters(const tN2kMsg &N2kMsg);
void FluidLevel(const tN2kMsg &N2kMsg);
void OutsideEnvironmental(const tN2kMsg &N2kMsg);
void EnvironmentalParameters(const tN2kMsg &N2kMsg);
void Pressure(const tN2kMsg &N2kMsg);
void Attitude(const tN2kMsg &N2kMsg);
void BinaryStatus(const tN2kMsg &N2kMsg);
void Temperature(const tN2kMsg &N2kMsg);
void TemperatureExt(const tN2kMsg &N2kMsg);
void DCStatus(const tN2kMsg &N2kMsg);
void BatteryConfigurationStatus(const tN2kMsg &N2kMsg);
void Speed(const tN2kMsg &N2kMsg);
void WaterDepth(const tN2kMsg &N2kMsg);
void DistanceLog(const tN2kMsg &N2kMsg);
void COGSOGRapid(const tN2kMsg &N2kMsg);
void GNSSPosition(const tN2kMsg &N2kMsg);
void WindData(const tN2kMsg &N2kMsg);

template<typename T> void PrintLabelValWithConversionCheckUnDef(const char* label, T val, double (*ConvFunc)(double val)=0, bool AddLf=false ) {
  Serial.print(label);
  if (!N2kIsNA(val)) {
    if (ConvFunc) { Serial.print(ConvFunc(val)); } else { Serial.print(val); }
  } 
  else Serial.print("not available");
  if (AddLf) Serial.println();
}

static tNMEA2000Handler NMEA2000Handlers[] = {
	{ 126992L,&SystemTime },
//	{ 126996L,&ProductInformation },
	{ 127245L,&Rudder },
	{ 127250L,&VesselHeading },
	{ 127257L,&Attitude },
//	{ 127258L,&MagneticVariation },
	{ 127488L,&EngineRapid },
	{ 127493L,&TransmissionParameters },
	{ 127501L,&BinaryStatus },
	{ 127505L,&FluidLevel },
	{ 127506L,&DCStatus },
	{ 127513L,&BatteryConfigurationStatus },
	{ 128259L,&Speed },
	{ 128267L,&WaterDepth },
	{ 128275L,&DistanceLog },
//	{ 129025L,&PositionRapid },
	{ 129026L,&COGSOGRapid },
	{ 129029L,&GNSSPosition },
//	{ 129033L,&TimeDate },
//	{ 129038L,&AISClassAPositionReport },
//	{ 129039L,&AISClassBPositionReport },
//	{ 129044L,&Datum },
//	{ 129283L,&CrossTrackError },
//	{ 129291L,&SetDriftRapid },
//	{ 129540L,&GNSSSatsInView },
//	{ 129542L,&GNSSPseudorangeNoiseStatistics },
//	{ 129794L,&AISClassAstatisticVoyageData },
//	{ 129809L,&AISClassBStaticdataPartA },
//	{ 129810L,&AISClassBStaticdataPartB },
	{ 130306L,&WindData },
	{ 130310L,&OutsideEnvironmental },
	{ 130311L,&EnvironmentalParameters },
    { 130312L,&Temperature },
	{ 130314L,&Pressure },
	{ 130316L,&TemperatureExt },
//	{ 130577L,&DirectionData },
	{ 0,0 }
};

//*****************************************************************************
//NMEA 2000 message handler
void HandleNMEA2000Msg(const tN2kMsg &N2kMsg) {
	int iHandler;

	// Find handler
#if USE_SERIAL == 1
	Serial.print("In Main Handler: "); Serial.print(N2kMsg.PGN);
#endif
	for (iHandler = 0; NMEA2000Handlers[iHandler].PGN != 0 && !(N2kMsg.PGN == NMEA2000Handlers[iHandler].PGN); iHandler++);

	if (NMEA2000Handlers[iHandler].PGN != 0) {
#if USE_SERIAL == 1
		Serial.println(" handler found");
#endif
		NMEA2000Handlers[iHandler].Handler(N2kMsg);
	}
	else
	{
#if USE_SERIAL == 1
		Serial.println(" ");
#endif
	}
}

void COGSOGRapid(const tN2kMsg &N2kMsg) {
	// PGN 129026 COG & SOG, Rapid Update.
	// kommt typisch 4x pro Sekunde
	unsigned char SID;
	tN2kHeadingReference HeadingReference;
	double COG, COG2;
	double SOG, SOG2;

	if (ParseN2kCOGSOGRapid(N2kMsg, SID, HeadingReference, COG, SOG))
	{
		COG2 = RadToDeg(COG);  // NMEA2000 ist radians > in Grad umrechnen
		if (COG2 < 0.0)
		{
			COG2 = 360 + COG2; // 0..180 und 0..-179 in 0..360 Grad umrechnen
		}
		SOG2 = SOG*1.9438;    // in kn (NMEA2000 ist m/s) (m/s * 1.9438 = kn)

#if USE_SERIAL == 1
		Serial.print("COG: ");
		Serial.print(COG2, 0);
		Serial.print(" Grad, ");
		Serial.print("SOG: ");
		Serial.print(SOG2, 2);
		Serial.println(" kn ");
#endif
		SOG_disp = SOG2;
		COG_disp = COG2;
	}
	else {
		Serial.print("Fehler PGN: ");
		Serial.println(N2kMsg.PGN);
	}
}

static unsigned short days[4][12] =
{
	{ 0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335 },
	{ 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700 },
	{ 731, 762, 790, 821, 851, 882, 912, 943, 974,1004,1035,1065 },
	{ 1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430 },
};


void DaysSince1970_to_DayMonthYear(unsigned int SystemDate)
{
	unsigned int years;
	unsigned int DaysCount;

//	Serial.print("  DaysSince: "); Serial.println(SystemDate);

	DaysCount = SystemDate;
//	Serial.print("  DaysSince: "); Serial.println(DaysCount);

	years = DaysCount / (365 * 4 + 1) * 4;
//	Serial.print("  years: "); Serial.println(years);
	DaysCount %= 365 * 4 + 1;

	unsigned int year;
	for (year = 3; year>0; year--)
	{
		if (DaysCount >= days[year][0])
			break;
	}

	unsigned int month;
	for (month = 11; month>0; month--)
	{
		if (DaysCount >= days[year][month])
			break;
	}

	DatumZeit_disp.year = years + year + 1970;
	DatumZeit_disp.month = month + 1;
	DatumZeit_disp.day = DaysCount - days[year][month] + 1;
}

void SecondsSinceMidnight_to_HourMinuteSecond(double SystemTime)
{
	unsigned int SecondsCount;
//	Serial.print("  SecondsSince: "); Serial.println(SystemTime);

	SecondsCount = (unsigned int)SystemTime;
//	Serial.print("  SecondsSince: "); Serial.println(SecondsCount);

	DatumZeit_disp.second = SecondsCount % 60;
//	Serial.print("  Seconds: "); Serial.println(DatumZeit_disp.second);

	SecondsCount /= 60;
	DatumZeit_disp.minute = SecondsCount % 60;
//	Serial.print("  Minutes: "); Serial.println(DatumZeit_disp.minute);

	SecondsCount /= 60;
	DatumZeit_disp.hour = SecondsCount % 24;
//	Serial.print("  hour: "); Serial.println(DatumZeit_disp.hour);
}

void GNSSPosition(const tN2kMsg &N2kMsg) {
	// PGN 129029 GNSS Position Data
	// kommt typisch 1x pro Sekunde
	unsigned char SID;
	uint16_t DaysSince1970;
	double SecondsSinceMidnight;
	double Latitude;
	double Longitude;
	double Altitude;
	tN2kGNSStype GNSStype;
	tN2kGNSSmethod GNSSmethod;
	unsigned char nSatellites;
	double HDOP;
	double PDOP;
	double GeoidalSeparation;
	unsigned char nReferenceStations;
	tN2kGNSStype ReferenceStationType;
	uint16_t ReferenceSationID;
	double AgeOfCorrection;

	if (ParseN2kGNSS(N2kMsg, SID, DaysSince1970, SecondsSinceMidnight,
		Latitude, Longitude, Altitude, GNSStype, GNSSmethod,
		nSatellites, HDOP, PDOP, GeoidalSeparation,
		nReferenceStations, ReferenceStationType, ReferenceSationID,
		AgeOfCorrection)) {
		if (SecondsSinceMidnight > 0) {
			DaysSince1970_to_DayMonthYear(DaysSince1970);
			SecondsSinceMidnight_to_HourMinuteSecond(SecondsSinceMidnight);
		}
#if USE_SERIAL == 1
		Serial.print("GNSS info: "); Serial.println(SID);
		Serial.print("  days since 1.1.1970: "); Serial.println(DaysSince1970);
		Serial.print("  seconds since midnight: "); Serial.println(SecondsSinceMidnight);
		Serial.print("  latitude: "); Serial.println(Latitude);
		Serial.print("  longitude: "); Serial.println(Longitude);
		Serial.print("  altitude: "); Serial.println(Altitude);
		Serial.print("  GNSS type: "); PrintN2kEnumType(GNSStype, OutputStream);
		Serial.print("  GNSS method: "); PrintN2kEnumType(GNSSmethod, OutputStream);
		Serial.print("  satellite count: "); Serial.println(nSatellites);
		Serial.print("  HDOP: "); Serial.println(HDOP);
		Serial.print("  PDOP: "); Serial.println(PDOP);
		Serial.print("  geoidal separation: "); Serial.println(GeoidalSeparation);
		Serial.print("  reference stations: "); Serial.println(nReferenceStations);
#endif
    	LAT_DEG_disp = Latitude;
		LAT_MIN_disp = 12.345;
		LONG_DEG_disp = Longitude;
		LONG_MIN_disp = 23.456;
	}
	else {
		Serial.print("Fehler PGN: ");
		Serial.println(N2kMsg.PGN);
	}
}

void SystemTime(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	uint16_t SystemDate;
	double SystemTime;
	tN2kTimeSource TimeSource;

	if (ParseN2kSystemTime(N2kMsg, SID, SystemDate, SystemTime, TimeSource)) {
		if (SystemDate > 0) {
			DaysSince1970_to_DayMonthYear(SystemDate);
			SecondsSinceMidnight_to_HourMinuteSecond(SystemTime);
		}
#if USE_SERIAL == 1
		Serial.print("System time: "); Serial.println(SID);
		Serial.print("  days since 1.1.1970: "); Serial.println(SystemDate);
		Serial.print("  seconds since midnight: "); Serial.println(SystemTime);
		Serial.print("  time source: "); PrintN2kEnumType(TimeSource, OutputStream);
#endif
	}
	else
	{
		Serial.print("Failed to parse PGN: "); Serial.println(N2kMsg.PGN);
	}
}

void DistanceLog(const tN2kMsg &N2kMsg) {
	uint16_t SystemDate;
	double SystemTime;
	uint32_t Log;
	uint32_t TripLog;

	if (ParseN2kDistanceLog(N2kMsg, SystemDate, SystemTime, Log, TripLog)) {
		LOG_disp = (double)Log / 1852.0;
		TRIP_disp = (double)TripLog / 1852.0;
#if USE_SERIAL == 1
		Serial.print("Distance Log: ");
		Serial.print("  days since 1.1.1970: "); Serial.println(SystemDate);
		Serial.print("  seconds since midnight: "); Serial.println(SystemTime);
		Serial.print("  Log(m/nm): "); Serial.print(Log); Serial.print("/"); Serial.println(LOG_disp);
		Serial.print("  TripLog(m/nm): "); Serial.print(TripLog); Serial.print("/"); Serial.println(TRIP_disp);
#endif
	}
	else
	{
		Serial.print("Failed to parse PGN: "); Serial.println(N2kMsg.PGN);
	}
}

void Rudder(const tN2kMsg &N2kMsg) {
	unsigned char Instance;
	tN2kRudderDirectionOrder RudderDirectionOrder;
	double RudderPosition;
	double AngleOrder;

	if (ParseN2kRudder(N2kMsg, RudderPosition, Instance, RudderDirectionOrder, AngleOrder)) {
#if USE_SERIAL == 1
		PrintLabelValWithConversionCheckUnDef("Rudder: ", Instance, 0, true);
		PrintLabelValWithConversionCheckUnDef("  position (deg): ", RudderPosition, &RadToDeg, true);
		Serial.print("  direction order: "); PrintN2kEnumType(RudderDirectionOrder, OutputStream);
		PrintLabelValWithConversionCheckUnDef("  angle order (deg): ", AngleOrder, &RadToDeg, true);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: "); OutputStream->println(N2kMsg.PGN);
	}
}

void EngineRapid(const tN2kMsg &N2kMsg) {
	unsigned char EngineInstance;
	double EngineSpeed;
	double EngineBoostPressure;
	signed char EngineTiltTrim;

	if (ParseN2kEngineParamRapid(N2kMsg, EngineInstance, EngineSpeed, EngineBoostPressure, EngineTiltTrim)) {
#if USE_SERIAL == 1
		Serial.print("Engine rapid params: "); Serial.println(EngineInstance);
		Serial.print("  RPM: "); Serial.println(EngineSpeed);
		Serial.print("  boost pressure: "); Serial.println(EngineBoostPressure);
		Serial.print("  tilt trim: "); Serial.println(EngineTiltTrim);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: "); Serial.println(N2kMsg.PGN);
	}
}

void EngineDynamicParameters(const tN2kMsg &N2kMsg) {
	unsigned char EngineInstance;
	double EngineOilPress;
	double EngineOilTemp;
	double EngineCoolantTemp;
	double AltenatorVoltage;
	double FuelRate;
	double EngineHours;
	double EngineCoolantPress;
	double EngineFuelPress;
	int8_t EngineLoad;
	int8_t EngineTorque;

	if (ParseN2kEngineDynamicParam(N2kMsg, EngineInstance, EngineOilPress, EngineOilTemp, EngineCoolantTemp,
		AltenatorVoltage, FuelRate, EngineHours,
		EngineCoolantPress, EngineFuelPress,
		EngineLoad, EngineTorque)) {
#if USE_SERIAL == 1
		PrintLabelValWithConversionCheckUnDef("Engine dynamic params: ", EngineInstance, 0, true);
		PrintLabelValWithConversionCheckUnDef("  oil pressure (Pa): ", EngineOilPress, 0, true);
		PrintLabelValWithConversionCheckUnDef("  oil temp (C): ", EngineOilTemp, &KelvinToC, true);
		PrintLabelValWithConversionCheckUnDef("  coolant temp (C): ", EngineCoolantTemp, &KelvinToC, true);
		PrintLabelValWithConversionCheckUnDef("  altenator voltage (V): ", AltenatorVoltage, 0, true);
		PrintLabelValWithConversionCheckUnDef("  fuel rate (l/h): ", FuelRate, 0, true);
		PrintLabelValWithConversionCheckUnDef("  engine hours (h): ", EngineHours, &SecondsToh, true);
		PrintLabelValWithConversionCheckUnDef("  coolant pressure (Pa): ", EngineCoolantPress, 0, true);
		PrintLabelValWithConversionCheckUnDef("  fuel pressure (Pa): ", EngineFuelPress, 0, true);
		PrintLabelValWithConversionCheckUnDef("  engine load (%): ", EngineLoad, 0, true);
		PrintLabelValWithConversionCheckUnDef("  engine torque (%): ", EngineTorque, 0, true);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: "); OutputStream->println(N2kMsg.PGN);
	}
}

void TransmissionParameters(const tN2kMsg &N2kMsg) {
	unsigned char EngineInstance;
	tN2kTransmissionGear TransmissionGear;
	double OilPressure;
	double OilTemperature;
	unsigned char DiscreteStatus1;

	if (ParseN2kTransmissionParameters(N2kMsg, EngineInstance, TransmissionGear, OilPressure, OilTemperature, DiscreteStatus1)) {
#if USE_SERIAL == 1
		Serial.print("Transmission params: "); Serial.println(EngineInstance);
		Serial.print("  gear: "); PrintN2kEnumType(TransmissionGear, OutputStream);
		Serial.print("  oil pressure: "); Serial.println(OilPressure);
		Serial.print("  oil temperature: "); Serial.println(KelvinToC(OilTemperature));
		Serial.print("  discrete status: "); Serial.println(DiscreteStatus1);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: "); Serial.println(N2kMsg.PGN);
	}
}

void WindData(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	double WindSpeed;
	double WindAngle;
	tN2kWindReference WindReference;

	double u, v;

	if (ParseN2kWindSpeed(N2kMsg, SID, WindSpeed, WindAngle, WindReference)) {
		AWS_disp = (double)WindSpeed * 1.9438;    // in kn (NMEA2000 ist m/s) (m/s * 1.9438 = kn)
		AWA_disp = RadToDeg(WindAngle);           // Apparent Wind Angle (0-360) (relative to bow) 
		AWD_disp = HDG_disp + AWA_disp;           // Apparent Wind Direction (0-360) (relative to true north) AWD = HDR + AWA
		if (AWD_disp > 360.0) {
			AWD_disp = AWD_disp - 360.0;
		}

		if (AWA_disp > 180.0) {
			AWArel_disp = -(AWA_disp - 180); // Apparent Wind Angle rel (relative to the bow, 0 to 180, starboard plus, port minus)
		}
		else {
			AWArel_disp = AWA_disp;          // Apparent Wind Angle rel (relative to the bow, 0 to 180, starboard plus, port minus)
		}

		if (SOG_disp < 0.5) {
			COG_disp = HDG_disp;
		}

#if USE_SERIAL == 1
		PrintLabelValWithConversionCheckUnDef("WindData: ", SID, 0, true);
		PrintLabelValWithConversionCheckUnDef(" AWS (kn): ", AWS_disp, 0, true);
		PrintLabelValWithConversionCheckUnDef(" AWA (deg): ", AWA_disp, 0, true);
		PrintLabelValWithConversionCheckUnDef(" HDG (deg): ", HDG_disp, 0, true);
		PrintLabelValWithConversionCheckUnDef(" AWD (deg): ", AWD_disp, 0, true);
		PrintLabelValWithConversionCheckUnDef(" AWArel (deg): ", AWArel_disp, 0, true);
		PrintLabelValWithConversionCheckUnDef(" SOG (kn): ", SOG_disp, 0, true);
		PrintLabelValWithConversionCheckUnDef(" COG (deg): ", COG_disp, 0, true);
		PrintLabelValWithConversionCheckUnDef(" STW (kn): ", STW_disp, 0, true);
#endif

		u = SOG_disp * sin(DegToRad(COG_disp)) - AWS_disp * sin(DegToRad(AWD_disp));
		v = SOG_disp * cos(DegToRad(COG_disp)) - AWS_disp * cos(DegToRad(AWD_disp));

		TWS_disp = sqrt(u*u + v*v);
		TWD_disp = RadToDeg(atan(u / v));
		if (TWD_disp > 360) {
			TWD_disp = TWD_disp - 360;
		}
		if (TWD_disp < 0) {
			TWD_disp = TWD_disp + 360;
		}

		TWA_disp = TWD_disp - HDG_disp;           // True Wind Angle (0-360) (relative to true north)
		if (TWA_disp < 0) {
			TWA_disp = TWA_disp + 360;
		}
		Gwd_disp = TWD_disp;

		if (TWS_disp < 1) {
			Bft_disp = 0;
		}
		else if (TWS_disp <= 3.5) {
			Bft_disp = 1;
		}
		else if (TWS_disp <= 6.5) {
			Bft_disp = 2;
		}
		else if (TWS_disp <= 10.5) {
			Bft_disp = 3;
		}
		else if (TWS_disp <= 15.5) {
			Bft_disp = 4;
		}
		else if (TWS_disp <= 21.5) {
			Bft_disp = 5;
		}
		else if (TWS_disp <= 27.5) {
			Bft_disp = 6;
		}
		else if (TWS_disp <= 33.5) {
			Bft_disp = 7;
		}
		else if (TWS_disp <= 40.5) {
			Bft_disp = 8;
		}
		else if (TWS_disp <= 47.5) {
			Bft_disp = 9;
		}
		else if (TWS_disp <= 55.5) {
			Bft_disp = 10;
		}
		else if (TWS_disp <= 63.5) {
			Bft_disp = 11;
		}
		else {
			Bft_disp = 12;
		}

		if (TWD_disp < 22.5 + 0) {
			sprintf(GwdRose_disp, "N ");
		}
		else if (TWD_disp < 22.5 + 45) {
			sprintf(GwdRose_disp, "NE");
		}
		else if (TWD_disp < 22.5 + 90) {
			sprintf(GwdRose_disp, "E ");
		}
		else if (TWD_disp < 22.5 + 135) {
			sprintf(GwdRose_disp, "SE");
		}
		else if (TWD_disp < 22.5 + 180) {
			sprintf(GwdRose_disp, "S ");
		}
		else if (TWD_disp < 22.5 + 225) {
			sprintf(GwdRose_disp, "SW");
		}
		else if (TWD_disp < 22.5 + 270) {
			sprintf(GwdRose_disp, "W ");
		}
		else if (TWD_disp < 22.5 + 315) {
			sprintf(GwdRose_disp, "NW");
		}
		else {
			sprintf(GwdRose_disp, "N ");
		}

#if USE_SERIAL == 1
		PrintLabelValWithConversionCheckUnDef(" TWS (kn): ", TWS_disp, 0, true);
		PrintLabelValWithConversionCheckUnDef(" TWD (deg): ", TWD_disp, 0, true);
		PrintLabelValWithConversionCheckUnDef(" TWA (deg): ", TWA_disp, 0, true);
		//		Serial.print("WindReference: "); PrintN2kEnumType(WindReference, OutputStream, false);
		PrintLabelValWithConversionCheckUnDef(" BFT: ", Bft_disp, 0, true);
		Serial.print(" Direction: ");  Serial.println(GwdRose_disp);
#endif

		//		TWS_disp = sqrt((STW_disp*STW_disp + AWS_disp*AWS_disp) - (2 * STW_disp*AWS_disp*cos(DegToRad(AWA_disp))));
		//		TWA_disp = RadToDeg(acos((AWS_disp*AWS_disp - TWS_disp*TWS_disp - STW_disp*STW_disp) / (2 * TWS_disp*STW_disp)));

	}
	else {
		Serial.print("Failed to parse PGN: "); Serial.println(N2kMsg.PGN);
	}
}

void VesselHeading(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	tN2kHeadingReference HeadingReference;
	double Heading;
	double Deviation;
	double Variation;

	if (ParseN2kHeading(N2kMsg, SID, Heading, Deviation, Variation, HeadingReference)) {
#if USE_SERIAL == 1
		PrintLabelValWithConversionCheckUnDef("Heading: ", SID, 0, true);
		OutputStream->print("  reference: "); PrintN2kEnumType(HeadingReference, OutputStream);
		PrintLabelValWithConversionCheckUnDef("  Heading (deg): ", Heading, &RadToDeg, true);
		PrintLabelValWithConversionCheckUnDef("  Deviation (deg): ", Deviation, &RadToDeg, true);
		PrintLabelValWithConversionCheckUnDef("  Variation (deg): ", Variation, &RadToDeg, true);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: "); OutputStream->println(N2kMsg.PGN);
	}
	HDG_disp = RadToDeg(Heading);
}
void OutsideEnvironmental(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	double WaterTemperature;
	double OutsideAmbientAirTemperature;
	double AtmosphericPressure;


	if (ParseN2kOutsideEnvironmentalParameters(N2kMsg, SID, WaterTemperature, OutsideAmbientAirTemperature, AtmosphericPressure)) {
#if USE_SERIAL == 1
		PrintLabelValWithConversionCheckUnDef("Water temp: ", WaterTemperature, &KelvinToC);
		PrintLabelValWithConversionCheckUnDef(", outside ambient temp: ", OutsideAmbientAirTemperature, &KelvinToC);
		PrintLabelValWithConversionCheckUnDef(", pressure: ", AtmosphericPressure, 0, true);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: ");  Serial.println(N2kMsg.PGN);
	}
	if (!N2kIsNA(WaterTemperature)) {
		TempWater_disp = KelvinToC(WaterTemperature);
	}
}

void EnvironmentalParameters(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	tN2kTempSource TempSource;
	double Temperature;
	tN2kHumiditySource HumiditySource;
	double Humidity;
	double AtmosphericPressure;


	if (ParseN2kEnvironmentalParameters(N2kMsg, SID, TempSource, Temperature, HumiditySource, Humidity, AtmosphericPressure)) {
#if USE_SERIAL == 1
		Serial.print("Temperature source: "); PrintN2kEnumType(TempSource, OutputStream, false);
		PrintLabelValWithConversionCheckUnDef("Temparature: ", Temperature, &KelvinToC);
//		Serial.print("Humidity source: "); PrintN2kEnumType(HumiditySource, OutputStream, false);
		PrintLabelValWithConversionCheckUnDef(", humidity: ", Humidity, 0, true);
		PrintLabelValWithConversionCheckUnDef(", pressure: ", AtmosphericPressure, 0, true);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: ");  Serial.println(N2kMsg.PGN);
	}
	if (TempSource == N2kts_SeaTemperature)
	{
		TempWater_disp = KelvinToC(Temperature);
	}
}

void Temperature(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	unsigned char TempInstance;
	tN2kTempSource TempSource;
	double ActualTemperature;
	double SetTemperature;

	if (ParseN2kTemperature(N2kMsg, SID, TempInstance, TempSource, ActualTemperature, SetTemperature)) {
#if USE_SERIAL == 1
		Serial.print("Temperature source: "); PrintN2kEnumType(TempSource, OutputStream, false);
		PrintLabelValWithConversionCheckUnDef(", actual temperature: ", ActualTemperature, &KelvinToC);
		PrintLabelValWithConversionCheckUnDef(", set temperature: ", SetTemperature, &KelvinToC, true);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: ");  Serial.println(N2kMsg.PGN);
	}
}

void Pressure(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	unsigned char Instance;
	tN2kPressureSource PressureSource;
	double ActualPressure;

	if (ParseN2kPressure(N2kMsg, SID, Instance, PressureSource, ActualPressure)) {
#if USE_SERIAL == 1
		Serial.print("Pressure source: "); PrintN2kEnumType(PressureSource, OutputStream, false);
		PrintLabelValWithConversionCheckUnDef(", pressure: ", ActualPressure, &PascalTomBar, true);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: ");  OutputStream->println(N2kMsg.PGN);
	}
	Press_disp = PascalTomBar(ActualPressure);
}

void TemperatureExt(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	unsigned char TempInstance;
	tN2kTempSource TempSource;
	double ActualTemperature;
	double SetTemperature;

	if (ParseN2kTemperatureExt(N2kMsg, SID, TempInstance, TempSource, ActualTemperature, SetTemperature)) {
#if USE_SERIAL == 1
		Serial.print("Temperature source: "); PrintN2kEnumType(TempSource, OutputStream, false);
		PrintLabelValWithConversionCheckUnDef(", actual temperature: ", ActualTemperature, &KelvinToC);
		PrintLabelValWithConversionCheckUnDef(", set temperature: ", SetTemperature, &KelvinToC, true);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: ");  Serial.println(N2kMsg.PGN);
	}
}

void BatteryConfigurationStatus(const tN2kMsg &N2kMsg) {
	unsigned char BatInstance;
	tN2kBatType BatType;
	tN2kBatEqSupport SupportsEqual;
	tN2kBatNomVolt BatNominalVoltage;
	tN2kBatChem BatChemistry;
	double BatCapacity;
	int8_t BatTemperatureCoefficient;
	double PeukertExponent;
	int8_t ChargeEfficiencyFactor;

	if (ParseN2kBatConf(N2kMsg, BatInstance, BatType, SupportsEqual, BatNominalVoltage, BatChemistry, BatCapacity, BatTemperatureCoefficient, PeukertExponent, ChargeEfficiencyFactor)) {
#if USE_SERIAL == 1
		Serial.print("Battery instance: ");
		Serial.println(BatInstance);
		Serial.print("  - type: "); PrintN2kEnumType(BatType, OutputStream);
		Serial.print("  - support equal.: "); PrintN2kEnumType(SupportsEqual, OutputStream);
		Serial.print("  - nominal voltage: "); PrintN2kEnumType(BatNominalVoltage, OutputStream);
		Serial.print("  - chemistry: "); PrintN2kEnumType(BatChemistry, OutputStream);
		PrintLabelValWithConversionCheckUnDef("  - capacity (Ah): ", BatCapacity, &CoulombToAh, true);
		PrintLabelValWithConversionCheckUnDef("  - temperature coefficient (%): ", BatTemperatureCoefficient, 0, true);
		PrintLabelValWithConversionCheckUnDef("  - peukert exponent: ", PeukertExponent, 0, true);
		PrintLabelValWithConversionCheckUnDef("  - charge efficiency factor (%): ", ChargeEfficiencyFactor, 0, true);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: "); Serial.println(N2kMsg.PGN);
	}
}

void DCStatus(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	unsigned char DCInstance;
	tN2kDCType DCType;
	unsigned char StateOfCharge;
	unsigned char StateOfHealth;
	double TimeRemaining;
  double RippleVoltage;
  double Capacity;

	if (ParseN2kDCStatus(N2kMsg, SID, DCInstance, DCType, StateOfCharge, StateOfHealth, TimeRemaining, RippleVoltage, Capacity)) {
#if USE_SERIAL == 1
		Serial.print("DC instance: ");
		Serial.println(DCInstance);
		Serial.print("  - type: "); PrintN2kEnumType(DCType, OutputStream);
		Serial.print("  - state of charge (%): "); Serial.println(StateOfCharge);
		Serial.print("  - state of health (%): "); Serial.println(StateOfHealth);
		Serial.print("  - time remaining (h): "); Serial.println(TimeRemaining / 60);
		Serial.print("  - ripple voltage: "); Serial.println(RippleVoltage);
    Serial.print("  - capacity: "); Serial.println(Capacity);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: ");  Serial.println(N2kMsg.PGN);
	}
}

void Speed(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	double SOW;
	double SOG;
	tN2kSpeedWaterReferenceType SWRT;

	if (ParseN2kBoatSpeed(N2kMsg, SID, SOW, SOG, SWRT)) {
#if USE_SERIAL == 1
		Serial.print("Boat speed:");
		PrintLabelValWithConversionCheckUnDef(" SOW:", N2kIsNA(SOW) ? SOW : msToKnots(SOW));
		PrintLabelValWithConversionCheckUnDef(", SOG:", N2kIsNA(SOG) ? SOG : msToKnots(SOG));
		Serial.print(", ");
		PrintN2kEnumType(SWRT, OutputStream, true);
#endif
		STW_disp = SOW;
	}
}

void WaterDepth(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	double DepthBelowTransducer;
	double Offset;

	if (ParseN2kWaterDepth(N2kMsg, SID, DepthBelowTransducer, Offset)) {
#if USE_SERIAL == 1
		if (Offset>0)
		{
			Serial.print("Water depth:");
		}
		else
		{
			if (Offset == 0)
			{
				Serial.print("Depth below Transducer:");
			}
			else
			{
				Serial.print("Depth below keel:");
			}
		}
		Serial.print(DepthBelowTransducer + Offset);
		Serial.print(" Offset:");
		Serial.println(Offset);
#endif
		Depth_disp = DepthBelowTransducer;
	}
}

void printLLNumber(Stream *OutputStream, unsigned long long n, uint8_t base = 10)
{
	unsigned char buf[16 * sizeof(long)]; // Assumes 8-bit chars.
	unsigned long long i = 0;

	if (n == 0) {
		Serial.print('0');
		return;
	}

	while (n > 0) {
		buf[i++] = n % base;
		n /= base;
	}

	for (; i > 0; i--)
		Serial.print((char)(buf[i - 1] < 10 ?
			'0' + buf[i - 1] :
			'A' + buf[i - 1] - 10));
}

void BinaryStatusFull(const tN2kMsg &N2kMsg) {
	unsigned char BankInstance;
	tN2kBinaryStatus BankStatus;

	if (ParseN2kBinaryStatus(N2kMsg, BankInstance, BankStatus)) {
#if USE_SERIAL == 1
			Serial.print("Binary status for bank "); Serial.print(BankInstance); Serial.println(":");
		Serial.print("  "); //printLLNumber(OutputStream,BankStatus,16);
		for (uint8_t i = 1; i <= 28; i++) {
			if (i>1) Serial.print(",");
			PrintN2kEnumType(N2kGetStatusOnBinaryStatus(BankStatus, i), OutputStream, false);
		}
		Serial.println();
#endif
	}
}

void BinaryStatus(const tN2kMsg &N2kMsg) {
	unsigned char BankInstance;
	tN2kOnOff Status1, Status2, Status3, Status4;

	if (ParseN2kBinaryStatus(N2kMsg, BankInstance, Status1, Status2, Status3, Status4)) {
		if (BankInstance>2) { // note that this is only for testing different methods. MessageSender.ini sends 4 status for instace 2
			BinaryStatusFull(N2kMsg);
		}
		else {
#if USE_SERIAL == 1
			Serial.print("Binary status for bank "); Serial.print(BankInstance); Serial.println(":");
			Serial.print("  Status1="); PrintN2kEnumType(Status1, OutputStream, false);
			Serial.print(", Status2="); PrintN2kEnumType(Status2, OutputStream, false);
			Serial.print(", Status3="); PrintN2kEnumType(Status3, OutputStream, false);
			Serial.print(", Status4="); PrintN2kEnumType(Status4, OutputStream, false);
			Serial.println();
#endif
		}
	}
}

void FluidLevel(const tN2kMsg &N2kMsg) {
	unsigned char Instance;
	tN2kFluidType FluidType;
	double Level = 0;
	double Capacity = 0;

	if (ParseN2kFluidLevel(N2kMsg, Instance, FluidType, Level, Capacity)) {
		switch (FluidType) {
		case N2kft_Fuel:
#if USE_SERIAL == 1
			Serial.print("Fuel level :");
#endif
			break;
		case N2kft_Water:
#if USE_SERIAL == 1
			Serial.print("Water level :");
#endif
			break;
		case N2kft_GrayWater:
#if USE_SERIAL == 1
			Serial.print("Gray water level :");
#endif
			break;
		case N2kft_LiveWell:
#if USE_SERIAL == 1
			Serial.print("Live well level :");
#endif
			break;
		case N2kft_Oil:
#if USE_SERIAL == 1
			Serial.print("Oil level :");
#endif
			break;
		case N2kft_BlackWater:
#if USE_SERIAL == 1
			Serial.print("Black water level :");
#endif
			break;
		}
#if USE_SERIAL == 1
		Serial.print(Level); Serial.print("%");
		Serial.print(" ("); Serial.print(Capacity*Level / 100); Serial.print("l)");
		Serial.print(" capacity :"); Serial.println(Capacity);
#endif
	}
}

void Attitude(const tN2kMsg &N2kMsg) {
	unsigned char SID;
	double Yaw;
	double Pitch;
	double Roll;

	if (ParseN2kAttitude(N2kMsg, SID, Yaw, Pitch, Roll)) {
#if USE_SERIAL == 1
		PrintLabelValWithConversionCheckUnDef("Attitude: ", SID, 0, true);
		PrintLabelValWithConversionCheckUnDef("  Yaw (deg): ", Yaw, &RadToDeg, true);
		PrintLabelValWithConversionCheckUnDef("  Pitch (deg): ", Pitch, &RadToDeg, true);
		PrintLabelValWithConversionCheckUnDef("  Roll (deg): ", Roll, &RadToDeg, true);
#endif
	}
	else {
		Serial.print("Failed to parse PGN: "); OutputStream->println(N2kMsg.PGN);
	}
}
