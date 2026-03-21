#include "Internet.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <Serie.h>
#include "Heure.h"
#include "Stock.h"

// WIFI

int16_t ComSurv = 6; // Timeout sans Wifi par pas de 30s

String Liste_AP = "";
static uint8_t bestBSSID[6]; // Meilleur en dBm adresse MAC

String Format_WiFi(int num, const String &nom, int niveau, const String &MAC, int channel);
bool Liste_WIFI();

// ***********************************
// INIT INTERNET
// Configure adresses WIFI
// ***************************
void Init_Internet()
{
    String PointsMessage = "",PointsMessage2 = "";

    CanvaBase->fillRect(0, EcranH2, EcranW, EcranH2, RGB565_BLACK);
    CanvaBase->setFont(u8g2_font_helvB18_tf);
    PrintCentre(CanvaBase, "Lancement du Wifi", EcranW2, EcranH - 100, 1);
    CanvaBase->flush();
    hostname = String(HOSTNAME);
    uint32_t chipId = 0;
    for (int i = 0; i < 17; i = i + 8)
    {
        chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    hostname += String(chipId); // Add chip ID to hostname
    EcranPrintln("Hostname : " + hostname);
    WiFi.hostname(hostname);
    bool bestWifi = false;

    EcranPrintln("Lancement du Wifi");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
    WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
    bestWifi = Liste_WIFI();
    EcranPrint("Version : ");
    EcranPrintln(Version);
    LireSerial();

    // Check WiFi connection
    // ... check mode
    if (WiFi.getMode() != WIFI_STA)
    {
        WiFi.mode(WIFI_STA);
        delay(10);
    }

    DefFuseauHoraire();

    // WIFI

    EcranPrintln("ssid:" + ssid);
    EcranPrintln("password:" + password);
    if (ssid.length() > 0)
    {

        CanvaBase->fillRect(0, EcranH2, EcranW, EcranH2, RGB565_BLACK);
        CanvaBase->setFont(u8g2_font_helvB18_tf);
        PrintCentre(CanvaBase, "Wifi Begin", EcranW2, EcranH - 100, 1);
        PrintCentre(CanvaBase, ssid, EcranW2, EcranH - 50, 1);
        CanvaBase->flush();
        EcranPrintln("Wifi Begin : " + ssid);
        if (bestWifi)
        {
            WiFi.begin(ssid.c_str(), password.c_str(), 0, bestBSSID); // Connexion forcée au BSSID choisi
        }
        else
        {
            WiFi.begin(ssid.c_str(), password.c_str());
        }

        while (WiFi.status() != WL_CONNECTED && (millis() < 40000))
        { // Attente connexion au Wifi
            PointsMessage += ".";
            PrintCentre(CanvaBase, PointsMessage, EcranW2, EcranH - 10, 1);
            EcranPrint(".");
            EcranPrint(String(WiFi.status()));
            CanvaBase->flush();
            for (int i = 0; i < 10; i++)
            {
                delay(30);
                LireSerial();
            }
        }
        EcranPrintln("");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        MyIP = WiFi.localIP().toString();
        EcranPrintln("Connecté par WiFi, addresse IP : " + MyIP + " or " + hostname + ".local");
        PointsMessage = "Connecté par WiFi, addresse IP : " + MyIP ;
        PointsMessage2 =  " or " + hostname + ".local";
    }
    else
    {
        EcranPrintln("Echec de connexion au WiFi");
        
        PointsMessage = "Echec de connexion au WiFi";
    }
    CanvaBase->setFont(u8g2_font_10x20_tf);
    CanvaBase->fillRect(0, EcranH2, EcranW, EcranH2, RGB565_BLACK);
    PrintCentre(CanvaBase, PointsMessage , EcranW2, EcranH - 100, 1);
    PrintCentre(CanvaBase, PointsMessage2 , EcranW2, EcranH - 50, 1);
    CanvaBase->flush();
    WiFi.scanDelete(); //
    delay(2000);
}

bool Liste_WIFI()
{
    int bestNetworkDb = -1000;
    bool bestFound = false;
    int n = 0;
    // WiFi.disconnect();
    delay(100);
    EcranPrintln("Scan start");
    // WiFi.scanNetworks will return the number of networks found.
    n = WiFi.scanNetworks();
    EcranPrintln("Scan done");
    Liste_AP = "";
    if (n <= 0)
    {
        EcranPrintln("Pas de réseau Wifi trouvé");
    }
    else
    {
        EcranPrint(String(n));
        EcranPrintln(" réseaux trouvés");
        EcranPrintln("|Nr|          SSID              |   RSSI  |       MAC       | Channel |");
        for (int i = 0; i < n; ++i)
        {
            // Print SSID and RSSI for each network found
            EcranPrintln(Format_WiFi(i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.BSSIDstr(i), WiFi.channel(i)));
            Liste_AP += WiFi.SSID(i).c_str() + RS + String(WiFi.RSSI(i)) + RS + WiFi.BSSIDstr(i).c_str() + RS + String(WiFi.channel(i)) + GS;
            if (WiFi.SSID(i) == ssid)
            {
                if (WiFi.RSSI(i) > bestNetworkDb)
                {
                    bestNetworkDb = WiFi.RSSI(i);
                    memcpy(bestBSSID, WiFi.BSSID(i), 6);
                    bestFound = true;
                }
            }
        }
    }
    WiFi.scanDelete();
    return bestFound;
}
String Format_WiFi(int num, const String &nom, int niveau, const String &MAC, int channel)
{
    char value[100];
    sprintf(value, "|%2d|%-28s|%4d dBm |%-16s|%9d|", num, nom.c_str(), niveau, MAC.c_str(), channel);
    return String(value);
}
