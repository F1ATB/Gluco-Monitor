#include "pageClavier.h"
#include "Config.h"
#include <Arduino.h>
#include "Ecran/Gestion.h"
#include "Ecran/pageCompte.h"
#include "Stock.h"
#include "Langues/Langue.h"
// ==========================
// PARAMETRES
// ==========================

#define KEY_W 44
#define KEY_H 44
#define KEY_SPACING 3
#define START_Y 110

String textBuffer = "";

bool isUpper = true;
bool isNumeric = false;

static unsigned long lastBlink = 0;
static bool cursorVisible = true;

// ==========================
// CLAVIERS
// ==========================

const char *alphaKeys[4][10] = {
    {"A", "Z", "E", "R", "T", "Y", "U", "I", "O", "P"},
    {"Q", "S", "D", "F", "G", "H", "J", "K", "L", "M"},
    {"W", "X", "C", "V", "B", "N", ".", "-", "@", "_"},
    {"SHIFT", "SPACE", "DEL", "123", "Cancel", "OK", "", "", "", ""}};

const char *numKeys[4][10] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"+", "-", "*", "/", "=", "%", "(", ")", "#", "!"},
    {".", ",", "?", ";", ":", "'", "\"", "&", "€", "$"},
    {"ABC", "SPACE", "DEL", ".com", "Cancel", "OK", "", "", "", ""}};

void Position(int row, int col, int &x, int &y, int &keyWidth, int &keyHeight);

// ==========================
// DESSIN
// ==========================

void drawTextBox()
{
  CanvaBase->fillRect(20, 50, 440, 40, RGB565_LIGHTGREY);
  CanvaBase->drawRect(20, 50, 440, 40, RGB565_BLACK);
  CanvaBase->setFont(u8g2_font_helvB14_tf);
  CanvaBase->setTextSize(1);
  CanvaBase->setTextColor(RGB565_BLACK);
  CanvaBase->setCursor(30, 76);

  String displayText = textBuffer;
  if (cursorVisible)
    displayText += "_";

  CanvaBase->print(displayText);
}

void drawKey(int row, int col)
{
  const char *label = isNumeric ? numKeys[row][col] : alphaKeys[row][col];
  if (label[0] == '\0')
    return;

  int x, y, ww, hh;
  Position(row, col, x, y, ww, hh);

  CanvaBase->fillRoundRect(x, y, ww, hh, 8, RGB565_BLACK);
  CanvaBase->drawRoundRect(x, y, ww, hh, 8, RGB565_WHITE);

  CanvaBase->setTextColor(RGB565_WHITE);
  CanvaBase->setFont(u8g2_font_helvB14_tf);
  CanvaBase->setTextSize(1);

  String keyLabel = label;
  if (!isUpper && !isNumeric && keyLabel.length() == 1)
    keyLabel.toLowerCase();

  int16_t x1, y1;
  uint16_t w, h;
  keyLabel = utf8ToLatin15(keyLabel);
  CanvaBase->getTextBounds(keyLabel, 0, 0, &x1, &y1, &w, &h);

  CanvaBase->setCursor(x + (ww - w) / 2, y + (hh + h) / 2);
  CanvaBase->print(keyLabel);
}

void drawKeyboard()
{
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 10; c++)
      drawKey(r, c);
}

// ==========================
// EFFET VISUEL TOUCH
// ==========================

void highlightKey(int row, int col)
{
  int x, y, w, h;
  Position(row, col, x, y, w, h);
  CanvaBase->fillRoundRect(x, y, w, h, 8, RGB565_BLUE);
  CanvaBase->flush();
  delay(120);
  drawKey(row, col);
  CanvaBase->flush();
}

// ==========================
// GESTION APPUI
// ==========================

void handleTouch_clavier(int tx, int ty)
{
  int x, y, w, h;
  for (int r = 0; r < 4; r++)
  {
    for (int c = 0; c < 10; c++)
    {

      Position(r, c, x, y, w, h);
      if (tx > x && tx < x + w &&
          ty > y && ty < y + h)
      {
        const char *label = isNumeric ? numKeys[r][c] : alphaKeys[r][c];
        if (label[0] == '\0')
          return;

        highlightKey(r, c);

        String key = label;

        if (key == "SHIFT")
        {
          isUpper = !isUpper;
          drawKeyboard();
        }
        else if (key == "123")
        {
          isNumeric = true;
          drawKeyboard();
        }
        else if (key == "ABC")
        {
          isNumeric = false;
          drawKeyboard();
        }
        else if (key == "SPACE")
          textBuffer += " ";
        else if (key == "DEL" && textBuffer.length() > 0)
          textBuffer.remove(textBuffer.length() - 1);
        else if (key == "Cancel")
        {
          textBuffer = "";
          if (PageActu == pageClavier_WifiPwd)
          {

            PageActu = pageConfiguration;
            return;
          }
          if (PageActu == pageClavier_CompteEmail || PageActu == pageClavier_ComptePwd)
          {            
            CompteSetup();
      
            return;
          }
        }
        else if (key == "OK")
        {
          textBuffer.trim();
          Serial.println("Validé: " + textBuffer);
          if (PageActu == pageClavier_WifiPwd)
          {
            password = textBuffer;
            RecordFichierParametres();
            CanvaBase->fillScreen(RGB565_DARKGREY);
            PrintCentre(CanvaBase, T("Restart"), EcranW / 2, EcranH / 2, 2);
            CanvaBase->flush();
            delay(1000);
            ESP.restart();
          }
          if (PageActu == pageClavier_CompteEmail)
          {
            libreEmail = textBuffer;
            RecordFichierParametres();           
            CompteSetup();         
            return;
          }
          if (PageActu == pageClavier_ComptePwd)
          {
            librePass = textBuffer;
            RecordFichierParametres();
            CompteSetup();
            return;
          }
        }
        else
        {
          if (!isUpper && !isNumeric)
            key.toLowerCase();
          textBuffer += key;
        }

        drawTextBox();
        delay(180);
      }
    }
  }
}

// ==========================
// Position touches
// ==========================
void Position(int row, int col, int &x, int &y, int &keyWidth, int &keyHeight)
{
  keyWidth = KEY_W;
  keyHeight = KEY_H;
  if (row == 3) // Dernière ligne plus large
  {
    keyWidth += 31;
  }
  x = col * (keyWidth + KEY_SPACING) + 6;
  y = row * (keyHeight + KEY_SPACING) + START_Y;
}
// ==========================
// SETUP
// ==========================

void setup_clavier()
{
  String Titre = T("Clavier");
  CanvaBase->fillScreen(RGB565_DARKGREY);
  if (PageActu == pageClavier_WifiPwd)
  {
    Titre = T("PassWiFi") + ssid;
    textBuffer = password;
  }
  if (PageActu == pageClavier_CompteEmail)
  {
    Titre = T("EmailLinkUp");
    textBuffer = libreEmail;
  }
  if (PageActu == pageClavier_ComptePwd)
  {
    Titre = T("PasseLinkUp");
    textBuffer = librePass;
  }

  CanvaBase->setTextColor(RGB565_BLACK);
  CanvaBase->setFont(u8g2_font_helvB14_tf);
  PrintCentre(CanvaBase, Titre, EcranW / 2, 30, 1);
  drawTextBox();
  drawKeyboard();
  CanvaBase->flush();
}

// ==========================
// LOOP
// ==========================

void loop_touch_clavier()
{

  // Curseur clignotant
  if (millis() - lastBlink > 500)
  {
    cursorVisible = !cursorVisible;
    drawTextBox();
    lastBlink = millis();
  }
}