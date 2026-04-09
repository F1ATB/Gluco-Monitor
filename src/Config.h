#pragma once
#include <Arduino.h>

#define HOSTNAME "GlucoMonit-"

// Sensor types
enum SensorType {
    SENSOR_LIBRE = 0,
    SENSOR_DEXCOM = 1
};

#define RecurrenceGlycemie 120000 // 2 minutes

//========= MACRO =========
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

//============ Version et Build ==========
extern const char* Version ;
extern const char* BuildDate ;

extern String ssid, password, hostname;
extern String MyIP;

extern String libreEmail;
extern String librePass;
extern String libreZone;
extern bool ServerConnu;

// Dexcom configuration
extern String dexcomUsername;
extern String dexcomPassword;
extern String dexcomRegion;

// Sensor selection
extern SensorType sensorType;

extern const char *regions[12];
extern const char *regionsCode[12];

extern unsigned long lastDemandeGlycMillis, recurGlycMillis, lastReceptionGlycMillis, lastGlycOkMillis;
extern int8_t idxFuseau; // Fuseau Horaire
extern int8_t Jour;      //-1=inconnu,0=dimanche,1=lundi...
extern bool HeureValide;
extern int16_t Int_Heure, Int_Minute;
extern String DATE, HEURE, DateAMJ, Hmn;
extern long AgeGlycemie;
extern uint64_t T_On_seconde;

#define MAX_POINTS 300
extern int16_t glucoseValues[];
extern unsigned long glucoseHeure[];
extern int16_t pointCountGly;
extern String Glycemie;
extern int8_t TrendArrow;
extern unsigned long lastGlyUnixTime;
extern int16_t GlycemieVal, targetLow,targetHigh; 

extern String ES, FS, GS, RS, US;

extern int16_t LuminositeNuit;

extern bool SetupEnCours;

//======= Page HTML Brute ============
extern bool AutorisationPageBrute;
extern unsigned long TimerAutorisationBruteMillis;

// PSRAM
extern EXT_RAM_BSS_ATTR char MessageEcran[];
extern EXT_RAM_BSS_ATTR String LoginJSON, GraphJSON, ConnectionJSON;
