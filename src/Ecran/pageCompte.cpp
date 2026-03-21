#include "pageCompte.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <Ecran/Gestion.h>
#include <Ecran/pageClavier.h>
#include <Ecran/pageLibreServeur.h>
#include <Stock.h>

static Bouton Boutons[4] = {
    {15, 288, 110, 30, "Modifier"},
    {15, 288, 110, 30, "Modifier"},
    {15, 288, 110, 30, "Modifier"},
    {165, 288, 200, 30, "Tester"}};
void drawPara(String Titre, String Valeur, int H0, int index);

void CompteSetup()
{
    PageActu = pageCompte;
    CanvaBase->setFont(u8g2_font_helvB18_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    CanvaBase->fillScreen(C_grisFonce);
    PrintCentre(CanvaBase, "Compte LibreLinkUp", EcranW / 2, 30, 1);

    drawPara("Email", libreEmail, 50, 0);
    drawPara("Password", librePass, 110, 1);
    String zoneAffichee = "Non définie";
    for (int i = 0; i < 12; i++)
    {

        if (strlen(regions[i]) > 0)
        {
            if (String(regionsCode[i]) == libreZone)
            {
                zoneAffichee = regions[i];
                break;
            }
        }
    }
    drawPara("Zone serveur", zoneAffichee, 170, 2);

    Bouton_Trace(Boutons[3]); // Tester
   

    CanvaBase->flush();
}
void drawPara(String Titre, String Valeur, int H0, int index)
{
    CanvaBase->setFont(u8g2_font_helvB14_tf);
    CanvaBase->fillRoundRect(7, H0, EcranW - 14, 50, 8, RGB565_NAVY);
    CanvaBase->drawRoundRect(7, H0, EcranW - 14, 50, 8, RGB565_WHITE);

    PrintCentre(CanvaBase,  Titre , EcranW2, H0 + 20, 1);
    CanvaBase->setFont(u8g2_font_10x20_mf );
    PrintGauche(CanvaBase, Valeur, 10, H0 + 40, 1);
    Boutons[index].X0 = EcranW - 122;
    Boutons[index].Y0 = H0 + 10;
    Bouton_Trace(Boutons[index]);
}

void handleTouch_Compte(uint16_t touchX, uint16_t touchY)
{

    if (Bouton_Appui(Boutons[0], touchX, touchY)) // Email
    {
        PageActu = pageClavier_CompteEmail;
        setup_clavier();
    }
    else if (Bouton_Appui(Boutons[1], touchX, touchY)) // Password
    {
        PageActu = pageClavier_ComptePwd;
        setup_clavier();
    }
    else if (Bouton_Appui(Boutons[2], touchX, touchY)) // Zone serveur
    {
        pageLibreServeurSetup();
    }
    else if (Bouton_Appui(Boutons[3], touchX, touchY)) // Tester la connexion
    {
        
        CanvaBase->fillRect(0, 225, EcranW, 62, C_grisFonce);
        if (loginLibreLinkUp())
        {
            CanvaBase->setFont(u8g2_font_helvB18_tf);
            PrintCentre(CanvaBase, "Connexion réussie !", EcranW / 2, 255, 1);
        }
        else
        { //Problème
            CanvaBase->setFont(u8g2_font_10x20_tf);
            if (ServerConnu){
                 PrintCentre(CanvaBase, "Utilisateur inconnu sur le serveur", EcranW / 2, 255, 1);
            } else {
                 PrintCentre(CanvaBase, "Serveur innacessible", EcranW / 2, 255, 1);
            }
            
        }
        CanvaBase->flush();
    }

}
