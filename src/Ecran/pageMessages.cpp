#include "Ecran/pageMessages.h"
#include "Ecran/Gestion.h"
#include <U8g2lib.h>
#include "Config.h"

#define MessageTitre "Historique"

static uint16_t compteurLoopMessage = 0; // Pour ne pas systematiquement rafraichir la page
static int16_t MesIdxDebut = 0, MesIdxFin = 0;

void MessagesInit()
{
    CanvaMessage->fillScreen(RGB565_BLACK);
    CanvaMessage->setTextColor(RGB565_WHITE);
    CanvaMessage->setFont(u8g2_font_10x20_tf);
    PrintCentre(CanvaMessage, MessageTitre, EcranW2, 18, 1);
}

void pageMessageDefilement(int16_t DeltaTouchY)
{
    if (abs(DeltaTouchY) > 30)
    {
        int16_t deltaLigne = abs(int(DeltaTouchY / 10));
        while (MesIdxDebut != MesIdxFin && deltaLigne != 0)
        {
            if (MessageEcran[MesIdxDebut] == '\n')
                deltaLigne--;
            if (DeltaTouchY < 0)
            {
                MesIdxDebut = (MesIdxDebut + 1) & 0x1FFF;
            }
            else
            {
                MesIdxDebut = (MesIdxDebut - 1) & 0x1FFF;
            }
        }
        if (MesIdxDebut == MesIdxFin && DeltaTouchY < 0)
        {
            MesIdxDebut = MesIdxDebut - 10; // pour revenir en arrière si on est arrivé à la fin
        }
        else
        {
            MesIdxDebut = MesIdxDebut + 1; // pour revenir en arrière si on est arrivé au début
        }
        MesIdxDebut = MesIdxDebut & 0x1FFF;

        MessageLoop();
        compteurLoopMessage = 2; // pour attendre le defilement
    }
    else
    {
        compteurLoopMessage = (compteurLoopMessage + 1) & 0x1F;
        if (compteurLoopMessage == 1)
            MessageLoop();
    }
}

void EcranPrintln(const String &Mm, uint16_t couleur)
{
    int p = 0;
    Serial.println(Mm);

    String M_Latin = utf8ToLatin15(Mm);
    if (couleur != RGB565_WHITE)
        M_Latin = RS + M_Latin; // pour les messages d'erreur, on ajoute un caractère spécial au début pour les différencier visuellement et les afficher en rouge
    for (int i = 0; i < M_Latin.length(); i++)
    {
        MessageEcran[MesIdxFin] = M_Latin.charAt(i);
        MesIdxFin++;
        MesIdxFin = MesIdxFin & 0x1FFF;
    }
    MessageEcran[MesIdxFin] = '\n';
    MesIdxFin++;
    MesIdxFin = MesIdxFin & 0x1FFF;
}
void EcranPrint(const String &m)
{
    Serial.print(m);
    String M_Latin = utf8ToLatin15(m);
    for (int i = 0; i < M_Latin.length(); i++)
    {
        MessageEcran[MesIdxFin] = M_Latin.charAt(i);
        MesIdxFin++;
        MesIdxFin = MesIdxFin & 0x1FFF;
    }
}
void MessageLoop()
{
    CanvaMessage->setFont(u8g2_font_6x10_tf);
    CanvaMessage->setTextColor(RGB565_WHITE);
    int16_t p = -1;
    CanvaMessage->fillRect(0, 20, EcranW, EcranH_20, RGB565_BLACK);
    CanvaMessage->setCursor(0, 40);
    
    uint16_t idx = MesIdxDebut;
    while (idx != MesIdxFin && CanvaMessage->getCursorY() < EcranH_20)
    {
        if (MessageEcran[idx] == RS[0]) // Caractère spécial pour les messages d'erreur
        {
            CanvaMessage->setTextColor(RGB565_ORANGE);
            
            
        }
        else
        {
            if (MessageEcran[idx] == '\n') CanvaMessage->setTextColor(RGB565_WHITE);
            CanvaMessage->print(MessageEcran[idx]);
            if (p == -1 && MessageEcran[idx] == '\n'){
                p = idx;   // On mémorise la position du début de la ligne suivante pour pouvoir revenir en arrière si on défile trop loin
                }       
        }
        idx++;
        idx = idx & 0x1FFF;
    }
    if (CanvaMessage->getCursorY() >= EcranH_20)
    {
        p++;
        p = p & 0x1FFF;
        MesIdxDebut = p; // Ligne d'après
        CanvaMessage->fillRect(0, 0, EcranW, 20, RGB565_BLACK);
        CanvaMessage->setFont(u8g2_font_10x20_tf);
        PrintCentre(CanvaMessage, MessageTitre, EcranW2, 18, 1);
    }
}
void recalPageMessage()  //Pour mettre en fin de tableau quand on est pas sur la page
{
    if (CanvaMessage->getCursorY() > EcranH-40 && PageActu!=pageMessages)
    {
        pageMessageDefilement(-35);
    }
}   