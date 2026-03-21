#pragma once
#include <Arduino.h>
#include "Config.h"
#include <Arduino_GFX_Library.h>
#include "Ecran/Gestion.h"

void EcranPrintln(const String &Mm, uint16_t couleur = RGB565_WHITE);
void EcranPrint(const String &m);
void MessagesInit();
void MessageLoop();
void pageMessageDefilement(int16_t DeltaTouchY);
void recalPageMessage(); // Pour mettre en fin de tableau quand on est pas sur la page