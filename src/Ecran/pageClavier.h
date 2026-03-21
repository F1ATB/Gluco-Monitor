#pragma once
#include <Arduino.h>
#include "Config.h"
#include <Arduino_GFX_Library.h>
#include "Ecran/Gestion.h"



void loop_touch_clavier();
void handleTouch_clavier(int tx, int ty);
void setup_clavier();