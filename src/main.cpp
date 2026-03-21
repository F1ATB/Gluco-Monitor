/*
 * FreeStyle, Libre, and related brand marks are trademarks of Abbott Diabetes Care Inc. in various jurisdictions.
 Other trademarks are the property of their respective owners.
 * This documentation is not affiliated with Abbott Diabetes Care, Inc. or any of its subsidiaries
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "mbedtls/md.h"
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#include "Heure.h"
#include "Libreview.h"
#include "Config.h"
#include "Stock.h"
#include "Serie.h"
#include "Internet.h"
#include "Server.h"
#include "Ecran/Gestion.h"
#include "Ecran/pageMessages.h"
#include "Ecran/pageAccueil.h"
#include "Ecran/pageConfiguration.h"
#include "Ecran/pageLibreServeur.h"
#include "Ecran/pageCompte.h"
#include "Ecran/pageInfos.h"

static unsigned long testMillis = 0;

void setup()
{
  Serial.begin(115200);
  for (int i = 0; i < MAX_POINTS; i++) // Tableau des glycémies sur 24h
  {
    glucoseValues[i] = 0;
    glucoseHeure[i] = 0;
  }

  LireSerial();
  InitStock(); // Init LittleFS
  if (psramInit())
  {
    Serial.println("PSRAM  correctement initialisée");
  }
  else
  {
    Serial.println("La PSRAM ne fonctionne pas");
  }
  LireSerial();
  ReadFichierParametres();
  LireSerial();
  InitEcran();
  LireSerial();
  Init_Internet();
  CanvaBase->flush();
  Init_Server();
  LireSerial();

  //  ========Modification du programme par le Wifi  - OTA(On The Air) ================

  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin(); // Mandatory

  LireSerial();

  Serial.printf("PSRAM: %d\n", psramFound());
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
}

void loop()
{

  LireSerial();
  if (HeureValide)
  {
    LectureGlycemie();
    FormatteHeureDate();
  }
  loopEcran();

  if (millis() - lastGlycOkMillis > 1210000) // Si on n'a pas réussi à récupérer une glycémie depuis plus de 20 minutes, on redémarre le module pour tenter de résoudre les problèmes de communication
    AlertePasdeGlycemie();

  //======= Page HTML Brute ============

  if (millis() - TimerAutorisationBruteMillis > 180000)
    AutorisationPageBrute = false; // Autorisation pour 3mn

  delay(2);
}
