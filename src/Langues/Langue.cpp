#include "Langues/Langue.h"
#include "Config.h"
#include <Arduino.h>
#include "Ecran/pageLangue.h"
#include <ArduinoJson.h>



int8_t LaLangue = LANG_NONDEF;
String LangueSymbole[5] ={"en","fr","de","es","it"};

String T(String key) { //Traduction phrase dans la langue
    String file;
   
    switch(LaLangue)
    {
        case LANG_EN:
            file=String(LangEN);
            break;
        case LANG_FR:
            file=String(LangFR);
            break;
        case LANG_DE:
            file=String(LangDE);
            break;
        case LANG_ES:
            file=String(LangES);
            break;
        case LANG_IT:
            file=String(LangIT);
            break;
    }
    
    JsonDocument doc;
    deserializeJson(doc, file);
    return doc[key] | key; // Retourne la clé si la traduction n'existe pas
}