#pragma once
#include "Config.h"
#include <Arduino.h>


extern const char* codeTZ[];
extern const char* nomTZ[];
extern const int NB_TZ;

void DefFuseauHoraire();
void FormatteHeureDate();

long convertToUnix(const char *timestamp); // Convertit une date au format "MM/DD/YYYY HH:MM:SS AM/PM" en temps Unix
int unixToHeure(time_t unixTime);