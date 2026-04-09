#include "pageCompte.h"
#include "Config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <Ecran/Gestion.h>
#include <Ecran/pageClavier.h>
#include <Ecran/pageLibreServeur.h>
#include <Stock.h>
#include "Langues/Langue.h"
#include "Libreview.h"
#include "Dexcom.h"

static Bouton Boutons[5] = {
    {15, 40, 150, 30, "Sensor Type"},
    {15, 288, 110, 30, "Modifier"},
    {15, 288, 110, 30, "Modifier"},
    {15, 288, 110, 30, "Modifier"},
    {165, 288, 200, 30, "Tester"}};
void drawPara(String Titre, String Valeur, int H0, int index);

void CompteSetup()
{
    PageActu = pageCompte;
    Boutons[1].Texte=T("Modifier");
    Boutons[2].Texte=T("Modifier");
    Boutons[3].Texte=T("Modifier");
    Boutons[4].Texte=T("Tester");
    CanvaBase->setFont(u8g2_font_helvB18_tf);
    CanvaBase->setTextColor(RGB565_WHITE);
    CanvaBase->fillScreen(C_grisFonce);
    
    // Sensor type selector at top - two buttons side by side
    CanvaBase->setFont(u8g2_font_helvB14_tf);
    PrintCentre(CanvaBase, T("SensorType"), EcranW / 2, 25, 1);
    
    // FreeStyle button (left)
    int buttonWidth = (EcranW - 21) / 2;  // 21 = 7 + 7 + 7 (margins and spacing)
    uint16_t libreColor = (sensorType == SENSOR_LIBRE) ? RGB565_GREEN : RGB565_NAVY;
    CanvaBase->fillRoundRect(7, 35, buttonWidth, 35, 8, libreColor);
    CanvaBase->drawRoundRect(7, 35, buttonWidth, 35, 8, RGB565_WHITE);
    CanvaBase->setFont(u8g2_font_helvB14_tf);
    PrintCentre(CanvaBase, "FreeStyle", 7 + buttonWidth / 2, 55, 1);
    
    // Dexcom button (right)
    uint16_t dexcomColor = (sensorType == SENSOR_DEXCOM) ? RGB565_GREEN : RGB565_NAVY;
    CanvaBase->fillRoundRect(14 + buttonWidth, 35, buttonWidth, 35, 8, dexcomColor);
    CanvaBase->drawRoundRect(14 + buttonWidth, 35, buttonWidth, 35, 8, RGB565_WHITE);
    PrintCentre(CanvaBase, "Dexcom", 14 + buttonWidth + buttonWidth / 2, 55, 1);
    
    // Display appropriate account info based on sensor type
    if (sensorType == SENSOR_LIBRE)
    {
        PrintCentre(CanvaBase, T("Compte") + " LibreLinkUp", EcranW / 2, 95, 1);
        drawPara("Email", libreEmail, 110, 1);
        drawPara(T("Password"), librePass, 170, 2);
        String zoneAffichee = T("Undefined");
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
        drawPara(T("ServerZone"), zoneAffichee, 230, 3);
    }
    else // SENSOR_DEXCOM
    {
        PrintCentre(CanvaBase, T("Compte") + " Dexcom Share", EcranW / 2, 95, 1);
        drawPara(T("Username"), dexcomUsername, 110, 1);
        drawPara(T("Password"), dexcomPassword, 170, 2);
        
        // Region selection with radio buttons (centered)
        CanvaBase->setFont(u8g2_font_helvB14_tf);
        CanvaBase->fillRoundRect(7, 230, EcranW - 14, 50, 8, RGB565_NAVY);
        CanvaBase->drawRoundRect(7, 230, EcranW - 14, 50, 8, RGB565_WHITE);
        PrintCentre(CanvaBase, T("Region"), EcranW2, 250, 1);
        
        // Calculate positions to center the entire group
        // Approximate text widths: "Non-US" ~60px, "US" ~25px, "JP" ~25px
        int radioY = 265;
        int radioR = 8;
        int textOffset = 15; // Space between radio and text
        
        // Total width calculation: radio + text + spacing + radio + text + spacing + radio + text
        // Non-US: 8 + 15 + 60 = 83
        // US: 8 + 15 + 25 = 48
        // JP: 8 + 15 + 25 = 48
        // Spacing between groups: 20px each
        int totalWidth = 83 + 20 + 48 + 20 + 48; // ~219px
        int startX = EcranW2 - (totalWidth / 2);
        
        int radioX1 = startX + 8; // Non-US button
        int radioX2 = radioX1 + 83 + 20 + 8; // US button
        int radioX3 = radioX2 + 48 + 20 + 8; // JP button
        
        // Non-US button
        CanvaBase->fillCircle(radioX1, radioY, radioR, RGB565_WHITE);
        if (dexcomRegion != "US" && dexcomRegion != "JP") {
            CanvaBase->fillCircle(radioX1, radioY, radioR - 3, RGB565_GREEN);
        } else {
            CanvaBase->fillCircle(radioX1, radioY, radioR - 3, RGB565_NAVY);
        }
        CanvaBase->setFont(u8g2_font_helvB14_tf);
        CanvaBase->setCursor(radioX1 + textOffset, radioY + 5);
        CanvaBase->print("Non-US");
        
        // US button
        CanvaBase->fillCircle(radioX2, radioY, radioR, RGB565_WHITE);
        if (dexcomRegion == "US") {
            CanvaBase->fillCircle(radioX2, radioY, radioR - 3, RGB565_GREEN);
        } else {
            CanvaBase->fillCircle(radioX2, radioY, radioR - 3, RGB565_NAVY);
        }
        CanvaBase->setCursor(radioX2 + textOffset, radioY + 5);
        CanvaBase->print("US");
        
        // JP button
        CanvaBase->fillCircle(radioX3, radioY, radioR, RGB565_WHITE);
        if (dexcomRegion == "JP") {
            CanvaBase->fillCircle(radioX3, radioY, radioR - 3, RGB565_GREEN);
        } else {
            CanvaBase->fillCircle(radioX3, radioY, radioR - 3, RGB565_NAVY);
        }
        CanvaBase->setCursor(radioX3 + textOffset, radioY + 5);
        CanvaBase->print("JP");
    }

    Bouton_Trace(Boutons[4]); // Tester
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
    // Sensor type selection - two buttons
    if (touchY >= 35 && touchY <= 70)
    {
        int buttonWidth = (EcranW - 21) / 2;
        
        // FreeStyle Libre button (left)
        if (touchX >= 7 && touchX <= 7 + buttonWidth)
        {
            if (sensorType != SENSOR_LIBRE) {
                sensorType = SENSOR_LIBRE;
                RecordFichierParametres();
                lastDemandeGlycMillis = 0;
                CompteSetup();
            }
            return;
        }
        
        // Dexcom button (right)
        if (touchX >= 14 + buttonWidth && touchX <= EcranW - 7)
        {
            if (sensorType != SENSOR_DEXCOM) {
                sensorType = SENSOR_DEXCOM;
                RecordFichierParametres();
                lastDemandeGlycMillis = 0;
                CompteSetup();
            }
            return;
        }
    }

    if (Bouton_Appui(Boutons[1], touchX, touchY)) // Email/Username
    {
        if (sensorType == SENSOR_LIBRE)
        {
            PageActu = pageClavier_CompteEmail;
        }
        else
        {
            PageActu = pageClavier_DexcomUsername;
        }
        setup_clavier();
    }
    else if (Bouton_Appui(Boutons[2], touchX, touchY)) // Password
    {
        if (sensorType == SENSOR_LIBRE)
        {
            PageActu = pageClavier_ComptePwd;
        }
        else
        {
            PageActu = pageClavier_DexcomPwd;
        }
        setup_clavier();
    }
    else if (sensorType == SENSOR_DEXCOM && touchY >= 230 && touchY <= 280)
    {
        // Handle Dexcom region radio buttons (check this BEFORE Bouton_Appui)
        // Calculate positions to match display (same calculation as in CompteLoop)
        int totalWidth = 83 + 20 + 48 + 20 + 48; // ~219px
        int startX = EcranW2 - (totalWidth / 2);
        
        int radioX1 = startX + 8; // Non-US button
        int radioX2 = radioX1 + 83 + 20 + 8; // US button
        int radioX3 = radioX2 + 48 + 20 + 8; // JP button
        int touchRadius = 50; // Touch area
        
        // Check if Non-US radio button was clicked
        if (touchX >= radioX1 - touchRadius && touchX <= radioX1 + touchRadius) {
            if (dexcomRegion != "US" && dexcomRegion != "JP") {
                // Already Non-US, no change needed
            } else {
                dexcomRegion = "Non-US";
                RecordFichierParametres();
                CompteSetup();
            }
            return;
        }
        // Check if US radio button was clicked
        else if (touchX >= radioX2 - touchRadius && touchX <= radioX2 + touchRadius) {
            if (dexcomRegion != "US") {
                dexcomRegion = "US";
                RecordFichierParametres();
                CompteSetup();
            }
            return;
        }
        // Check if JP radio button was clicked
        else if (touchX >= radioX3 - touchRadius && touchX <= radioX3 + touchRadius) {
            if (dexcomRegion != "JP") {
                dexcomRegion = "JP";
                RecordFichierParametres();
                CompteSetup();
            }
            return;
        }
    }
    else if (Bouton_Appui(Boutons[3], touchX, touchY)) // Zone serveur (LibreView only)
    {
        if (sensorType == SENSOR_LIBRE)
        {
            pageLibreServeurSetup();
        }
    }
    else if (Bouton_Appui(Boutons[4], touchX, touchY)) // Tester la connexion
    {
        CanvaBase->fillRect(0, 225, EcranW, 62, C_grisFonce);
        bool loginSuccess = false;
        
        if (sensorType == SENSOR_LIBRE)
        {
            loginSuccess = loginLibreLinkUp();
        }
        else
        {
            loginSuccess = loginDexcomShare();
        }
        
        if (loginSuccess)
        {
            CanvaBase->setFont(u8g2_font_helvB18_tf);
            PrintCentre(CanvaBase, T("ConnectOK"), EcranW / 2, 255, 1);
        }
        else
        { //Problème
            CanvaBase->setFont(u8g2_font_10x20_tf);
            if (ServerConnu){
                 PrintCentre(CanvaBase, T("UserUnknown"), EcranW / 2, 255, 1);
            } else {
                 PrintCentre(CanvaBase, T("ServerNoAccess"), EcranW / 2, 255, 1);
            }
            
        }
        CanvaBase->flush();
    }
}

void pageDexcomRegionSetup()
{
    // Simple region toggle for Dexcom (US vs Non-US)
    dexcomRegion = (dexcomRegion == "US") ? "Non-US" : "US";
    RecordFichierParametres();
    CompteSetup();
}
