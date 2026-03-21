#pragma once
#include <Arduino.h>
#include "Config.h"
#include <Arduino_GFX_Library.h>
#include "Ecran/Gestion.h"

void handleTouch_AutBrute(uint16_t touchX, uint16_t touchY);
void AutorisationInit();
void loop_Autorisation();