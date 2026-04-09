#include "Server.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

#include "HTML/pageMain.h"
#include "HTML/pageBrute.h"
#include "HTML/pageAutorisationBrute.h"
#include "HTML/pageOTA.h"
#include "HTML/JS_Commun.js.h"
#include "HTML/JS_Main.js.h"
#include "Ecran/pageAutBrute.h"
#include "Langues/Langue.h"
#include "Langues/en.h"
#include "Langues/fr.h"
#include "Langues/de.h"
#include "Langues/it.h"
#include "Langues/es.h"

// Serveur Web
static AsyncWebServer server(80);

uint8_t MonBuffer[4 + MAX_POINTS * 6]; // Pour les tableaux de glycemie
// Prototypes
void notFound(AsyncWebServerRequest *request);
void handleDoUpdate(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);

void Init_Server()
{

  // Main Page
  //*********
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", MainHtml); });
  server.on("/Brute", HTTP_GET, [](AsyncWebServerRequest *request)
            {
                  if (AutorisationPageBrute)
                  {                      
                      request->send(200, "text/html", BruteHtml);
                  }
                  else
                  {
                      PageActu = pageAutBrute;
                      request->send(200, "text/html", AutBruteHtml);
                  }
                  TimerAutorisationBruteMillis = millis(); });
  server.on("/OTA", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
                if (AutorisationPageBrute)
                  {
                      request->send(200, "text/html", OTAupdateHtml);
                  }
                  else
                  {
                      PageActu = pageAutBrute;
                      request->send(200, "text/html", AutBruteHtml);
                  }
                  TimerAutorisationBruteMillis = millis(); });
  server.on(
      "/update", HTTP_POST,
      [](AsyncWebServerRequest *request) {},
      [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,
         size_t len, bool final)
      {
        handleDoUpdate(request, filename, index, data, len, final);
      });

  server.on("/JS_Commun", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/javascript", String(JS_Commun) +"\nconst Version = '" + String(Version) +"';"); });
  server.on("/JS_Traduction", HTTP_GET, [](AsyncWebServerRequest *request)
            {  String file;
              switch(LaLangue)
                  {
                      case LANG_EN:
                          file=String(LangEN);
                          break;
                      case LANG_FR:
                          file=String(LangFR);
                          break;
                      case LANG_DE:
                          file=String(LangDE);
                          break;
                      case LANG_ES:
                          file=String(LangES);
                          break;
                      case LANG_IT:
                          file=String(LangIT);
                          break;
                  }
              
              request->send(200, "text/javascript",  "Traduction =" + file +";"); });
  server.on("/JS_Main", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/javascript", JS_Main); });
  server.on("/LoginJSON", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", LoginJSON); });
  server.on("/ConnectionJSON", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", ConnectionJSON); });
  server.on("/GraphJSON", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", GraphJSON); });
  server.on("/ajaxGlycemie", HTTP_GET, [](AsyncWebServerRequest *request)
            {JsonDocument doc;
                doc["GlycemieVal"] = GlycemieVal;
                doc["TrendArrow"] = TrendArrow;
                doc["lastGlyUnixTime"] = lastGlyUnixTime;
                doc["targetLow"] = targetLow;
                doc["targetHigh"] = targetHigh;
                doc["sensorType"] = (int)sensorType;
                String Json;
                serializeJson(doc, Json);
                request->send(200, "application/json", Json); });
  server.on("/dataGly", HTTP_GET, [](AsyncWebServerRequest *request)
            {
                int16_t tailles[2]; //Pour Javascript derrier, il faut un multiple de 4 octets
                tailles[0]=pointCountGly;
                memcpy(&MonBuffer[0], tailles,  2*sizeof(int16_t)); //En premier la taille des tableaux
                memcpy(&MonBuffer[4], glucoseHeure, pointCountGly * sizeof(uint32_t));
                memcpy(&MonBuffer[4+pointCountGly * sizeof(uint32_t)], glucoseValues, pointCountGly * sizeof(int16_t));

                
                size_t size =
                        2*sizeof(int16_t) +
                        pointCountGly * sizeof(uint32_t) +
                        pointCountGly * sizeof(int16_t);

                    AsyncWebServerResponse *response =
                        request->beginResponse(
                            200,
                            "application/octet-stream",
                            (uint8_t*)&MonBuffer,
                            size
                        );

                    request->send(response); });
  server.on("/Restart", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", RestartHtml); 
                 delay(1000);
                 ESP.restart(); });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "image/svg+xml", Favicon); });
  server.on("/favicon192.ico", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "image/svg+xml", Favicon192); });
  server.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", Manifest); });
  server.onNotFound(notFound);

  server.begin();
  EcranPrintln(T("Serveur80"));
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void handleDoUpdate(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{ // Mise à jour par OTA
  char progress[30];
  if (!index)
  {
    EcranPrintln(T("Update"));
    // content_len = request->contentLength();
    if (!Update.begin(UPDATE_SIZE_UNKNOWN))
    {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) != len)
  {
    Update.printError(Serial);
    sprintf(progress, "Progress: %d%%\n", (Update.progress() * 100) / Update.size());
    EcranPrintln(String(progress));
  }

  if (final)
  {
    AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
    response->addHeader("Refresh", "20");
    response->addHeader("Location", "/");
    request->send(response);
    if (!Update.end(true))
    {
      Update.printError(Serial);
    }
    else
    {
      EcranPrintln(T("UpdateComplete"));
      Serial.flush();
      ESP.restart();
    }
  }
}