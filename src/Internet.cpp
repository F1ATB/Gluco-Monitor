#include "Internet.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <Serie.h>
#include "Heure.h"
#include "Stock.h"
#include "Ecran/pageWifiList.h"
#include "Ecran/Gestion.h"
#include "Ecran/pageMessages.h"
#include "Langues/Langue.h"

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
    String PointsMessage = "", PointsMessage2 = "", PointsMessage3 = "";
    hostname = String(HOSTNAME);
    uint32_t chipId = 0;
    for (int i = 0; i < 17; i = i + 8)
    {
        chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    hostname += String(chipId); // Add chip ID to hostname
    EcranPrintln("Hostname : " + hostname);
    if (ssid.length() == 0)
    {
        QuestionConfiguration(T("SelectWifi"), WifiListSetup);
    }
    CanvaBase->fillRect(0, EcranH2, EcranW, EcranH2, RGB565_BLACK);

    CanvaBase->setFont(u8g2_font_helvB18_tf);
    PrintCentre(CanvaBase, T("InitWifi"), EcranW2, EcranH - 100, 1);
    CanvaBase->flush();

    WiFi.hostname(hostname);
    bool bestWifi = false;

    EcranPrintln(T("InitWifi"));

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
    WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
    bestWifi = Liste_WIFI();
    EcranPrint(T("Version"));
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

    EcranPrintln(T("Nom_WiFi") + ssid);
    EcranPrintln(T("MotDePasse") + password);
    if (ssid.length() > 0)
    {

        CanvaBase->fillRect(0, EcranH2, EcranW, EcranH2, RGB565_BLACK);
        CanvaBase->setFont(u8g2_font_helvB18_tf);
        PrintCentre(CanvaBase, "WiFi", EcranW2, EcranH - 100, 1);
        PrintCentre(CanvaBase, ssid, EcranW2, EcranH - 50, 1);
        CanvaBase->flush();
        EcranPrintln(T("RechercheWiFi") + ssid);
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
        EcranPrintln(T("ConnectedWiFi") + MyIP + T("ou") + hostname + ".local");
        PointsMessage = T("ConnectedWiFi");
        PointsMessage2 = MyIP;
        PointsMessage3 = T("ou") + hostname + ".local";
    }
    else
    {
        EcranPrintln(T("EchecWiFi"));

        PointsMessage = T("EchecWiFi");
    }
    CanvaBase->setFont(u8g2_font_10x20_tf);
    CanvaBase->fillRect(0, EcranH2, EcranW, EcranH2, RGB565_BLACK);
    PrintCentre(CanvaBase, PointsMessage, EcranW2, EcranH - 120, 1);
    PrintCentre(CanvaBase, PointsMessage2, EcranW2, EcranH - 80, 1);
    PrintCentre(CanvaBase, PointsMessage3, EcranW2, EcranH - 40, 1);
    WiFi.scanDelete(); //
    CanvaBase->flush();
    delay(3000);
}

bool Liste_WIFI()
{
    int bestNetworkDb = -1000;
    bool bestFound = false;
    int n = 0;
    // WiFi.disconnect();
    delay(100);
    EcranPrintln(T("ScanStart"));
    // WiFi.scanNetworks will return the number of networks found.
    n = WiFi.scanNetworks();
    EcranPrintln(T("ScanTermine"));
    Liste_AP = "";
    if (n <= 0)
    {
        EcranPrintln(T("PasReseau"));
    }
    else
    {
        EcranPrint(String(n));
        EcranPrintln(T("reseauxTrouves"));
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
