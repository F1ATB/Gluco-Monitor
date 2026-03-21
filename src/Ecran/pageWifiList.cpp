#include "pageWiFiList.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <Ecran/Gestion.h>
#include <Ecran/pageClavier.h>
#include <Stock.h>

String Format2_WiFi(int num, const String &nom, int niveau);        // 
static int nbWifi = 0, iSelected = -1; // Nombre de réseaux trouvés et index du réseau sélectionné
static String ssidList[6]; // SSID du réseau sélectionné

 
static Bouton Boutons[6] = {};

void WifiListSetup()
{
    PageActu = pageWifiList;
    CanvaBase->setFont(u8g2_font_helvB18_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    CanvaBase->fillScreen(C_grisFonce);
    PrintCentre(CanvaBase, "Liste des réseaux Wifi", EcranW / 2, 30, 1);
    PrintCentre(CanvaBase, "Scan start", EcranW / 2, 100, 1);
    CanvaBase->flush();
    // WiFi.scanNetworks will return the number of networks found.
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
    WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
    nbWifi = WiFi.scanNetworks();
    PrintCentre(CanvaBase, "Scan done", EcranW / 2, 150, 1);
    CanvaBase->flush();
    delay(500);
    CanvaBase->fillRect(0, 40, EcranW, EcranH - 48, C_grisFonce);
    if (nbWifi >= 6)
        nbWifi = 6; // Limite à 6 réseaux pour éviter les débordements
    if (nbWifi <= 0)
    {
        PrintCentre(CanvaBase, "Pas de réseau Wifi trouvé", EcranW / 2, 80, 1);
    }
    else
    {
        CanvaBase->fillRoundRect(7, 54, EcranW - 14, nbWifi * 30 + 40, 8, RGB565_NAVY);
        CanvaBase->drawRoundRect(7, 54, EcranW - 14, nbWifi * 30 + 40, 8, RGB565_WHITE);
        CanvaBase->setFont(u8g2_font_10x20_mf );
        PrintCentre(CanvaBase, "|Nr|            SSID             |  RSSI |", EcranW2, 74, 1);
        for (int i = 0; i < nbWifi; i++)
        {
            ssidList[i] = WiFi.SSID(i);
            int rssi = WiFi.RSSI(i);

            Boutons[i].X0 =15;
            Boutons[i].Y0 =  90 + i * 30; // Position Y pour chaque réseau
            Boutons[i].W = EcranW - 30;
            Boutons[i].H = 26;
            Boutons[i].Texte = Format2_WiFi(i + 1, ssidList[i], rssi);           
            Bouton_Trace(Boutons[i]);    
        }
    }

    WiFi.scanDelete();

   
    CanvaBase->flush();
}

String Format2_WiFi(int num, const String &nom, int niveau)
{
    char value[100];
    sprintf(value, "|%2d|%-28s|%3d dBm|", num, nom.c_str(), niveau);
    return String(value);
}

void handleTouch_WifiList(uint16_t touchX, uint16_t touchY)
{  
    for (int i = 0; i < nbWifi; ++i)
    {
        if (Bouton_Appui(Boutons[i], touchX, touchY)) {
            
            Bouton_Trace(Boutons[i],RGB565_RED); 
            delay(300) ;
            ssid = String(ssidList[i]);
            RecordFichierParametres();
            PageActu = pageClavier_WifiPwd;
            setup_clavier(); 
            return;       
        } else {
            Bouton_Trace(Boutons[i]);
        }
        
    }
    
     
}
