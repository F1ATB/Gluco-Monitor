#include "Ecran/pageConfiguration.h"
#include "Config.h"
#include <Arduino.h>
#include "Ecran/Gestion.h"
#include "Ecran/pageWiFiList.h"
#include "Ecran/pageClavier.h"
#include "Ecran/pageCompte.h"
#include "Ecran/pageInfos.h"
#include "Ecran/pageAffichage.h"
#include "Ecran/pageFuseauH.h"
#include "Ecran/pageLangue.h"

static Bouton Boutons[7] = {
    {25, 50, 110, 30, "WiFi"},
    {185, 50, 110, 30, "Compte"},
    {15, 100, 110, 30, "Affichage"},
    {185, 100, 110, 30, "Informations"},
    {15, 100, 110, 30, "Langue"},
    {185, 100, 110, 30, "Fuseau Horaire"},
    {300, 200, 150, 30, "Restart"}};

void ParaInit()
{
  String Titre = "Paramètres de Configuration";
  CanvaConfig->fillScreen(C_grisFonce);
  CanvaConfig->setFont(u8g2_font_helvB18_tf);
  CanvaConfig->setTextColor(RGB565_WHITE);
  PrintCentre(CanvaConfig, Titre, EcranW / 2, 30, 1);
  CanvaConfig->fillRoundRect(7, 50, EcranW - 14, EcranH - 60, 8, RGB565_NAVY);
  CanvaConfig->drawRoundRect(7, 50, EcranW - 14, EcranH - 60, 8, RGB565_WHITE);

  int X, Y;
  for (int i = 0; i < 7; i++)
  {

    if (i % 2 == 1)
    { // Si c'est une ligne paire, on la décale un peu pour faire un effet de damier
      X = EcranW2 + 20;
    }
    else
    {
      X = 20;
      Y = 80 + i * 25;
    }
    if (i == 6)
    {
      X = EcranW - 220;
      Y = EcranH - 70;
    }
    Boutons[i].X0 = X;
    Boutons[i].Y0 = Y;
    Boutons[i].W = 200;
    Boutons[i].H = 40;

    Bouton_Trace(Boutons[i], RGB565_WHITE, CanvaConfig);
  }
}
void pageConfigurationChoix(uint16_t touchX, uint16_t touchY, int16_t DeltaTouchX, int16_t DeltaTouchY)
{
  for (int i = 0; i < 7; i++)
  {

    if (Bouton_Appui(Boutons[i], touchX, touchY, CanvaConfig))
    {
      {
        switch (i)
        {
        case 0:
          WifiListSetup();
          break;
        case 1:
          CompteSetup();
          break;
        case 2:
          pageAffichageSetup();
          break;
        case 3:
          pageInfosSetup();
          break;
        case 4:
          pageLangueSetup();
          break;
        case 5:
          pageFuseauSetup();
          break;
        case 6:
          ESP.restart();
          break;

        default:
          break;
        }
        delay(100);
        return;
      }
    }
  }
}