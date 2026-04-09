#pragma once
#include "Config.h"
#include <Arduino.h>
#include "Heure.h"
#include "Ecran/Gestion.h"

// Dexcom Share API functions
void LectureDexcom();
bool loginDexcomShare();
void getDexcomReadings();
