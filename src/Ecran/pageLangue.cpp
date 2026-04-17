#include "Ecran/pageLangue.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>
#include "Ecran/Gestion.h"
#include "Ecran/pageConfiguration.h"
#include "Stock.h"
#include "Langues/Langue.h"

static RadioBouton Rboutons[5] = {
    {10, 130, 15, ""},
    {10, 130, 15, ""},
    {10, 130, 15, ""},
    {10, 130, 15, ""},
    {10, 130, 15, ""}};

static RadioBouton UnitBoutons[2] = {
    {10, 250, 15, "mg/dL"},
    {10, 250, 15, "mmol/L"}};

void DrawBoutons_();

void pageLangueSetup()
{
    PageActu = pageLangue;
    CanvaBase->setFont(u8g2_font_helvB18_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    CanvaBase->fillScreen(C_grisFonce);
    PrintCentre(CanvaBase, T("Lang"), EcranW / 2, 30, 1);

    CanvaBase->fillRoundRect(7, 80, EcranW - 14, 100, 8, RGB565_NAVY);
    CanvaBase->drawRoundRect(7, 80, EcranW - 14, 100, 8, RGB565_WHITE);

    CanvaBase->fillRoundRect(7, 205, EcranW - 14, 90, 8, RGB565_NAVY);
    CanvaBase->drawRoundRect(7, 205, EcranW - 14, 90, 8, RGB565_WHITE);
    CanvaBase->setFont(u8g2_font_helvB14_tf);
    PrintCentre(CanvaBase, T("GlucoseUnit"), EcranW / 2, 230, 1);

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
            pageLangueSetup();
            ParaInit();
            return;
        }
    }

    for (int i = 0; i < 2; i++)
    {
        if (RadioBouton_Appui(UnitBoutons[i], touchX, touchY))
        {
            glucoseUnit = (i == 0) ? GLUCOSE_UNIT_MGDL : GLUCOSE_UNIT_MMOLL;
            RecordFichierParametres();
            pageLangueSetup();
            return;
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

    for (int i = 0; i < 2; i++)
    {
        UnitBoutons[i].X0 = EcranW * (i + 1) / 3;
        if ((i == 0 && glucoseUnit == GLUCOSE_UNIT_MGDL) || (i == 1 && glucoseUnit == GLUCOSE_UNIT_MMOLL))
        {
            RadioBouton_Trace(UnitBoutons[i], RGB565_BLUE);
        }
        else
        {
            RadioBouton_Trace(UnitBoutons[i]);
        }
    }
}