#include "Ecran/pageInfos.h"
#include "Ecran/Gestion.h"
#include <U8g2lib.h>
#include "Config.h"
#include "Internet.h"
#include <WiFi.h>

void pageInfosSetup()
{
    PageActu = pageInfos;
    CanvaBase->setFont(u8g2_font_helvB18_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    CanvaBase->fillScreen(C_grisFonce);
    PrintCentre(CanvaBase, "Infos", EcranW / 2, 30, 1);
    CanvaBase->fillRoundRect(7, 50, EcranW - 14, EcranH - 60, 8, RGB565_NAVY);
    CanvaBase->drawRoundRect(7, 50, EcranW - 14, EcranH - 60, 8, RGB565_WHITE);
    CanvaBase->setFont(u8g2_font_10x20_mf);
    PrintGauche(CanvaBase, String("Version logicielle : ") + String(Version), 20, 80, 1);
    PrintGauche(CanvaBase, "Auteur : F1ATB", 20, 110, 1);
    if (WiFi.status() == WL_CONNECTED)
    {
        PrintGauche(CanvaBase, "Adresse IP :", 20, 140, 1);
        PrintGauche(CanvaBase, WiFi.localIP().toString(), EcranW2, 140, 1);
    }
    else
    {
        PrintGauche(CanvaBase, "WiFi non connecté à :" +ssid, 20, 140, 1);
    }
    PrintGauche(CanvaBase, hostname + ".local", EcranW2, 170, 1);
    int Heures = T_On_seconde /3600;
    int Minutes =T_On_seconde%60;
    char value[40];
    sprintf(value, "Gluco-Monitor On depuis : %2dh %02dmn", Heures, Minutes);
    PrintGauche(CanvaBase, String(value) , 20, 200, 1);
    CanvaBase->flush();
}

void handleTouch_Infos(uint16_t touchX, uint16_t touchY)
{
}