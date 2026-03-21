#include "Ecran/pageLangue.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>
#include "Ecran/Gestion.h"
#include "Stock.h"
#include "Langues/Langue.h"

static RadioBouton Rboutons[5] = {
    {10, 130, 15, ""},
    {10, 130, 15, ""},
    {10, 130, 15, ""},
    {10, 130, 15, ""},
    {10, 130, 15, ""}};

void DrawBoutons_();

void pageLangueSetup()
{
    PageActu = pageLangue;
    CanvaBase->setFont(u8g2_font_helvB18_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    CanvaBase->fillScreen(C_grisFonce);
    PrintCentre(CanvaBase, "Langue", EcranW / 2, 30, 1);

    CanvaBase->fillRoundRect(7, 80, EcranW - 14, 100, 8, RGB565_NAVY);
    CanvaBase->drawRoundRect(7, 80, EcranW - 14, 100, 8, RGB565_WHITE);
    DrawBoutons_();
    CanvaBase->flush();
}

void handleTouch_Langue(uint16_t touchX, uint16_t touchY)
{
  
    for (int i = 0; i < 5; i++)
    {
        if (RadioBouton_Appui(Rboutons[i], touchX, touchY))
        {
            LaLangue = i;
            RecordFichierParametres();
            DrawBoutons_();
        }
    }
}
void DrawBoutons_()
{
    for (int i = 0; i < 5; i++)
    {
        Rboutons[i].X0 = EcranW * (i * 2 + 1) / 12;
        Rboutons[i].Texte = LangueSymbole[i];
        if (i == LaLangue)
        {
            RadioBouton_Trace(Rboutons[i], RGB565_BLUE);
        }
        else
        {
            RadioBouton_Trace(Rboutons[i]);
        }
    }
}