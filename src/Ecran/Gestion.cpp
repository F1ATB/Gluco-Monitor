#include <Arduino.h>
#include <esp32-hal-ledc.h>
#include <Arduino_GFX_Library.h> //Works with Version 1.6.0 and not 1.6.1 (October 2025)
#include <Wire.h>
#include <U8g2lib.h>

#include "Config.h"
#include "Ecran/Gestion.h"
#include "Ecran/pageMessages.h"
#include "Ecran/pageClavier.h"
#include "Ecran/pageAccueil.h"
#include "Ecran/pageConfiguration.h"
#include "Ecran/pageWiFiList.h"
#include "Ecran/pageCompte.h"
#include "Ecran/pageLibreServeur.h"
#include "Ecran/pageInfos.h"
#include "Ecran/pageAffichage.h"
#include "Ecran/pageFuseauH.h"
#include "Ecran/pageLangue.h"
#include "Ecran/pageAutBrute.h"

//************Alternative declarations *******************************************
// Arduino_DataBus *bus = new Arduino_ESP32QSPI(45, 47, 21, 48, 40, 39);
// Arduino_GFX *g = new Arduino_AXS15231B(bus, GFX_NOT_DEFINED, 0, false, 320, 480);
// Arduino_Canvas *CanvaBase = new Arduino_Canvas(320, 480, g, 0, 0, 0);
//*******************************************************************************

Arduino_ESP32QSPI *bus = new Arduino_ESP32QSPI(45, 47, 21, 48, 40, 39);
Arduino_AXS15231B *g = new Arduino_AXS15231B(bus, GFX_NOT_DEFINED, 0, false, 320, 480);
Arduino_Canvas *CanvaBase = new Arduino_Canvas(320, 480, g, 0, 0, 0);

Arduino_Canvas *CanvaAccueil = nullptr;
Arduino_Canvas *CanvaMessage = nullptr;
Arduino_Canvas *CanvaConfig = nullptr;

static uint16_t touchX, touchY;
static int16_t PageOld = 0, PageDelta = 0, PageTotalTournante = 3;
int16_t PageActu = 0;
static int16_t DeltaTouchX = 0, DeltaTouchY = 0;

static unsigned long LastClickMillis = 0, PeriodeRafraichissement = 0, ClickLong = 0, LastClickPageFixe = 0;

// Ecran
int16_t EcranW, EcranH, EcranW2, EcranH2, EcranH_20, EcranH_30;
int8_t rotation = 3;

// Prototypes
bool getTouchPoint(uint16_t &x, uint16_t &y, int16_t &dX, int16_t &dY);
void PrintCentre(Arduino_Canvas *canva, const String &S, int16_t X, int16_t Y, uint8_t Sz);
void PrintGauche(Arduino_Canvas *canva, const String &S, int16_t X, int16_t Y, uint8_t Sz);
void PrintDroite(Arduino_Canvas *canva, const String &S, int16_t X, int16_t Y, uint8_t Sz);
String utf8ToLatin15(const String &utf8);

void InitEcran()
{

  // Initialize touch
  Wire.begin(TOUCH_SDA, TOUCH_SCL);
  Wire.setClock(TOUCH_I2C_CLOCK);

  // Configure touch pins
  pinMode(TOUCH_INT_PIN, INPUT_PULLUP);
  pinMode(TOUCH_RST_PIN, OUTPUT);
  digitalWrite(TOUCH_RST_PIN, LOW);
  delay(200);
  digitalWrite(TOUCH_RST_PIN, HIGH);
  delay(200);

  // Initialize Display
  CanvaBase->begin();
  CanvaBase->setRotation(rotation);

  EcranW = CanvaBase->width();
  EcranW2 = EcranW / 2;
  EcranH = CanvaBase->height();
  EcranH_20 = EcranH - 20;
  EcranH2 = EcranH / 2;
  EcranH_30 = EcranH - 30;
  CanvaAccueil = new Arduino_Canvas(EcranW, EcranH, g, 0, 0, 0);
  CanvaMessage = new Arduino_Canvas(EcranW, EcranH, g, 0, 0, 0);
  CanvaConfig = new Arduino_Canvas(EcranW, EcranH, g, 0, 0, 0);

  CanvaAccueil->begin(GFX_SKIP_OUTPUT_BEGIN);
  CanvaMessage->begin(GFX_SKIP_OUTPUT_BEGIN);
  CanvaConfig->begin(GFX_SKIP_OUTPUT_BEGIN);

  CanvaBase->fillScreen(RGB565_BLACK);
  CanvaBase->flush();
  delay(100);
  // Initialisation du PWM sur GPIO 1
  ledcAttach(GFX_BL, 5000, 8); //  5kHz, 8 bits de résolution
  // Baisser la luminosité (0 = éteint, 255 = max)
  ledcWrite(GFX_BL, 255);
  Trace_Gauge(CanvaBase);
  CanvaBase->setTextColor(RGB565_WHITE);
  CanvaBase->setFont(u8g2_font_fub35_tf);
  PrintCentre(CanvaBase, "Gluco-Monitor", EcranW2, EcranH2 - 10, 1);
  CanvaBase->setFont(u8g2_font_helvB18_tf);
  PrintDroite(CanvaBase, "by F1ATB", EcranW - 10, EcranH - 30, 1);
  CanvaBase->flush();
  Serial.println("Ecran Init");
  delay(3000);

  AccueilInit();
  MessagesInit();
  ParaInit();
  getTouchPoint(touchX, touchY, DeltaTouchX, DeltaTouchY); // pour initialiser les variables de touché
  PageDelta = 0;
}

void loopEcran()
{
  if ((millis() - LastClickMillis) > 20)
  {
    LastClickMillis = millis();
    //===========================
    // Lecture du Touché
    //===========================

    if (getTouchPoint(touchX, touchY, DeltaTouchX, DeltaTouchY))
    {

      if (PageActu < PageTotalTournante)
      //===============
      { // Touché  Pages tournantes
        //=================
        if (DeltaTouchX < -50)
        {
          PageActu = (PageOld + 1) % PageTotalTournante;
          PageDelta = 480; // Delta en pixels
        }
        if (DeltaTouchX > 50)
        {
          PageActu = (PageTotalTournante + PageOld - 1) % PageTotalTournante;
          PageDelta = -480; // Delta en pixels
        }
        if (PageDelta == 0) // On attend la stabilisation du touché pour éviter les actions intempestives
        {
          if (PageActu == pageMessages)
            pageMessageDefilement(DeltaTouchY);
          if (PageActu == pageConfiguration && millis() - ClickLong > 300)
            pageConfigurationChoix(touchX, touchY, DeltaTouchX, DeltaTouchY);
        }
      }
      else
      //================
      { // Touché Pages fixes
        //=================
        if (millis() - LastClickPageFixe < 200)
          return; // Anti-rebond simple

        PageDelta = 0;
        if (DeltaTouchX < -6) // On veut revenir à la page de configuration generale
        {
          PageActu = pageConfiguration;
          PageDelta = 480; // Delta en pixels
        }
        else if (DeltaTouchX > 6)
        {
          PageActu = pageConfiguration;
          PageDelta = -480; // Delta en pixels
        }
        else
        {
          switch (PageActu)
          {
          case pageClavier_WifiPwd:
          case pageClavier_CompteEmail:
          case pageClavier_ComptePwd:
            handleTouch_clavier(touchX, touchY);
            break;
          case pageCompte:
            handleTouch_Compte(touchX, touchY);
            break;
          case pageWifiList:
            handleTouch_WifiList(touchX, touchY);
            break;
          case pageLibreServeur:
            handleTouch_LibreServeur(touchX, touchY);
            break;
          case pageInfos:
            handleTouch_Infos(touchX, touchY);
            break;
          case pageAffichage:
            handleTouch_Affichage(touchX, touchY);
            break;
          case pageFuseauH:
            handleTouch_Fuseau(touchX, touchY,DeltaTouchY);
            break;
          case pageLangue:
            handleTouch_Langue(touchX, touchY);
            break;
          case pageAutBrute:
            handleTouch_AutBrute(touchX, touchY);
            break;
          }
        }
        LastClickPageFixe = millis();
      }
    }
    else
    //=================
    { // Pas de click touché.
      //=================
      ClickLong = millis();
    }
    //===========================
    // Rafraichissement Affichage
    //===========================
    // PAge en glissement
    if (PageDelta != 0 || (millis() - PeriodeRafraichissement) > 200)
    {
      PeriodeRafraichissement = millis();

      if (PageDelta > 0)
      {
        PageDelta = PageDelta - 80;
        if (PageDelta <= 0)
        {
          PageDelta = 0;
          PageOld = PageActu;
        }
      }
      else
      {
        PageDelta = PageDelta + 80;
        if (PageDelta >= 0)
        {
          PageDelta = 0;
          PageOld = PageActu;
        }
      }

      switch (PageActu) // Affichage cyclique de la page en cours
      {
        // page tournantes
      case pageAccueil:
        AccueiLoop();
        CanvaBase->draw16bitRGBBitmap(PageDelta, 0, CanvaAccueil->getFramebuffer(), EcranW, EcranH);
        break;
      case pageMessages:
        pageMessageDefilement(0); // pour forcer le rafraichissement  de temps en temps
        CanvaBase->draw16bitRGBBitmap(PageDelta, 0, CanvaMessage->getFramebuffer(), EcranW, EcranH);
        break;
      case pageConfiguration:
        CanvaBase->draw16bitRGBBitmap(PageDelta, 0, CanvaConfig->getFramebuffer(), EcranW, EcranH);
        break;
        // Pages Fixes
      case pageClavier_WifiPwd:
        loop_touch_clavier();
        break;
      case pageClavier_CompteEmail:
        loop_touch_clavier();
        break;
      case pageClavier_ComptePwd:
        loop_touch_clavier();
        break;
      case pageAutBrute:
        loop_Autorisation();
      }
      recalPageMessage();
      //=====================================
      // BArre de séparation pages tournantes
      //=====================================
      if (PageDelta > 0)
        CanvaBase->writeFastVLine(PageDelta, 0, EcranH_20, C_grisFonce);
      if (PageDelta < 0)
        CanvaBase->writeFastVLine(PageDelta + EcranW, 0, EcranH_20, C_grisFonce);
      CanvaBase->flush();
    }
  }
}
//===========================
// Lecture du Touché
//===========================
bool getTouchPoint(uint16_t &x, uint16_t &y, int16_t &dX, int16_t &dY)
{
  bool valide = true;
  uint16_t X0 = x;
  uint16_t Y0 = y;
  uint8_t data[AXS_MAX_TOUCH_NUMBER * 6 + 2] = {0};

  // Define the read command array properly
  const uint8_t read_cmd[11] = {
      0xb5, 0xab, 0xa5, 0x5a, 0x00, 0x00,
      (uint8_t)((AXS_MAX_TOUCH_NUMBER * 6 + 2) >> 8),
      (uint8_t)((AXS_MAX_TOUCH_NUMBER * 6 + 2) & 0xff),
      0x00, 0x00, 0x00};

  Wire.beginTransmission(TOUCH_ADDR);
  Wire.write(read_cmd, 11);
  if (Wire.endTransmission() != 0)
  {
    valide = false;
  }
  else
  {

    if (Wire.requestFrom(TOUCH_ADDR, sizeof(data)) != sizeof(data))
    {
      valide = false;
    }
    else
    {
      for (int i = 0; i < sizeof(data); i++)
      {
        data[i] = Wire.read();
      }

      if (data[1] > 0 && data[1] <= AXS_MAX_TOUCH_NUMBER)
      {
        uint16_t rawX = ((data[2] & 0x0F) << 8) | data[3];
        uint16_t rawY = ((data[4] & 0x0F) << 8) | data[5];
        if (rawX > 500 || rawY > 500)
          valide = false;
        switch (rotation)
        {
        case 0:
          x = rawX;
          y = rawY;
          break;
        case 1:
          y = map(rawX, 0, 320, 320, 0);
          x = rawY;
          break;
        case 2:
          x = map(rawX, 0, 320, 320, 0);
          y = map(rawY, 0, 480, 480, 0);
          break;
        case 3:
          y = rawX;
          x = map(rawY, 0, 480, 480, 0);
          break;
        }
      }
      else
      {
        valide = false;
      }
    }
  }
  if (valide)
  {
    if (dX == 1000)
    { // ancien pas valide
      dX = 0;
      dY = 0;
    }
    else
    {
      dX = x - X0;
      dY = y - Y0;
    }
  }
  else
  {
    dX = 1000; // Pas valide
  }
  return valide;
}
//===========================
// Fonctions d'affichage de texte
//===========================
void PrintCentre(Arduino_Canvas *canva, const String &S, int16_t X, int16_t Y, uint8_t Sz)
{
  int16_t x1, y1;
  uint16_t w, h;
  canva->setTextSize(Sz);
  canva->getTextBounds(utf8ToLatin15(S), 0, 0, &x1, &y1, &w, &h);
  canva->setCursor(X - w / 2, Y);
  canva->print(utf8ToLatin15(S));
}
void PrintGauche(Arduino_Canvas *canva, const String &S, int16_t X, int16_t Y, uint8_t Sz)
{
  canva->setTextSize(Sz);
  canva->setCursor(X + 4, Y + 3);
  canva->print(utf8ToLatin15(S));
}
void PrintDroite(Arduino_Canvas *canva, const String &S, int16_t X, int16_t Y, uint8_t Sz)
{
  if (X < 0)
    X = canva->width();
  canva->setTextSize(Sz);
  uint16_t w, h;
  int16_t x1, y1;
  canva->getTextBounds(utf8ToLatin15(S), 0, 0, &x1, &y1, &w, &h);
  canva->setCursor(X - w - 4, Y + 3);
  canva->print(utf8ToLatin15(S));
}

String utf8ToLatin15(const String &utf8)
{
  String latin15;
  latin15.reserve(utf8.length()); // optimisation mémoire

  for (size_t i = 0; i < utf8.length();)
  {
    uint8_t c = (uint8_t)utf8[i];

    if (c < 0x80)
    {
      // ASCII direct
      latin15 += (char)c;
      i++;
    }
    else if ((c & 0xE0) == 0xC0 && i + 1 < utf8.length())
    {
      // Séquence UTF-8 sur 2 octets
      uint8_t c1 = (uint8_t)utf8[i + 1];
      if ((c1 & 0xC0) == 0x80)
      {
        uint16_t codepoint = ((c & 0x1F) << 6) | (c1 & 0x3F);
        if (codepoint <= 0xFF)
        {
          latin15 += (char)codepoint;
        }
        else
        {
          latin15 += '?';
        }
        i += 2;
      }
      else
      {
        latin15 += '?';
        i++;
      }
    }
    else if ((c & 0xF0) == 0xE0 && i + 2 < utf8.length())
    {
      // Séquence UTF-8 sur 3 octets (ex: €, Œ…)
      uint8_t c1 = (uint8_t)utf8[i + 1];
      uint8_t c2 = (uint8_t)utf8[i + 2];
      if ((c1 & 0xC0) == 0x80 && (c2 & 0xC0) == 0x80)
      {
        uint16_t codepoint = ((c & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);

        // Mapping spécial Latin-15
        switch (codepoint)
        {
        case 0x20AC:
          latin15 += (char)0xA4;
          break; // €
        case 0x0160:
          latin15 += (char)0xA6;
          break; // Š
        case 0x0161:
          latin15 += (char)0xA8;
          break; // š
        case 0x017D:
          latin15 += (char)0xB4;
          break; // Ž
        case 0x017E:
          latin15 += (char)0xB8;
          break; // ž
        case 0x0152:
          latin15 += (char)0xBC;
          break; // Œ
        case 0x0153:
          latin15 += (char)0xBD;
          break; // œ
        case 0x0178:
          latin15 += (char)0xBE;
          break; // Ÿ
        case 0x2705:
          latin15 += (char)0xBB;
          break; //  ✅
        case 0x26A0:
          latin15 += (char)0xA4;
          break; // ⚠️
        case 0xFE0F:
          latin15 += (char)0x20;
          break; //  Selecteur Emoji colore
        default:
          if (codepoint <= 0xFF)
          {
            latin15 += (char)codepoint;
          }
          else
          {
            latin15 += '?';
          }
        }
        i += 3;
      }
      else
      {
        latin15 += '?';
        i++;
      }
    }
    else
    {
      latin15 += '?';
      i++;
    }
  }

  return latin15;
}
//========= GESTION DES BOUTONS =========================
bool Bouton_Appui(Bouton &b, int16_t Tx, int16_t Ty, Arduino_Canvas *canva)
{
  bool touche(Tx >= b.X0 && Tx <= (b.X0 + b.W) &&
              Ty >= b.Y0 && Ty <= (b.Y0 + b.H));
  if (touche) // Passe le bouton en bleu pendant 120ms pour un retour visuel de l'appui
  {
    canva->fillRoundRect(b.X0, b.Y0, b.W, b.H, 8, RGB565_BLUE);
    if (canva != CanvaBase)
    {
      CanvaBase->draw16bitRGBBitmap(0, 0, canva->getFramebuffer(), EcranW, EcranH);
    }
    CanvaBase->flush();
    delay(120);
    Bouton_Trace(b, RGB565_WHITE, canva);
    if (canva != CanvaBase)
    {
      CanvaBase->draw16bitRGBBitmap(0, 0, canva->getFramebuffer(), EcranW, EcranH);
    }
    CanvaBase->flush();
  }

  return touche;
}

void Bouton_Trace(Bouton &B, uint16_t colorBord, Arduino_Canvas *canva)
{
  canva->fillRoundRect(B.X0, B.Y0, B.W, B.H, 8, RGB565_BLACK);
  canva->drawRoundRect(B.X0, B.Y0, B.W, B.H, 8, colorBord);
  canva->setFont(u8g2_font_10x20_mf);
  canva->setTextColor(RGB565_WHITE);
  int16_t x1, y1;
  uint16_t w, h;
  canva->getTextBounds(utf8ToLatin15(B.Texte), 0, 0, &x1, &y1, &w, &h);
  PrintCentre(canva, B.Texte, B.X0 + B.W / 2, B.Y0 + (B.H + h) / 2 -2, 1);
}

bool RadioBouton_Appui(RadioBouton &rb, int16_t x, int16_t y)
{
  bool touche(x >= (rb.X0 - 10) && x <= (rb.X0 + 2 * rb.R + 10) &&
              y >= (rb.Y0 - 10) && y <= (rb.Y0 + 2 * rb.R + 10));

  return touche;
}
void RadioBouton_Trace(RadioBouton &rb, uint16_t colorCentre)
{
  CanvaBase->fillCircle(rb.X0 + rb.R, rb.Y0 + rb.R, rb.R, colorCentre);
  CanvaBase->drawCircle(rb.X0 + rb.R, rb.Y0 + rb.R, rb.R, RGB565_WHITE);
  CanvaBase->setFont(u8g2_font_10x20_mf);
  CanvaBase->setTextColor(RGB565_WHITE);
  PrintGauche(CanvaBase, rb.Texte, rb.X0 + 2 * rb.R+2, rb.Y0 + rb.R +2, 1);
}
void AlertePasdeGlycemie()
{
  CanvaBase->fillScreen(RGB565_DARKSALMON);
  CanvaBase->setFont(u8g2_font_helvB18_tf);
  CanvaBase->setTextColor(RGB565_YELLOW);
  PrintCentre(CanvaBase, "Pas de glycémie récente", EcranW2, EcranH2 - 10, 1);
  PrintCentre(CanvaBase, "Redémarrage", EcranW2, EcranH2 + 40, 1);
  CanvaBase->flush();
  delay(10000);
  ESP.restart();
}