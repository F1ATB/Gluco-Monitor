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
void LireSerial()
{
  int inbyte;
  // Port Serie
  while (Serial.available() > 0)
  {
    inbyte = Serial.read();

    if ((inbyte == 10) || (inbyte == 13))
    {
      DecodeSerial();
    }
    else
    {
      SerialIn += String(char(inbyte));
      Serial.print(char(inbyte));
    }
  }
}
void DecodeSerial()
{
  String sw;
  String valeur = "";
  int p;
  SerialIn.trim();

  p = SerialIn.indexOf(":");
  if (p > 0)
  {
    sw = SerialIn.substring(0, p + 1);
    valeur = SerialIn.substring(p + 1);
    sw.trim();
    valeur.trim();
  }
  else
  {
    sw = SerialIn;
  }
  if (sw.length() > 0)
  {
    Serial.println("Commande série: " + sw + " Valeur: " + valeur);
    if (sw.indexOf("restart") >= 0)
    {
      ESP.restart();
    }
    if (sw.indexOf("ssid:") >= 0)
    {
      ssid = valeur;
      RecordFichierParametres();
    }
    if (sw.indexOf("password:") == 0)
    {
      password = valeur;
      RecordFichierParametres();
    }
    if ((sw.indexOf("H") >= 0 || sw.indexOf("?") >= 0) && p == -1)
    {
      MessageCommandes();
    }
    if (sw.indexOf("partition") >= 0)
    {
      dumpPartitions();
    }
    if (sw.indexOf("lib-email:") >= 0)
    {
      if (valeur.indexOf("@") > 0)
      {
        libreEmail = valeur;
        RecordFichierParametres();
      }
    }
    if (sw.indexOf("lib-password:") >= 0)
    {
      librePass = valeur;
      RecordFichierParametres();
    }
    if (sw.indexOf("lib-zone:") >= 0)
    {
      if (valeur == "no")
      {
        valeur = "";
      }
      bool OK = false;
      for (int i = 0; i < 12; i++)
      {
        if (String(regionsCode[i]) == valeur)
          OK = true;
      }
      if (OK)
      {
        libreZone = valeur;
        RecordFichierParametres();
      }
    }
    if(sw.indexOf("erase")==0){ //Pour effacer la configuration
      RemoveParametres();
    }
  }
  SerialIn = "";
}
// commandes disponibles par port serie ou Telnet
const char *helpSerial = R"====(
**************
Commands for serial port configuration (respect capitalization, punctuation, and end with the 'Enter' key):
Commandes pour configuration par port série  (respect majuscules, ponctuation et terminer par touche 'Enter'):

ssid:...           | Name WiFi network / Nom du réseau WiFi
password:...       | WiFi password / Mot de passe du WiFi
lib-email:...@.... | Email LibreLinkUp account / Email compte LibreLinkUp
lib-password:...   | Password LibreLinkUp / Mot de passe LibreLinkUp
lib-zone:...       | no, eu, eu2, fr, de, us, ca, au, jp, ap, ae
erase              | Erase parametres.json file / efface les paramètres
restart            | To restart ESP32 / Pour redémarrer l'ESP32     
H or / ou  ?       | Help / Pour avoir cette aide
**************
)====";

void MessageCommandes()
{
  String M = helpSerial;
  String Before;
  while (M.indexOf("\n") >= 0)
  {
    SplitS(M, Before, "\n", M);
    Serial.println(Before);
  }
}
void SplitS(String Str, String &Before, String Separ, String &After)
{
  int p = Str.indexOf(Separ);
  Before = Str.substring(0, p);
  After = Str.substring(p + 1);
}
void dumpPartitions()
{
  esp_partition_iterator_t it =
      esp_partition_find(ESP_PARTITION_TYPE_ANY,
                         ESP_PARTITION_SUBTYPE_ANY,
                         NULL);
  while (it != NULL)
  {
    const esp_partition_t *p = esp_partition_get(it);
    Serial.println("Name:" + String(p->label) + "| Type:" + String(p->type) + "| Subtype:" + String(p->subtype) + "| Offset:" + String(p->address) + "| Size:" + String(p->size / 1024) + "kB");
    it = esp_partition_next(it);
  }

  esp_partition_iterator_release(it);
}