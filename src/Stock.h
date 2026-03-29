#pragma once
#include "Config.h"
#include <Arduino.h>
#include "FS.h"
#include "LittleFS.h"

void InitStock();
void ReadFichierParametres();
void RecordFichierParametres();
void RemoveParametres();