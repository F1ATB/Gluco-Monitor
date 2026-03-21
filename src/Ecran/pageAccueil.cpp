#include "Ecran/pageAccueil.h"
#include "Config.h"
#include <Arduino.h>
#include "Ecran/Gestion.h"
#include "time.h"



static bool flipCouleurs = false;
static float dtReponse = 0.0;

void Trace_Gauge(Arduino_Canvas *canva);

void AccueilInit()
{
}

void AccueiLoop()
{

    CanvaAccueil->fillScreen(RGB565_BLACK);
    CanvaAccueil->setTextColor(RGB565_WHITE);
    int16_t W2 = EcranW / 2;
    int16_t C = EcranH / 2;
    int16_t R0 = EcranH / 3.5;
    int16_t R1 = EcranH / 2 - 20;
    int16_t Teta0 = -180;
    Trace_Gauge(CanvaAccueil);

    // HEURE
    
    CanvaAccueil->setFont(u8g2_font_fub35_tf);
    if (HeureValide)
        PrintDroite(CanvaAccueil, Hmn, -1, EcranH / 9, 1);
    // Affiche Glycemie
    if (Glycemie == "")
    {
        CanvaAccueil->setFont(u8g2_font_helvB18_tf);
        PrintCentre(CanvaAccueil, "Attente glycémie", W2, C + 25, 1);
    }
    else
    {

        CanvaAccueil->setFont(u8g2_font_inb63_mn);
        PrintCentre(CanvaAccueil, Glycemie, W2, C + 25, 1);

        CanvaAccueil->setFont(u8g2_font_10x20_tf);
        PrintGauche(CanvaAccueil, "mg/dL", W2 + R0, C + 20, 1);

        
        Teta0 = -180 + 18 * GlycemieVal / 40;
        if (Teta0 > 0)
            Teta0 = 0;
        if (Teta0 < -180)
            Teta0 = -180;
        float T = float(Teta0) * 3.14 / 180.0; // Conversion en radians
        R0 = 0.8 * R0;
        CanvaAccueil->fillTriangle(W2 + R1 * cos(T), C + R1 * sin(T), W2 + R0 * cos(T - 0.2), C + R0 * sin(T - 0.2), W2 + R0 * cos(T + 0.2), C + R0 * sin(T + 0.2), RGB565_WHITE); // Aiguille

        // Flèche tendance
        int16_t X0 = EcranW / 6;
        int16_t Y0 = EcranH / 6;
        int16_t x0, y0, x1, y1, x2, y2, x3, y3, x4, y4;
        switch (TrendArrow)
        {
        case 1:
            x0 = -20;
            y0 = 0;
            x1 = 0;
            y1 = 20;
            x2 = 20;
            y2 = 0;
            x3 = -10;
            y3 = -50;
            x4 = +10;
            y4 = -50; // Flèche vers le bas fort
            break;
        case 2:
            x0 = 0;
            y0 = 20;
            x1 = 20;
            y1 = 20;
            x2 = 20;
            y2 = 0;
            x3 = -30;
            y3 = -40;
            x4 = -40;
            y4 = -30; // Flèche vers le bas
            break;
        case 3:
            x0 = 0;
            y0 = 20;
            x1 = 20;
            y1 = 0;
            x2 = 0;
            y2 = -20;
            x3 = -50;
            y3 = -10;
            x4 = -50;
            y4 = +10; // Flèche horizontale
            break;
        case 4:
            x0 = 20;
            y0 = 0;
            x1 = 20;
            y1 = -20;
            x2 = 0;
            y2 = -20;
            x3 = -30;
            y3 = +40;
            x4 = -40;
            y4 = +30; // Flèche vers le haut
            break;
        case 5:
            x0 = 20;
            y0 = 0;
            x1 = 0;
            y1 = -20;
            x2 = -20;
            y2 = 0;
            x3 = -10;
            y3 = 50;
            x4 = +10;
            y4 = 50; // Flèche vers le haut fort
            break;
        }
        CanvaAccueil->fillTriangle(X0 + x0, Y0 + y0, X0 + x1, Y0 + y1, X0 + x2, Y0 + y2, RGB565_WHITE); // Aiguille
        CanvaAccueil->fillTriangle(X0 + x3, Y0 + y3, X0 + x1, Y0 + y1, X0 + x4, Y0 + y4, RGB565_WHITE);
    }
    // Ecrit durée depuis la dernière glycémie
    CanvaAccueil->setFont(u8g2_font_helvB18_tf);
    if (HeureValide && lastGlyUnixTime > 0)
    {

        time_t now;
        time(&now);
        AgeGlycemie = (long)now - lastGlyUnixTime;
        int minutes = AgeGlycemie / 60;
        int secondes = AgeGlycemie % 60;
        char buffer[10];
        sprintf(buffer, "%d:%02d", minutes, secondes);

        if (minutes >= 10)
            CanvaAccueil->setTextColor(RGB565_ORANGE);
        if (minutes >= 15)
            CanvaAccueil->setTextColor(RGB565_RED);
        PrintDroite(CanvaAccueil, String(buffer), EcranW, EcranH / 3, 1);
        
    }
    else
    {
        CanvaAccueil->setTextColor(RGB565_GREY);
        PrintDroite(CanvaAccueil, "Age", EcranW, EcranH / 3, 1);
    }
    CanvaAccueil->setTextColor(RGB565_WHITE);
    // Trace Avancement demande glycémie
    float dT = 0.0;

    if (lastReceptionGlycMillis <= lastDemandeGlycMillis)
    { // ON a appelé pas encore de réponse
        dtReponse = float((millis() - lastDemandeGlycMillis)) * float(EcranW) / float(recurGlycMillis);
    }
    else
    { // L réponse est arrivée, on affiche le temps écoulé depuis la dernière glycémie et le temps restant pour la prochaine
        dtReponse = float((lastReceptionGlycMillis - lastDemandeGlycMillis)) * float(EcranW) / float(recurGlycMillis);
        dT = float((millis() - lastReceptionGlycMillis)) * (float(EcranW) - dtReponse) / float(recurGlycMillis);
        if (dT > (float(EcranW + 10) - dtReponse)) // Pas normal on dépasse la récurrence, on affiche un rectangle rouge
        {
            CanvaAccueil->fillRect(0, EcranH - 10, EcranW, 10, RGB565_RED);
        }
        else
        {
            CanvaAccueil->fillRect(dtReponse, EcranH - 10, dT, 10, C_grisFonce);
        }
    }
    if (dtReponse > float(EcranW + EcranW2)) // Pas normal, on dépasse largement la récurrence, on affiche un rectangle rouge
    {
        dtReponse = float(EcranW + 20);
        
    }
    CanvaAccueil->fillRect(0, EcranH - 10, int(dtReponse), 10, RGB565_MAGENTA);

    // Trace courbe glycemie
    if (pointCountGly > 1)
    {
        int16_t X0 = 24;
        int16_t Y0 = EcranH / 1.9;
        int16_t W = EcranW - X0;
        int16_t H = EcranH * 0.37;
        int16_t EcranH10 = EcranH - 10;
        int16_t x, y, last_x;
        int lastHeure = -1;
        unsigned long Tmin = 0, Tmax = 0;
        Tmin = glucoseHeure[0];
        Tmax = glucoseHeure[pointCountGly - 1];
        last_x = X0;
        float DT = float(W) / (float(Tmax - Tmin));
        CanvaAccueil->setFont(u8g2_font_6x10_tf);
        uint16_t Couleurs[] = {RGB565_BLUE, RGB565_GREEN, RGB565_ORANGE, RGB565_RED};
        uint16_t CouleursFond[] = {C_bleuFonce, C_vertFonce, C_orangeFonce, C_rougeFonce};
        int seuilCoul[] = {0, 70, 180, 300, 400};
        int idxCoul = 0;
        for (int c = 0; c < 4; c++) // Trace fond graphique
        {
            int16_t y2 = EcranH10 - H * seuilCoul[c] / 400;
            y = EcranH10 - H * seuilCoul[c + 1] / 400;
            PrintDroite(CanvaAccueil, String(seuilCoul[c + 1]), X0, y, 1);
            CanvaAccueil->fillRect(X0, y, W, y2 - y, CouleursFond[c]);
        }
        for (int i = 0; i < pointCountGly; i++)
        {
            x = X0 + int(DT * float(glucoseHeure[i] - Tmin));
            y = H * glucoseValues[i] / 400;
            for (int c = 0; c < 4; c++)
            {
                if (glucoseValues[i] > seuilCoul[c])
                    idxCoul = c;
            }
            CanvaAccueil->fillRect(last_x, EcranH10 - y, x - last_x, y, Couleurs[idxCoul]);
            last_x = x;
            int heure = unixToHeure(glucoseHeure[i]);
            if (heure != lastHeure)
            {
                if (heure >= 0 && lastHeure >= 0)
                {
                    CanvaAccueil->drawFastVLine(x, EcranH10, 10, RGB565_WHITE);
                    if (x < W)
                        PrintGauche(CanvaAccueil, String(heure), x, EcranH - 5, 1);
                }
                lastHeure = heure;
            }
        }
        CanvaAccueil->drawFastVLine(X0, EcranH10 - H, H, RGB565_WHITE); // Axe vertical
    }
}

void Trace_Gauge(Arduino_Canvas *canva)
{
    int W2 = EcranW / 2;
    int C = EcranH / 2;
    int R0 = EcranH / 3.5;
    int R1 = EcranH / 2 - 20;
    int Teta0 = -180;
    int Teta1 = Teta0 + 180 * targetLow / 400;
    canva->fillArc(W2, C, R0, R1, Teta0, Teta1, RGB565_BLUE);
    Teta0 = Teta1;
    Teta1 = -180 + 180 * targetHigh / 400;
    canva->fillArc(W2, C, R0, R1, Teta0, Teta1, RGB565_GREEN);
    Teta0 = Teta1;
    Teta1 = -180 + 180 * 300 / 400;
    canva->fillArc(W2, C, R0, R1, Teta0, Teta1, RGB565_ORANGE);
    Teta0 = Teta1;
    Teta1 = 0;
    canva->fillArc(W2, C, R0, R1, Teta0, Teta1, RGB565_RED);
}