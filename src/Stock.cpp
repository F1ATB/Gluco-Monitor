#include "Stock.h"
#include "Config.h"
#include <Arduino.h>
#include "LittleFS.h"
#include <ArduinoJson.h>
#include "Heure.h"
#include "Langues/Langue.h"
#include "Ecran/Gestion.h"


void ReadFichierParametres();
String SerializeConfiguration();   
void DeserializeConfiguration(String json) ;
void RecordFichierParametres();


void InitStock()
{
  if (!LittleFS.begin(true))
  {
    Serial.println("Erreur de montage de LittleFS");
    return;
  }
  Serial.println("LittleFS monté avec succès");
  delay(500);
}   

// *************************************************************
// Stockage parametres en zone SPIFFS (methode 2026 depuis V17)
// *************************************************************
void RecordFichierParametres() {
  File file = LittleFS.open("/parametres.json", FILE_WRITE);
  file.print(SerializeConfiguration());  //Fichier au format JSON
  file.close();
  Serial.println("Ecriture fichier parametres");
}
void ReadFichierParametres() {
  if (!LittleFS.exists("/parametres.json")) {  //Fichier pas encore crée
    RecordFichierParametres();
  }

  File file = LittleFS.open("/parametres.json", "r");
  Serial.println("Lecture du fichier paramètres");
  String content = file.readString();  // lit tout le fichier
  file.close();
  DeserializeConfiguration(content);
}
void RemoveParametres(){
  LittleFS.remove("/parametres.json");
}

void DeserializeConfiguration(String json) {

 
  Serial.print("Json reçu:");
  Serial.println(json);
  JsonDocument conf;
  DeserializationError error = deserializeJson(conf, json);

  if (error) {
    Serial.print("Erreur de parsing des paramètres: ");
    Serial.println(error.c_str());
    return;
  }
  


  ssid = conf["ssid"].as<String>();
  password = conf["password"].as<String>();
  hostname = conf["hostname"] | hostname;
  MyIP = conf["MyIP"] | MyIP;
  idxFuseau = conf["idxFuseau"].isNull() ? idxFuseau : conf["idxFuseau"];
  rotation = conf["rotation"].isNull() ? rotation : conf["rotation"];
  libreEmail = conf["libreEmail"].as<String>();
  librePass = conf["librePass"].as<String>();
  libreZone = conf["libreZone"].as<String>();
  LuminositeNuit = conf["LuminositeNuit"] | LuminositeNuit;
  LaLangue=conf["LaLangue"] | LaLangue;
 
}

String SerializeConfiguration() {
  JsonDocument conf;
  conf["ssid"] = ssid;
  conf["password"] = password;
  conf["hostname"] = hostname;
  conf["MyIP"] = MyIP;
  conf["idxFuseau"] = idxFuseau;
  conf["rotation"] = rotation;
  conf["libreEmail"] = libreEmail;
  conf["librePass"] = librePass;
  conf["libreZone"] = libreZone;
  conf["LuminositeNuit"] = LuminositeNuit;
  conf["LaLangue"]=LaLangue;
  String Json;
  serializeJson(conf, Json);
  return Json;
}
