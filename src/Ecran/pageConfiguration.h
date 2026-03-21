#pragma once
#include <Arduino.h>
#include "Config.h"
#include <Arduino_GFX_Library.h>
#include "Ecran/Gestion.h"

void ParaInit();
void pageConfigurationChoix(uint16_t touchX,uint16_t touchY, int16_t DeltaTouchX, int16_t DeltaTouchY);