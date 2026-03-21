#include "Serie.h"
#include "Config.h"
#include <Arduino.h>
#include "esp_partition.h"
static String SerialIn;

void SplitS(String Str, String &Before, String Separ, String &After);
void DecodeSerial();
void LireSerial();  
void MessageCommandes();
void dumpPartitions();

// PORT SERIE 
void LireSerial() {
  int inbyte;
  //Port Serie
  while (Serial.available() > 0) {
    inbyte = Serial.read();


    if ((inbyte == 10) || (inbyte == 13)) {
      DecodeSerial();
    } else {
      SerialIn += String(char(inbyte));
    }
  }
  
  
}
void DecodeSerial() {
  String sw;
  String valeur = "";
  int p;
  SerialIn.trim();

  p = SerialIn.indexOf(":");
  if (p > 0) {
    sw = SerialIn.substring(0, p + 1);
    valeur = SerialIn.substring(p + 1);
    sw.trim();
    valeur.trim();
  } else {
    sw = SerialIn;
  }

  if (sw.indexOf("restart") >= 0) {
    ESP.restart();
  }
  if (sw.indexOf("ssid:") >= 0) {
    ssid = valeur;          
    RecordFichierParametres();
  }
  if (sw.indexOf("password:") >= 0) {
    password = valeur;
    RecordFichierParametres();
  }
  if ((sw.indexOf("H") >= 0 || sw.indexOf("?") >= 0) && p == -1) {
    MessageCommandes();
  }
  if (sw.indexOf("partition") >= 0) {
    dumpPartitions();
  }
  Serial.println("Commande série: " + sw + " Valeur: " + valeur);
  SerialIn = "";
}
// commandes disponibles par port serie ou Telnet
const char *helpSerial = R"====(
**************
commandes pour configuration par port série ou Telnet (respect majuscules, ponctuation et terminer par touche 'Enter'):

ssid:xxx     | Pour définir le nom xxx du Wifi à utiliser
password:yyy | Pour définir le mot de passe yyy du Wifi
restart      | Pour redémarrer l'ESP32 sans appui sur le bouton EN     
H ou ?       | pour avoir cette aide
**************
)====";


void MessageCommandes() {
  String M = helpSerial;
  String Before;
  while (M.indexOf("\n") >= 0) {
    SplitS(M, Before, "\n", M);
    Serial.println(Before);
  }
}
void SplitS(String Str, String &Before, String Separ, String &After) {
  int p = Str.indexOf(Separ);
  Before = Str.substring(0, p);
  After = Str.substring(p + 1);
}
void dumpPartitions() {
  esp_partition_iterator_t it =
    esp_partition_find(ESP_PARTITION_TYPE_ANY,
                       ESP_PARTITION_SUBTYPE_ANY,
                       NULL);
  while (it != NULL) {
    const esp_partition_t* p = esp_partition_get(it);
    Serial.println("Name:" + String(p->label) + "| Type:" + String(p->type) + "| Subtype:" + String(p->subtype) + "| Offset:" + String(p->address) + "| Size:" + String(p->size / 1024)+ "kB");
    it = esp_partition_next(it);
  }

  esp_partition_iterator_release(it);
}