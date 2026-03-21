#include "Ecran/pageFuseauH.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>
#include "Ecran/Gestion.h"
#include "Stock.h"

static int16_t idxVisu = 0, NbBoutons = 0;
static RadioBouton Rboutons[10];


void ImpressionZones();

void pageFuseauSetup()
{
    PageActu = pageFuseauH;
    CanvaBase->setFont(u8g2_font_helvB18_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    CanvaBase->fillScreen(C_grisFonce);
    PrintCentre(CanvaBase, "Fuseau Horaire", EcranW / 2, 30, 1);
    //============= FUSEAU ==========================
    CanvaBase->fillRoundRect(7, 50, EcranW - 14, 260, 8, RGB565_NAVY);
    CanvaBase->drawRoundRect(7, 50, EcranW - 14, 260, 8, RGB565_WHITE);
    idxVisu = idxFuseau - 2;
    ImpressionZones();
}

void handleTouch_Fuseau(uint16_t touchX, uint16_t touchY, int16_t DeltaTouchY)
{
    int16_t delta = DeltaTouchY / 2;

    if (delta != 0)
    {
        idxVisu = idxVisu - delta;
        ImpressionZones();
    }
    else
    {
        for (int i = 0; i < NbBoutons; i++)
        {
            if (RadioBouton_Appui(Rboutons[i], touchX, touchY))
            {
                idxFuseau = i + idxVisu;
                ImpressionZones();
                RecordFichierParametres();
                i = 100;
            }
        }
    }
}
void ImpressionZones()
{
    if (idxVisu < 0)
        idxVisu = 0;
    if (idxVisu > NB_TZ - 3)
        idxVisu = NB_TZ - 3;
    CanvaBase->fillRoundRect(7, 50, EcranW - 14, 260, 8, RGB565_NAVY);
    CanvaBase->drawRoundRect(7, 50, EcranW - 14, 260, 8, RGB565_WHITE);
    int Y = 60;
    NbBoutons = 0;
    for (int i = idxVisu; i < NB_TZ; i++)
    {

        Rboutons[NbBoutons].X0 = 15;
        Rboutons[NbBoutons].Y0 = Y;
        Rboutons[NbBoutons].R = 15;
        Rboutons[NbBoutons].Texte = nomTZ[i];
        if (i == idxFuseau)
        {
            RadioBouton_Trace(Rboutons[NbBoutons], RGB565_BLUE);
        }
        else
        {
            RadioBouton_Trace(Rboutons[NbBoutons]);
        }
        NbBoutons++;
        Y = Y + 40;
        if (Y > 280)
            break;
    }
    CanvaBase->flush();
}