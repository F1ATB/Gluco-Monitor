#include "Ecran/pageInfos.h"
#include "Ecran/Gestion.h"
#include <U8g2lib.h>
#include "Config.h"
#include "Internet.h"
#include <WiFi.h>
#include "Langues/Langue.h"


void pageInfosSetup()
{
    PageActu = pageInfos;
    CanvaBase->setFont(u8g2_font_helvB18_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    CanvaBase->fillScreen(C_grisFonce);
    PrintCentre(CanvaBase, T("Infos"), EcranW / 2, 30, 1);
    CanvaBase->fillRoundRect(7, 50, EcranW - 14, EcranH - 60, 8, RGB565_NAVY);
    CanvaBase->drawRoundRect(7, 50, EcranW - 14, EcranH - 60, 8, RGB565_WHITE);
    CanvaBase->setFont(u8g2_font_10x20_mf);
    PrintGauche(CanvaBase, T("VersionSoft") + String(Version) +"    " + String(BuildDate), 20, 80, 1);
    PrintGauche(CanvaBase, T("Auteur") + " : F1ATB", 20, 110, 1);
    if (WiFi.status() == WL_CONNECTED)
    {
        PrintGauche(CanvaBase, T("AdrIP"), 20, 140, 1);
        PrintGauche(CanvaBase, WiFi.localIP().toString(), EcranW2 -10, 140, 1);
    }
    else
    {
        PrintGauche(CanvaBase, T("NoWiFi") +ssid, 20, 140, 1);
    }
    PrintGauche(CanvaBase, hostname + ".local", EcranW2-10, 170, 1);
    uint64_t Heures = T_On_seconde /3600;
    uint64_t Minutes =(T_On_seconde % 3600) / 60;
    char value[40];
    String Gon=T("GlucoOn");
    sprintf(value, "%s%lluh %02llumn", Gon.c_str(),Heures, Minutes);
    PrintGauche(CanvaBase, String(value) , 20, 200, 1);
    PrintCentre(CanvaBase,"https://F1ATB.fr",EcranW2,EcranH-40,1);
    CanvaBase->flush();
}

void handleTouch_Infos(uint16_t touchX, uint16_t touchY)
{
}