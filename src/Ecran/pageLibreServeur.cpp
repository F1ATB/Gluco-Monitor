#include "pageLibreServeur.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <Ecran/Gestion.h>
#include <Ecran/pageCompte.h>
#include <Stock.h>

static Bouton BoutonBas[2] = {{15, 288, 210, 30, "Annuler"},
                              {255, 288, 210, 30, "OK"}};




static Bouton Boutons[12] = {};
static int idxBoutonSelectionne = -1;

void pageLibreServeurSetup()
{
    PageActu = pageLibreServeur;
    String Titre = "Sélectionner la zone du serveur LibreLinkUp";
    CanvaBase->fillScreen(C_grisFonce);
    CanvaBase->setTextColor(RGB565_BLACK);
    CanvaBase->setFont(u8g2_font_helvB14_tf);
    PrintCentre(CanvaBase, Titre, EcranW / 2, 30, 1);
    CanvaBase->fillRoundRect(7, 50, EcranW - 14, EcranH - 90, 8, RGB565_NAVY);
    CanvaBase->drawRoundRect(7, 50, EcranW - 14, EcranH - 90, 8, RGB565_WHITE);
    CanvaBase->setTextColor(RGB565_WHITE);
    int X,Y;
    for (int i=0;i<12;i++)
    {
        
        if (strlen(regions[i]) > 0)
        {
            
           
            if (i%2==1) {// Si c'est une ligne paire, on la décale un peu pour faire un effet de damier
                X=EcranW2+5;
            } else {
                X=15;
                Y = 80 + i * 18;
            }
            Boutons[i].X0 = X;
            Boutons[i].Y0 = Y-20;
            Boutons[i].W = EcranW2 - 20;
            Boutons[i].H = 30;
            Boutons[i].Texte = regions[i];
            if (String(regionsCode[i]) == libreZone) {
                idxBoutonSelectionne = i; // On sélectionne le bouton correspondant à la zone actuellement définie
                 Bouton_Trace(Boutons[i],RGB565_RED); // On trace le bouton sélectionné en rouge
            } else 
            {
                Bouton_Trace(Boutons[i]);  
            }
        }
    }
    

    Bouton_Trace(BoutonBas[0]); // Annuler
    Bouton_Trace(BoutonBas[1]); // OK

    CanvaBase->flush();
}

void handleTouch_LibreServeur(uint16_t touchX, uint16_t touchY)
{
    if (idxBoutonSelectionne != -1) // Si un bouton est déjà sélectionné, on le désélectionne
    {
        Bouton_Trace(Boutons[idxBoutonSelectionne]); // Redessine le bouton pour le "désélectionner"
        
    }
    for (int i=0;i<12;i++)
    {
        
        if (strlen(regions[i]) > 0)
        {
            if (Bouton_Appui(Boutons[i], touchX, touchY))
            {
                // Traitement pour le bouton sélectionné
                idxBoutonSelectionne = i;
                Serial.println("Zone sélectionnée : " + String(regions[i]) + " (code : " + String(regionsCode[i]) + ")" );
            }
        }
    }  
    if (idxBoutonSelectionne != -1) // Si un bouton est  sélectionné,
    {
        Bouton_Trace(Boutons[idxBoutonSelectionne],RGB565_RED); // Redessine le bouton pour le "sélectionner"
        
    } 
    if (Bouton_Appui(BoutonBas[0], touchX, touchY)) // Annuler
    {
        CompteSetup();
    }
    else if (Bouton_Appui(BoutonBas[1], touchX, touchY)) // OK
    {
        if (idxBoutonSelectionne != -1)
        {
            libreZone = regionsCode[idxBoutonSelectionne];
            Serial.println("Zone serveur LibreLinkUp définie sur : " + String(libreZone));
        }
        RecordFichierParametres();
        CompteSetup();
    }
}