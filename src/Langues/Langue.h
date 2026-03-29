#pragma once
#include <Arduino.h>
#include "Ecran/pageLangue.h"
#include "Langues/en.h"
#include "Langues/fr.h"
#include "Langues/de.h"
#include "Langues/it.h"
#include "Langues/es.h"


//Pages tournantes
#define LANG_NONDEF -1
#define LANG_EN 0
#define LANG_FR 1
#define LANG_DE 2
#define LANG_ES 3
#define LANG_IT 4

extern int8_t LaLangue;
extern String LangueSymbole[];

String T(String key); //Traduction Langue