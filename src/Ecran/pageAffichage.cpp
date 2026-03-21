#include "Ecran/pageAffichage.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>
#include "Ecran/Gestion.h"
#include "Stock.h"

static RadioBouton Rboutons[5] = {
    {10, 110, 15, "25%"},
    {10, 110, 15, "50%"},
    {10, 110, 15, "100%"},
    {10, 240, 15, "0°"},
    {10, 240, 15, "180°"}};

void DrawBoutons();

void pageAffichageSetup()
{
    PageActu = pageAffichage;
    CanvaBase->setFont(u8g2_font_helvB18_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    CanvaBase->fillScreen(C_grisFonce);
    PrintCentre(CanvaBase, "Affichage", EcranW / 2, 30, 1);
    //============= Luminosité ==========================
    CanvaBase->fillRoundRect(7, 50, EcranW - 14, 110, 8, RGB565_NAVY);
    CanvaBase->drawRoundRect(7, 50, EcranW - 14, 110, 8, RGB565_WHITE);
    CanvaBase->setFont(u8g2_font_helvB14_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    PrintCentre(CanvaBase, "Luminosité écran de nuit", EcranW / 2, 80, 1);

    //============= Rotation ==========================
    CanvaBase->fillRoundRect(7, 180, EcranW - 14, 110, 8, RGB565_NAVY);
    CanvaBase->drawRoundRect(7, 180, EcranW - 14, 110, 8, RGB565_WHITE);
    CanvaBase->setFont(u8g2_font_helvB14_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    PrintCentre(CanvaBase, "Rotation écran", EcranW / 2, 210, 1);
    DrawBoutons();

    CanvaBase->flush();
}

void handleTouch_Affichage(uint16_t touchX, uint16_t touchY)
{ 
    int8_t oldRotation=rotation;
    for (int i = 0; i < 5; i++)
    {
        if (RadioBouton_Appui(Rboutons[i], touchX, touchY))
        {
            switch (i)
            {
            case 0:
                LuminositeNuit = 50;
                break;
            case 1:
                LuminositeNuit = 100;
                break;
            case 2:
                LuminositeNuit = 255;
                break;
            case 3:
                rotation = 1;
                break;
            case 4:
                rotation = 3;
                break;
            }
            
            DrawBoutons();
            if (i < 3)
            {
                ledcWrite(GFX_BL, LuminositeNuit);
                delay(1000);
            }
            if (oldRotation!=rotation){
                CanvaBase->setRotation(rotation);
                pageAffichageSetup();
            }
            RecordFichierParametres();
        }
    }
}
void DrawBoutons()
{
    int16_t Seuil[3] = {50, 100, 255};
    for (int i = 0; i < 3; i++)
    {
        Rboutons[i].X0 = EcranW * (i * 3 + 1) / 9;
        if (LuminositeNuit == Seuil[i])
        {
            RadioBouton_Trace(Rboutons[i], RGB565_BLUE);
        }
        else
        {
            RadioBouton_Trace(Rboutons[i]);
        }
    }
    int16_t rot[2] = {1, 3};
    for (int i = 3; i < 5; i++)
    {
        Rboutons[i].X0 = EcranW * (i * 2 -5) / 4;
        if (rotation == rot[i - 3])
        {
            RadioBouton_Trace(Rboutons[i], RGB565_BLUE);
        }
        else
        {
            RadioBouton_Trace(Rboutons[i]);
        }
    }
}