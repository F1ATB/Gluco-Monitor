#include <Arduino.h>
#include <Heure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "mbedtls/md.h"
#include "Config.h"
#include "Libreview.h"

// variables globales
static String AuthToken = "";
static String userID = "";
static String patientId = "";
static String SHAuserID = "";
static bool UserOK = false;



// URL base de l’API (Europe ici, peut être différente)
// sur https://api.libreview.io", on reçoit plein d'info
// sur "https://api-eu.libreview.io"; il suggere une redirection ver fr
//const char *baseURL = "https://api-fr.libreview.io"; //"https://api-eu2.libreview.io";
// Proxy si problème
//const char *baseURL = "https://libreview-proxy.onrender.com/fr";

static String baseURL = "";// "https://api-fr.libreview.io"; //"https://api-eu2.libreview.io";
void getGraph();

String getSHA256(String payload);

bool loginLibreLinkUp()
{
  bool logUserOK = false;
  ServerConnu = false;
  baseURL = "https://api.libreview.io";
  if (libreZone != "")
  baseURL = "https://api-" + libreZone + ".libreview.io";

  Serial.println("Tentative de connexion à LibreLinkUp sur " + baseURL);
  delay(10);
  HTTPClient https;
  String loginURL = baseURL + "/llu/auth/login";

  https.begin(loginURL);
  https.setTimeout(15000); // HTTPClient en ms

  https.addHeader("Content-Type", "application/json");
  https.addHeader("Accept", "application/json");
  // Mes rajouts
  https.addHeader("User-Agent", "okhttp/4.9.0");
  https.addHeader("connection", "Keep-Alive");
  https.addHeader("product", "llu.android");
  https.addHeader("version", "4.17.0"); //Mettre le numéro de version de LinkLibre Up

  // JSON avec email & mot de passe
  String payload = "{\"email\":\"" + libreEmail + "\",\"password\":\"" + librePass + "\"}";

  int httpCode = https.POST(payload);
  String response = https.getString();
  LoginJSON = response;
  if (httpCode == HTTP_CODE_OK)
  {
    ServerConnu = true;
    Serial.println("Login OK: " + String(response.length()) + " caractères");
    JsonDocument doc;
    deserializeJson(doc, response);
    
    
    // Mon code
    AuthToken = doc["data"]["authTicket"]["token"].as<String>();

    

    Serial.println("Longueur : " + AuthToken.length());

    userID = doc["data"]["user"]["id"].as<String>();
    SHAuserID = getSHA256(userID);
    Serial.println("userID: " + userID);
    if (AuthToken.length() > 100 ) logUserOK = true;
  
  }
  else
  {
    EcranPrintln(HEURE + " Login to libreview failed, HTTP code: " + String(httpCode),RGB565_ORANGE);  
    Serial.println("Erreur de login, code: " + String(httpCode));
  }

  https.end();
  return logUserOK;
}
//============Obtention dernière Glycémie============
void getConnection() 
{
  HTTPClient https;
  String url = String(baseURL) + "/llu/connections";

  https.begin(url);
  https.setTimeout(15000); // HTTPClient en ms

  https.addHeader("Content-Type", "application/json");
  https.addHeader("Accept", "application/json");
  // Mes rajouts
  https.addHeader("User-Agent", "okhttp/4.9.0");
  https.addHeader("connection", "Keep-Alive");
  https.addHeader("product", "llu.android");
  https.addHeader("version", "4.17.0");

  // Header d’authentification

  https.addHeader("Authorization", "Bearer " + AuthToken);

  https.addHeader("Account-Id", SHAuserID);

  int httpCode = https.GET();

  if (httpCode == HTTP_CODE_OK)
  {
    String response = https.getString();
    Serial.println("Données connexion: " + String(response.length()) + " caractères");

    // StaticJsonDocument<512> doc;
    JsonDocument doc;
    deserializeJson(doc, response);
    ConnectionJSON = response;
    
    Glycemie = doc["data"][0]["glucoseItem"]["ValueInMgPerDl"].as<String>();
    if (Glycemie==""){
      GlycemieVal=0;
    } else {
      GlycemieVal = Glycemie.toInt();
    }
    String DateGly = doc["data"][0]["glucoseItem"]["Timestamp"].as<String>();
    targetLow = doc["data"][0]["targetLow"];
    targetHigh = doc["data"][0]["targetHigh"];
    Serial.println("Glycémie actuelle : " + Glycemie + "mg/dl  le " + DateGly);
    EcranPrintln(HEURE + " Dernière Glycémie connue : " + Glycemie + "mg/dl  le " + DateGly);
    TrendArrow = doc["data"][0]["glucoseItem"]["TrendArrow"].as<int8_t>();
    lastReceptionGlycMillis = millis();
    Serial.println();
    Serial.println("TrendArrow : " + String(TrendArrow));
    Serial.println();
    Serial.println("targetLow : " + String(targetLow));
    Serial.println("targetHigh : " + String(targetHigh));
    
    
    patientId = doc["data"][0]["patientId"].as<String>();

    const char *timestamp = doc["data"][0]["glucoseItem"]["Timestamp"].as<const char *>();
    lastGlyUnixTime = convertToUnix(timestamp);
    if (pointCountGly >= MAX_POINTS)
    {
      for (int i = 1; i < pointCountGly; i++)
      {
        glucoseValues[i - 1] = glucoseValues[i];
        glucoseHeure[i - 1] = glucoseHeure[i];
      }
      pointCountGly--;
    }
    glucoseValues[pointCountGly] = GlycemieVal;
    glucoseHeure[pointCountGly] = lastGlyUnixTime;
    pointCountGly++;
    lastGlycOkMillis = millis();
  }
  else
  {
    Serial.println("Erreur connexion gluco , code HTTP: " + String(httpCode));
    EcranPrintln(HEURE + " Connection (gluco) to libreview failed, HTTP code: " + String(httpCode),RGB565_ORANGE); 
   
  }

  https.end();
}

void getGraph()
{
  HTTPClient https;
  String url = String(baseURL) + "/llu/connections/" + patientId + "/graph"; //"/glucose-graph";

  https.begin(url);
  https.setTimeout(15000); // HTTPClient en ms

  https.addHeader("Content-Type", "application/json");
  https.addHeader("Accept", "application/json");
  // Mes rajouts
  https.addHeader("User-Agent", "okhttp/4.9.0");
  https.addHeader("connection", "Keep-Alive");
  https.addHeader("product", "llu.android");
  https.addHeader("version", "4.17.0");

  // Header d’authentification

  https.addHeader("Authorization", "Bearer " + AuthToken);

  https.addHeader("Account-Id", SHAuserID);

  int httpCode = https.GET();
  JsonDocument doc;
  if (httpCode == HTTP_CODE_OK)
  {
    String response = https.getString();
    Serial.println("Données graph: " +  String(response.length()) + " caractères");
    GraphJSON = response;
    JsonDocument filter; // Pour réduire taille en RAM
    filter["data"]["graphData"][0]["ValueInMgPerDl"] = true;
    filter["data"]["graphData"][0]["Timestamp"] = true;
    deserializeJson(doc, response, DeserializationOption::Filter(filter));
  }
  else
  {
    Serial.println(HEURE + " Connection (graph) to libreview failed, HTTP code: " + String(httpCode));
    EcranPrintln(HEURE + " Connection (graph) to libreview failed, HTTP code: " + String(httpCode),RGB565_ORANGE);
  }

  https.end();

  // Accès au tableau graphData
  JsonArray graphData = doc["data"]["graphData"];

  if (graphData.isNull())
  {
    Serial.println("graphData introuvable !");
    return;
  }
  int lastGly = 0;
  unsigned long lastGlyTime = 0;
  if (pointCountGly > 0)
  { // On déjà une valeur de glycémie récente à rajouter au graphe
    lastGly = glucoseValues[pointCountGly - 1];
    lastGlyTime = glucoseHeure[pointCountGly - 1];
  }

  pointCountGly = 0;
  for (JsonObject item : graphData)
  {
    int value = item["ValueInMgPerDl"];
    const char *timestamp = item["Timestamp"];
    unsigned long unixTime = convertToUnix(timestamp);
    Serial.println("Glucose: " + String(value) + " mg/dL, Timestamp: " + String(timestamp) + ", Unix Time: " + String(unixTime));
    if (pointCountGly + 1 < MAX_POINTS)
    {
      glucoseValues[pointCountGly] = value;
      glucoseHeure[pointCountGly] = unixTime;
      pointCountGly++;
    }
  }
  if (lastGlyTime > 0 && (lastGlyTime > glucoseHeure[pointCountGly - 1]))
  { // On rajoute la dernière valeur connue si elle est plus récente que la dernière du graphe
    glucoseValues[pointCountGly] = lastGly;
    glucoseHeure[pointCountGly] = lastGlyTime;
    pointCountGly++;
  }
  Serial.println("===== GRAPH DATA ===== de " + String(graphData.size()) + " points, pointCountGly: " + String(pointCountGly));
}

void LectureGlycemie()
{

  if (UserOK)

  {
    Serial.println("pointCountGly:" + String(pointCountGly));

    getConnection(); // Dernière valeur de glycémie
    if (pointCountGly < 3)
    {
      getGraph(); // Ancienne valeurs de glycémie
    }

    UserOK = false;
  }
  recurGlycMillis = RecurrenceGlycemie;
  if (AgeGlycemie>300) // Si la dernière glycémie a plus de 5 minutes (300s), on tente une nouvelle lecture
    recurGlycMillis = 30000; // 30 secondes pour tenter de récupérer une nouvelle glycémie plus rapidement
  if (AgeGlycemie>500) // On ne s'excite pas, on passe à 1.5 minute pour éviter de faire trop de requêtes si le serveur est en panne ou si on a un problème de connexion
    recurGlycMillis = 90000; 
  if (millis() - lastReceptionGlycMillis > recurGlycMillis || lastDemandeGlycMillis == 0)
  { 
    Serial.println("On demande une nouvelle glycémie...");
    lastDemandeGlycMillis = millis(); // Met à jour le temps dernière demande de glycémie avant de faire la lecture
    UserOK = loginLibreLinkUp();
    lastReceptionGlycMillis = millis(); // Met à jour le temps du dernier relevé reussi ou pas  de glycémie
  }
}

String getSHA256(String payload)
{
  byte shaResult[32];
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);

  // .c_str() convertit l'objet String en pointeur utilisable par mbedtls
  mbedtls_md_update(&ctx, (const unsigned char *)payload.c_str(), payload.length());

  mbedtls_md_finish(&ctx, shaResult);
  mbedtls_md_free(&ctx);

  // Conversion des 32 octets en une String Hexadécimale de 64 caractères
  String hashStr = "";
  for (int i = 0; i < 32; i++)
  {
    char str[3];
    sprintf(str, "%02x", (int)shaResult[i]);
    hashStr += str;
  }
  return hashStr;
}