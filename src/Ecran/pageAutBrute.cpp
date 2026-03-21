#include "Config.h"
#include <Arduino.h>
#include "Ecran/Gestion.h"



static Bouton Boutons[2] = {
    {30, 0, 180, 50, "Refuser"},
    {270, 0, 180, 50, "Accepter"}};

void AutorisationInit()
{
    
    String Titre = "Autorisation accès données";
    CanvaBase->fillScreen(C_grisFonce);
    CanvaBase->setFont(u8g2_font_helvB18_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    PrintCentre(CanvaBase, Titre, EcranW / 2, 30, 1);
    CanvaBase->fillRoundRect(7, 50, EcranW - 20, EcranH - 60, 8, RGB565_FIREBRICK);
    CanvaBase->drawRoundRect(7, 50, EcranW - 20, EcranH - 60, 8, RGB565_WHITE);
    for (int i = 0; i < 2; i++)
    {   
        Boutons[i].Y0=EcranH2;
        Bouton_Trace(Boutons[i], RGB565_WHITE, CanvaBase);
    }
    CanvaBase->flush();
}
void loop_Autorisation(){
    AutorisationInit();
   if (millis()- TimerAutorisationBruteMillis>180000) PageActu=pageAccueil;
}
void handleTouch_AutBrute(uint16_t touchX, uint16_t touchY)
{

    for (int i = 0; i < 2; i++)
    {
        if (Bouton_Appui(Boutons[i], touchX, touchY, CanvaBase))
        {
            switch (i)
            {
            case 0:
                
                break;
            case 1:
                TimerAutorisationBruteMillis = millis();
                AutorisationPageBrute = true;
                break;
            }
            Serial.println("************???????????????****======================");
            PageActu = pageAccueil;
        }
    }
}