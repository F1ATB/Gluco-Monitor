#include "Config.h"
#include "Heure.h"

#include <Arduino.h>

#include "esp_system.h"
#include "time.h"
#include "esp_sntp.h"

#include <WiFi.h>

#define MAX_SIZE_T 80

// Europe Centrale                                                                                  //-1=inconnu,0=dimache,1=lundi...
//static String codeTZ[] = {"CET-1CEST,M3.5.0,M10.5.0/3", "AST4", "GFT3", "RET-3", "EAT-3", "NCT-11", "WFT-12"}; // Europe centrale, Guadeloupe / Martinique, Guyane, Réunion, Mayotte,Nouvelle Calédonie, Wallis et Futuna
 const char* codeTZ[] =
{
  "UTC0",                               // UTC
  "GMT0",                               // Royaume-Uni hiver

  "CET-1CEST,M3.5.0,M10.5.0/3",         // Europe centrale
  "EET-2EEST,M3.5.0/3,M10.5.0/4",       // Europe de l'Est

  "MSK-3",                              // Russie (Moscou)

  "AST4ADT,M3.2.0,M11.1.0",             // Atlantique Canada
  "EST5EDT,M3.2.0,M11.1.0",             // New York / Est USA
  "CST6CDT,M3.2.0,M11.1.0",             // Chicago
  "MST7MDT,M3.2.0,M11.1.0",             // Denver
  "PST8PDT,M3.2.0,M11.1.0",             // Los Angeles

  "AKST9AKDT,M3.2.0,M11.1.0",           // Alaska
  "HST10",                              // Hawaii

  "BRT3",                               // Brésil
  "ART3",                               // Argentine

  "WAT-1",                              // Afrique Ouest
  "EAT-3",                              // Afrique Est

  "GST-4",                              // Dubaï
  "IST-5:30",                           // Inde

  "THA-7",                              // Thaïlande
  "CST-8",                              // Chine
  "JST-9",                              // Japon
  "KST-9",                              // Corée

  "AEST-10AEDT,M10.1.0,M4.1.0/3",       // Australie Est
  "ACST-9:30ACDT,M10.1.0,M4.1.0/3",     // Australie centre

  "NZST-12NZDT,M9.5.0,M4.1.0/3"         // Nouvelle-Zélande
};
 const char* nomTZ[] =
{
 "UTC (Coordinated Universal Time)",
 "United Kingdom",

 "Central Europe (France, Germany, Italy)",
 "Eastern Europe (Greece, Finland, Romania)",

 "Russia (Moscow)",

 "Canada Atlantic",
 "USA Eastern (New York, Washington)",
 "USA Central (Chicago, Dallas)",
 "USA Mountain (Denver)",
 "USA Pacific (Los Angeles, Seattle)",

 "Alaska",
 "Hawaii",

 "Brazil",
 "Argentina",

 "West Africa (Nigeria, Cameroon)",
 "East Africa (Kenya, Ethiopia)",

 "United Arab Emirates (Dubai)",
 "India",

 "Thailand / Vietnam",
 "China",
 "Japan",
 "South Korea",

 "Australia Eastern (Sydney, Melbourne)",
 "Australia Central (Adelaide)",

 "New Zealand"
};

static_assert(ARRAY_SIZE(codeTZ) == ARRAY_SIZE(nomTZ),
              "Timezone arrays size mismatch");

const int NB_TZ = ARRAY_SIZE(nomTZ);
const char *ntpServer1 = "fr.pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";

long convertToUnix(const char *timestamp);

void FormatteHeureDate()
{
  // Formatte la date et l'heure en String
  char buffer[MAX_SIZE_T];
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  DATE = String(buffer);
  strftime(buffer, sizeof(buffer), "%Y%m%d", &timeinfo);
  DateAMJ = String(buffer);
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
  HEURE = String(buffer);
  strftime(buffer, sizeof(buffer), "%H:%M", &timeinfo);
  Hmn = String(buffer);
  Int_Heure = timeinfo.tm_hour;
  Int_Minute = timeinfo.tm_min;
  Jour = timeinfo.tm_wday; //-1=inconnu,0=dimanche,1=lundi...
  T_On_seconde = esp_timer_get_time() / 1000000; //Timer en microseconde sur 64 bits
  if (Int_Heure < 7 || Int_Heure >= 23)
  {
    ledcWrite(GFX_BL, LuminositeNuit); // Baisser la luminosité la nuit ?????????????????????????????
  }
  else
  {
    ledcWrite(GFX_BL, 255); // Luminosité maximale le jour
  }
}

// **************
// * Heure DATE * -
// **************
void time_sync_notification(struct timeval *tv)
{
  FormatteHeureDate();
  HeureValide = true;
}
void DefFuseauHoraire(void)
{

  // Heure / Hour . A Mettre en priorité avant WIFI (exemple ESP32 Simple Time)
  // External timer to obtain the Hour and reset Watt Hour every day at 0h
  sntp_set_sync_interval(10800000); // Synchro toutes les 3h
  sntp_set_time_sync_notification_cb(time_sync_notification);
  // sntp_servermode_dhcp(1);   Déprecié
  esp_sntp_servermode_dhcp(true);                                  // Option
  configTzTime(codeTZ[idxFuseau], ntpServer1, ntpServer2); // Voir Time-Zone:
}

long convertToUnix(const char *dateStr)
{
  struct tm t;
  int month, day, year, hour, minute, second;
  char period[3]; // Pour stocker "AM" ou "PM"

  // Extraction des données de la chaîne
  // Format : %d/%d/%d %d:%d:%d %2s (Mois/Jour/Année Heure:Min:Sec AM/PM)
  sscanf(dateStr, "%d/%d/%d %d:%d:%d %2s", &month, &day, &year, &hour, &minute, &second, period);

  // Ajustement pour le format AM/PM
  if (strcmp(period, "PM") == 0 && hour < 12)
    hour += 12;
  if (strcmp(period, "AM") == 0 && hour == 12)
    hour = 0;

  // Remplissage de la structure tm
  t.tm_year = year - 1900; // Années depuis 1900
  t.tm_mon = month - 1;    // Mois de 0 à 11
  t.tm_mday = day;
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = second;
  t.tm_isdst = -1; // Ne pas gérer l'heure d'été automatiquement

  // Conversion en temps Unix (mktime)
  return mktime(&t);
}
String unixToTimestamp(time_t unixTime)
{
  struct tm t;

  // Heure locale (respecte le fuseau configuré)
  localtime_r(&unixTime, &t);

  char buffer[32];

  // Format français 24h
  strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &t);

  return String(buffer);
}
int unixToHeure(time_t unixTime)
{
  struct tm t;

  // Heure locale (respecte le fuseau configuré)
  localtime_r(&unixTime, &t);

  char buffer[32];

  // Format français 24h
  strftime(buffer, sizeof(buffer), "%H", &t);

  return atoi(buffer);
}