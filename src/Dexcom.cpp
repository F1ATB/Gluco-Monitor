#include <Arduino.h>
#include <Heure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "Dexcom.h"
#include "Langues/Langue.h"

// Dexcom Share API variables
static String dexcomSessionId = "";
static String dexcomAccountId = "";

// Dexcom Share API base URL, Non-US (default)
static String dexcomBaseURL = "https://shareous1.dexcom.com";

// Dexcom Share API endpoints
const char* DEXCOM_LOGIN_ENDPOINT = "/ShareWebServices/Services/General/LoginPublisherAccountById";
const char* DEXCOM_AUTHENTICATE_ENDPOINT = "/ShareWebServices/Services/General/AuthenticatePublisherAccount";
const char* DEXCOM_GLUCOSE_ENDPOINT = "/ShareWebServices/Services/Publisher/ReadPublisherLatestGlucoseValues";

const char* APP_ID = "d89443d2-327c-4a6f-89e5-496bbb0317db";

bool loginDexcomShare()
{
    ServerConnu = false;
    
    if (dexcomRegion == "US") {
        dexcomBaseURL = "https://share2.dexcom.com";
    } else if (dexcomRegion == "JP") {
        dexcomBaseURL = "https://share.dexcom.jp";
        APP_ID = "d8665ade-9673-4e27-9ff6-92db4ce13d13";
    }
    Serial.println("Connexion à Dexcom Share: " + dexcomBaseURL);
    
    HTTPClient https;
    String payload;
    int httpCode;
    String response;
    
    // Step 1: Authenticate to get account ID (only if not cached)
    if (dexcomAccountId.length() == 0) {
        Serial.println("Récupération de l'Account ID...");
        https.begin(dexcomBaseURL + String(DEXCOM_AUTHENTICATE_ENDPOINT));
        https.setTimeout(15000);
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Accept", "application/json");
        https.addHeader("User-Agent", "Dexcom Share/3.0.2.11 CFNetwork/711.2.23 Darwin/14.0.0");
        
        JsonDocument authDoc;
        authDoc["accountName"] = dexcomUsername;
        authDoc["password"] = dexcomPassword;
        authDoc["applicationId"] = APP_ID;
        
        serializeJson(authDoc, payload);
        
        httpCode = https.POST(payload);
        response = https.getString();
        
        // Format ConnectionJSON as proper JSON with accountId field
        JsonDocument connDoc;
        response.trim();
        if (response.startsWith("\"") && response.endsWith("\"")) {
            connDoc["accountId"] = response.substring(1, response.length() - 1);
        } else {
            connDoc["accountId"] = response;
        }
        serializeJson(connDoc, ConnectionJSON);
        
        https.end();
        
        if (httpCode != HTTP_CODE_OK) {
            Serial.println("Authentification échouée: " + String(httpCode));
            EcranPrintln(HEURE + T("LoginFailed") + String(httpCode), RGB565_ORANGE);
            return false;
        }
        
        response.trim();
        if (!response.startsWith("\"") || !response.endsWith("\"")) {
            Serial.println("Format de réponse invalide");
            return false;
        }
        
        dexcomAccountId = response.substring(1, response.length() - 1);
        Serial.println("Account ID obtenu: " + String(dexcomAccountId));
    } else {
        Serial.println("Utilisation de l'Account ID en cache");
    }
    
    ServerConnu = true;
    
    // Step 2: Login with account ID to get session ID
    https.begin(dexcomBaseURL + String(DEXCOM_LOGIN_ENDPOINT));
    https.setTimeout(15000);
    https.addHeader("Content-Type", "application/json");
    https.addHeader("Accept", "application/json");
    https.addHeader("User-Agent", "Dexcom Share/3.0.2.11 CFNetwork/711.2.23 Darwin/14.0.0");
    
    JsonDocument loginDoc;
    loginDoc["accountId"] = dexcomAccountId;
    loginDoc["password"] = dexcomPassword;
    loginDoc["applicationId"] = APP_ID;
    
    serializeJson(loginDoc, payload);
    
    httpCode = https.POST(payload);
    response = https.getString();
    
    // Format LoginJSON as proper JSON with sessionId field
    JsonDocument loginJsonDoc;
    response.trim();
    if (response.startsWith("\"") && response.endsWith("\"")) {
        loginJsonDoc["sessionId"] = response.substring(1, response.length() - 1);
    } else {
        loginJsonDoc["sessionId"] = response;
    }
    serializeJson(loginJsonDoc, LoginJSON);
    
    https.end();
    
    if (httpCode != HTTP_CODE_OK) {
        Serial.println("Login échoué: " + String(httpCode));
        EcranPrintln(HEURE + T("LoginFailed") + String(httpCode), RGB565_ORANGE);
        return false;
    }
    
    response.trim();
    if (!response.startsWith("\"") || !response.endsWith("\"")) {
        Serial.println("Format de réponse invalide");
        return false;
    }
    
    dexcomSessionId = response.substring(1, response.length() - 1);
    Serial.println("Session ID: " + String(dexcomSessionId));
    
    return dexcomSessionId.length() > 30;
}

void getDexcomReadings()
{
    HTTPClient https;
    
    Serial.println("getDexcomReadings - Session ID: " + dexcomSessionId);
    
    String url = dexcomBaseURL + String(DEXCOM_GLUCOSE_ENDPOINT) +
                 "?sessionId=" + dexcomSessionId +
                 "&minutes=1440&maxCount=288"; // 288 = 24h of 5-min readings

    Serial.println("URL Dexcom: " + url);
    https.begin(url);
    https.setTimeout(15000);

    https.addHeader("Content-Type", "application/json");
    https.addHeader("Accept", "application/json");
    https.addHeader("User-Agent", "Dexcom Share/3.0.2.11 CFNetwork/711.2.23 Darwin/14.0.0");

    int httpCode = https.GET();
    String response = https.getString();

    Serial.println("HTTP Code: " + String(httpCode));
    Serial.println("Response: " + response);
    
    if (httpCode == HTTP_CODE_OK) {
        Serial.println("Données Dexcom: " + String(response.length()) + " caractères");
        GraphJSON = response;

        if (response.length() == 0) {
            Serial.println("Réponse vide - aucune donnée disponible");
            EcranPrintln(HEURE + T("GlucoFailed") + " (empty response)", RGB565_ORANGE);
            https.end();
            return;
        }

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);
        
        if (error) {
            Serial.println("Erreur parsing JSON Dexcom: " + String(error.c_str()));
            https.end();
            return;
        }

        JsonArray readings = doc.as<JsonArray>();
        
        if (readings.size() > 0) {
            // Get the most recent reading (first in array)
            JsonObject latestReading = readings[0];
            
            int mgdl = latestReading["Value"];
            const char* trend = latestReading["Trend"];
            const char* timestamp = latestReading["WT"]; // Wall Time
            
            GlycemieVal = mgdl;
            Glycemie = String(mgdl);
            
            // Map Dexcom trend
            // -1=DoubleDown, 0=undefined, 1=Down, 2=DownRight, 3=Flat, 4=UpRight, 5=Up, 6=DoubleUp
            TrendArrow = 0; // Default to undefined
            if (trend != nullptr) {
                String trendStr = String(trend);
                if (trendStr == "DoubleUp") TrendArrow = 6;        // DoubleUp
                else if (trendStr == "SingleUp") TrendArrow = 5;   // Up
                else if (trendStr == "FortyFiveUp") TrendArrow = 4; // UpRight
                else if (trendStr == "Flat") TrendArrow = 3;       // Right (Flat)
                else if (trendStr == "FortyFiveDown") TrendArrow = 2; // DownRight
                else if (trendStr == "SingleDown") TrendArrow = 1; // Down
                else if (trendStr == "DoubleDown") TrendArrow = -1; // DoubleDown
            }
            
            // Parse timestamp - Dexcom format: "Date(1234567890000)"
            if (timestamp != nullptr) {
                String tsStr = String(timestamp);
                int startIdx = tsStr.indexOf('(') + 1;
                int endIdx = tsStr.indexOf(')');
                if (startIdx > 0 && endIdx > startIdx) {
                    lastGlyUnixTime = tsStr.substring(startIdx, endIdx - 3).toInt();
                }
            }
            
            String DateGly = unixToTimestamp(lastGlyUnixTime);
            EcranPrintln(HEURE + T("LastGlyco") + formatGlucoseValue(GlycemieVal) + " " + getGlucoseUnitLabel() + " " + T("le") + DateGly);
            lastReceptionGlycMillis = millis();
            
            Serial.println("Glycémie: " + formatGlucoseValue(GlycemieVal) + " " + getGlucoseUnitLabel());
            Serial.println("TrendArrow: " + String(TrendArrow));
            Serial.println("Timestamp: " + String(lastGlyUnixTime));
            
            // Store all readings in the glucose array
            pointCountGly = 0;
            for (int i = readings.size() - 1; i > -1; i--) {
                if (pointCountGly >= MAX_POINTS) break;
                
                int value = readings[i]["Value"];
                const char* ts = readings[i]["WT"];
                
                if (ts != nullptr) {
                    String tsStr = String(ts);
                    int startIdx = tsStr.indexOf('(') + 1;
                    int endIdx = tsStr.indexOf(')');
                    if (startIdx > 0 && endIdx > startIdx) {
                        long unixTime = tsStr.substring(startIdx, endIdx - 3).toInt();
                        glucoseValues[pointCountGly] = value;
                        glucoseHeure[pointCountGly] = unixTime;
                        pointCountGly++;
                    }
                }
            }
            Serial.println("Nombre de points Dexcom: " + String(pointCountGly));
            lastGlycOkMillis = millis();
        } else {
            EcranPrintln(HEURE + T("GlucoFailed") + " (no data)", RGB565_ORANGE);
        }
    } else {
        EcranPrintln(HEURE + T("GlucoFailed") + String(httpCode), RGB565_ORANGE);
        Serial.println("Erreur lecture Dexcom: " + response);
    }

    https.end();
}

void LectureDexcom()
{
    // Dexcom updates every 5 minutes (300 seconds) + 15 seconds extra
    recurGlycMillis = 315000;
    // Don't contact server if we have recent data
    if (AgeGlycemie < 315 && lastGlyUnixTime > 0) {
        // We have recent data, no need to poll yet
        return;
    } 
    if (AgeGlycemie > 500) {
        recurGlycMillis = 90000; // 1.5 minutes if very old
    } else if (AgeGlycemie > 315) {
        recurGlycMillis = 30000; // 30 seconds if data is old
    }
    
    if (millis() - lastReceptionGlycMillis > recurGlycMillis || lastDemandeGlycMillis == 0) {
        lastDemandeGlycMillis = millis();
        
        if (dexcomUsername != "" && dexcomPassword != "") {
            Serial.println("Demande nouvelle glycémie Dexcom...");
            if (loginDexcomShare()) {
                getDexcomReadings();
            }
        } else {
            EcranPrintln(T("DexcomIndefini"));
        }
        
        lastReceptionGlycMillis = millis();
    }
}

void clearDexcomCache()
{
    Serial.println("Clearing Dexcom cache...");
    dexcomSessionId = "";
    dexcomAccountId = "";
    dexcomBaseURL = "https://shareous1.dexcom.com";
}

