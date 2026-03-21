#include "Config.h"
#include <Arduino.h>




//======= VARIABLES ========
String ssid = "", password = "", hostname = "";
String MyIP = "0.0.0.0";

String  libreEmail = "";
String librePass = "";
String libreZone = "";
bool ServerConnu = false;

//Regions possibles pour LibreLinkUp
const char *regions[12] = {"General", "", "Europe", " Europe 2", "France", "Germany", "USA", "Canada", "Australia", "Japan ", "Asia Pacific", "UAE"};
const char *regionsCode[12] = {"", "", "eu", "eu2", "fr", "de", "us", "ca", "au", "jp", "ap", "ae"};

// Timers
unsigned long lastDemandeGlycMillis = 0;
unsigned long recurGlycMillis = 120000; // 2 minutes
unsigned long lastReceptionGlycMillis = 0;
unsigned long lastGlycOkMillis = 0;
long AgeGlycemie = 0;


// Heure
int8_t idxFuseau = 2; // Fuseau Horaire
int8_t Jour;        //-1=inconnu,0=dimanche,1=lundi...
bool HeureValide=false;
int16_t Int_Heure, Int_Minute;
String DATE, HEURE, DateAMJ, Hmn;
uint64_t T_On_seconde = 0;




// Glycémie
int16_t glucoseValues[MAX_POINTS]; // Tableau glycemie sur environ 24h
unsigned long glucoseHeure[MAX_POINTS]; // Heure glycemie sur environ  24h
int16_t pointCountGly = 0; // Nombre de points de glycémie actuellement stockés
String Glycemie = "";
int16_t GlycemieVal=0;
int8_t TrendArrow = 0; // 0=non défini, 1=Flèche vers le bas, 2=Flèche vers le bas à droite, 3=Flèche vers la droite, 4=Flèche vers le haut à droite, 5=Flèche vers le haut
unsigned long lastGlyUnixTime = 0; // Heure de la dernière glycémie reçue en format Unix Time
int16_t targetLow=70,targetHigh=180; //Seuils zone verte

String ES = String((char)27); // ESC Separator
String FS = String((char)28); // File Separator
String GS = String((char)29); // Group Separator
String RS = String((char)30); // Record Separator
String US = String((char)31); // Unit Separator

int16_t LuminositeNuit=255; //Maximum

//======= Page HTML Brute ============
bool AutorisationPageBrute=false;
unsigned long TimerAutorisationBruteMillis=0;

// PSRAM
EXT_RAM_BSS_ATTR char MessageEcran[8192];
EXT_RAM_BSS_ATTR String LoginJSON = "", GraphJSON = "",ConnectionJSON = "";


    