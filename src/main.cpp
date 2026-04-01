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
#include <esp_task_wdt.h>

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
#include "Ecran/pageFuseauH.h"
#include "Langues/Langue.h"

static unsigned long testWatchdog = 0;

#define WDT_TIMEOUT_SECONDS 600 // Watchdog 10 minutes = 600 secondes

void setup()
{
  Serial.begin(115200);
  SetupEnCours=true;
  LaLangue = LANG_NONDEF;
  //=========== Watchdog initialisation ==========
  esp_task_wdt_deinit();
  esp_task_wdt_config_t wdt_cfg = {
      .timeout_ms = WDT_TIMEOUT_SECONDS * 1000UL,
      .idle_core_mask = (1 << portNUM_PROCESSORS) - 1, // Bitmask of all cores
      .trigger_panic = true,
  };
  esp_task_wdt_init(&wdt_cfg);

  // Abonner la tâche Arduino loop() au watchdog
  esp_task_wdt_add(NULL);
  esp_task_wdt_reset();
  delay(1);
  //======= Stockage =============
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

  //========== Anciens paramètres ==============
  ReadFichierParametres();
  LireSerial();
  // =========== Ecran =========================
  bool LangueNonDefini=false;
  if(LaLangue == LANG_NONDEF){
    LaLangue = LANG_EN; //Par defaut
    LangueNonDefini=true;
  }
  InitEcran();
  LireSerial();
  // ===== Definition de la langue si non encore definie ====
  if (LangueNonDefini)
  {  
    QuestionConfiguration(T("Lang"), pageLangueSetup);
    QuestionConfiguration(T("F_Hor"), pageFuseauSetup);
  }
  // ============ Internet / Wifi et Heure ==============

  Init_Internet();
  CanvaBase->flush();
  esp_task_wdt_reset();
  delay(1);
  Init_Server();
  LireSerial();

  //  ========Modification du programme par le Wifi  - OTA(On The Air) ================

  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin(); // Mandatory

  LireSerial();

  //======== Demande compte LibreLinkUp si non défini =====================
  if (libreEmail.length() < 4)
  {
    QuestionConfiguration(T("SetLibreLinkUp"), CompteSetup);
  }

  esp_task_wdt_reset();
  delay(1);
  Serial.printf("PSRAM: %d\n", psramFound());
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
  SetupEnCours=false;

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

  //== Tests si fonctionnement nominal ============
  if (millis() - lastGlycOkMillis > 1210000) // Si on n'a pas réussi à récupérer une glycémie depuis plus de 20 minutes, on redémarre le module pour tenter de résoudre les problèmes de communication
    AlertePasdeGlycemie();

  if (HeureValide && lastGlyUnixTime > 0)
  {

    time_t now;
    time(&now);
    AgeGlycemie = (long)now - lastGlyUnixTime;
    if (AgeGlycemie > 1800 && millis() > 300000)
      AlertePasdeGlycemie(); // Pas de nouvelle mesure depuis 30mn. Exemple changement de capteur
  }
  if (millis() - testWatchdog > 10000)
  {
    testWatchdog = millis();
    if (WiFi.status() == WL_CONNECTED)
    {
      esp_task_wdt_reset(); // Reset du watchdog
      delay(1);
    }
  }

  //======= Page HTML Brute ============

  if (millis() - TimerAutorisationBruteMillis > 180000)
    AutorisationPageBrute = false; // Autorisation pour 3mn

  delay(2);
}
