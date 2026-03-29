#pragma once
#include <Arduino.h>
#include "Config.h"
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include "Heure.h"
#include "Libreview.h"
#include "Ecran/pageMessages.h"

//Pages tournantes
#define pageAccueil 0
#define pageConfiguration 1
#define pageMessages 2
//Pages Fixes  Les unités du chiffre donne les sous pages
#define pageWifiList 10
#define pageClavier_WifiPwd 11
#define pageCompte 20
#define pageClavier_CompteEmail 21
#define pageClavier_ComptePwd 22
#define pageLibreServeur 23
#define pageInfos 30
#define pageAffichage 40
#define pageFuseauH 50
#define pageLangue 60
#define pageAutBrute 70
#define pageAbout 80


// Pin definitions
#define GFX_BL 1
#define TOUCH_ADDR 0x3B
#define TOUCH_SDA 4
#define TOUCH_SCL 8
#define TOUCH_I2C_CLOCK 400000
#define TOUCH_RST_PIN 12
#define TOUCH_INT_PIN 11
#define AXS_MAX_TOUCH_NUMBER 1

//Couleurs
#define C_bleuFonce RGB565(0, 0, 70)
#define C_vertFonce RGB565(0, 50, 0)
#define C_orangeFonce RGB565(70, 50, 0)
#define C_rougeFonce RGB565(70, 0, 0)
#define C_grisFonce RGB565(50, 50, 50)
#define C_grisMoyen RGB565(100, 100, 100)


extern Arduino_Canvas *CanvaBase ;
extern Arduino_Canvas *CanvaMessage ;
extern Arduino_Canvas *CanvaConfig ;
extern Arduino_Canvas *CanvaAccueil ;
extern int16_t PageActu;

extern int16_t EcranW, EcranH, EcranW2, EcranH2, EcranH_20, EcranH_30;
extern int8_t rotation;

void InitEcran();
bool getTouchPoint(uint16_t &x, uint16_t &y, int16_t &dX, int16_t &dY);
void loopEcran();
void AccueiLoop() ;
void PrintCentre(Arduino_Canvas *canva, const String &S, int16_t X, int16_t Y, uint8_t Sz);
void PrintGauche(Arduino_Canvas *canva, const String &S, int16_t X, int16_t Y, uint8_t Sz);
void PrintDroite(Arduino_Canvas *canva, const String &S, int16_t X, int16_t Y, uint8_t Sz);
void QuestionConfiguration(String Question, void (*fonctionSiOK)());
void ClearClick();
String utf8ToLatin15(const String &utf8);

// Structure Boutons
struct Bouton
{ // pour les boutons noirs
    int16_t X0;
    int16_t Y0;
    int16_t W;
    int16_t H;
    String Texte;

};
bool Bouton_Appui(Bouton &b, int16_t x, int16_t y, Arduino_Canvas *canva = CanvaBase);
void Bouton_Trace(Bouton &B,uint16_t colorBord = RGB565_WHITE,Arduino_Canvas *canva = CanvaBase);

// Structure Radio Boutons
struct RadioBouton
{ // pour les boutons ronds
    int16_t X0;
    int16_t Y0;
    int16_t R;
    String Texte;

};  

bool RadioBouton_Appui(RadioBouton &rb, int16_t x, int16_t y);
void RadioBouton_Trace(RadioBouton &rb, uint16_t colorCentre = C_grisFonce);

void AlertePasdeGlycemie();
