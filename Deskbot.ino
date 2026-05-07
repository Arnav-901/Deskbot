// watch complete making video from here , please subscribe and support me https://youtu.be/3S44i9jbvqg?si=9gd9_DxMYLwjfb5E

// ==================================================
// ROBOVEDA
// ==================================================

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"
#include <math.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// ==================================================
// FORWARD DECLARATIONS
// (Raw string literals in handleConfigRoot confuse the Arduino IDE's
//  automatic prototype generator, causing 'Eye not declared' errors.
//  Manually declaring these here fixes it.)
// ==================================================
struct Eye;
void drawEyelidMask(float x, float y, float w, float h, int mood, bool isLeft);
void drawUltraProEye(Eye& e, bool isLeft);
void resetFlappyGame();
void updateMoodBasedOnWeather();
void startConfigPortal();
void getWeatherAndForecast();

// ==================================================
// 1. ASSETS & CONFIG
// ==================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 5
#define SCL_PIN 6
#define TOUCH_PIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- WEATHER ICONS ---
const unsigned char bmp_clear[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0xc0, 0x80, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x00, 0xff, 0xff, 0x00, 0x3e, 0xff, 0xff, 0x7c, 0x3e, 0xff, 0xff, 0x7c, 0x3e, 0xff, 0xff, 0x7c, 0x00, 0xff, 0xff, 0x00, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x00, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x01, 0x0f, 0xf0, 0x80, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_clouds[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0xff, 0xff, 0xc0, 0x00, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xfc, 0x07, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_rain[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0c, 0x00, 0x00, 0x60, 0x0c, 0x00, 0x00, 0xe0, 0x1c, 0x00, 0x00, 0xc0, 0x18, 0x00, 0x03, 0x80, 0x70, 0x00, 0x03, 0x80, 0x70, 0x00, 0x03, 0x00, 0x60, 0x00, 0x02, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char mini_sun[] PROGMEM = { 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x10, 0x08, 0x04, 0x20, 0x03, 0xc0, 0x27, 0xe4, 0x07, 0xe0, 0x07, 0xe0, 0x27, 0xe4, 0x03, 0xc0, 0x04, 0x20, 0x10, 0x08, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00 };
const unsigned char mini_cloud[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x1f, 0xf8, 0x3f, 0xfc, 0x3f, 0xfc, 0x7f, 0xfe, 0x3f, 0xfe, 0x1f, 0xfc, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char mini_rain[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x1f, 0xf8, 0x3f, 0xfc, 0x3f, 0xfc, 0x7f, 0xfe, 0x3f, 0xfe, 0x1f, 0xfc, 0x00, 0x00, 0x44, 0x44, 0x22, 0x22, 0x11, 0x11 };
const unsigned char bmp_tiny_drop[] PROGMEM = { 0x10, 0x38, 0x7c, 0xfe, 0xfe, 0x7c, 0x38, 0x00 };

// --- EMOTION PARTICLES (16x16) ---
const unsigned char bmp_heart[] PROGMEM = { 0x00, 0x00, 0x0c, 0x60, 0x1e, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 0x3f, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_zzz[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x30, 0x00, 0x7e, 0x00, 0x00, 0x3c, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x30, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_anger[] PROGMEM = { 0x00, 0x00, 0x11, 0x10, 0x2a, 0x90, 0x44, 0x40, 0x80, 0x20, 0x80, 0x20, 0x44, 0x40, 0x2a, 0x90, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// --- GLOBALS ---
// PAGE MAP:
// 0 = Eyes (Emo)
// 1 = Clock
// 2 = Weather
// 3 = Flappy Bird  
// 4 = World Clock  
// 5 = Forecast     
int currentPage = 0;
bool highBrightness = true;
int tapCounter = 0;
unsigned long lastTapTime = 0;
bool lastPinState = false;
unsigned long pressStartTime = 0;
bool isLongPressHandled = false;
const unsigned long LONG_PRESS_TIME = 800;
const unsigned long DOUBLE_TAP_DELAY = 300;
unsigned long lastPageSwitch = 0;
const unsigned long PAGE_INTERVAL = 8000;

// ==================================================
// FLAPPY BIRD GAME STATE
// ==================================================
#define PAGE_GAME 3

#define GRAVITY     0.06f
#define FLAP_VEL   -1.8f
#define PIPE_SPEED  1
#define PIPE_WIDTH  10
#define GAP_HEIGHT  30
#define BIRD_X      14
#define BIRD_RADIUS 3

// Game states
#define GAME_IDLE     0   // Waiting for hold-to-start
#define GAME_PLAYING  1   // Active gameplay
#define GAME_OVER     2   // Game over, show score, wait for hold-to-exit

int    gameState   = GAME_IDLE;
float  birdY       = 32.0f;
float  birdVel     = 0.0f;
int    pipeX       = 128;
int    pipeGapY    = 22;
int    score       = 0;
int    hiScore     = 0;

// Flash timer for game over blink
unsigned long gameOverFlashTime = 0;
bool gameOverFlashOn = false;

// Scroll offset for background parallax (simple ground scroll)
int groundScroll = 0;

void resetFlappyGame() {
  birdY     = 5.0f;
  birdVel   = 0.0f;
  pipeX     = 128;
  pipeGapY  = random(8, 28);
  score     = 0;
  groundScroll = 0;
}

// ==================================================
// MOODS
// ==================================================
#define MOOD_NORMAL    0
#define MOOD_HAPPY     1
#define MOOD_SURPRISED 2
#define MOOD_SLEEPY    3
#define MOOD_ANGRY     4
#define MOOD_SAD       5
#define MOOD_EXCITED   6
#define MOOD_LOVE      7
#define MOOD_SUSPICIOUS 8
int currentMood = MOOD_NORMAL;

String city;
String countryCode;
String apiKey;
String wifiSsid;
String wifiPass;
unsigned long lastWeatherUpdate = 0;
float temperature = 0.0;
float feelsLike = 0.0;
int humidity = 0;
String weatherMain = "Loading";
String weatherDesc = "Wait...";

struct ForecastDay {
  String dayName;
  int temp;
  String iconType;
};
ForecastDay fcast[3];
const char* ntpServer = "pool.ntp.org";
String tzString;

// ==================================================
// 2. ULTRA PRO PHYSICS ENGINE
// ==================================================

struct Eye {
  float x, y;
  float w, h;
  float targetX, targetY, targetW, targetH;
  float pupilX, pupilY;
  float targetPupilX, targetPupilY;
  float velX, velY, velW, velH;
  float pVelX, pVelY;
  float k = 0.12;
  float d = 0.60;
  float pk = 0.08;
  float pd = 0.50;
  bool blinking;
  unsigned long lastBlink;
  unsigned long nextBlinkTime;

  void init(float _x, float _y, float _w, float _h) {
    x = targetX = _x;
    y = targetY = _y;
    w = targetW = _w;
    h = targetH = _h;
    pupilX = targetPupilX = 0;
    pupilY = targetPupilY = 0;
    nextBlinkTime = millis() + random(1000, 4000);
  }

  void update() {
    float ax = (targetX - x) * k;
    float ay = (targetY - y) * k;
    float aw = (targetW - w) * k;
    float ah = (targetH - h) * k;
    velX = (velX + ax) * d;
    velY = (velY + ay) * d;
    velW = (velW + aw) * d;
    velH = (velH + ah) * d;
    x += velX;
    y += velY;
    w += velW;
    h += velH;
    float pax = (targetPupilX - pupilX) * pk;
    float pay = (targetPupilY - pupilY) * pk;
    pVelX = (pVelX + pax) * pd;
    pVelY = (pVelY + pay) * pd;
    pupilX += pVelX;
    pupilY += pVelY;
  }
};

Eye leftEye, rightEye;
unsigned long lastSaccade = 0;
unsigned long saccadeInterval = 3000;
float breathVal = 0;

// ==================================================
// 2b. CONFIG PORTAL
// ==================================================
#define CONFIG_AP_SSID  "deskbot"
#define CONFIG_AP_PASS  "12345678"
#define CONFIG_HOLD_MS  3000

Preferences prefs;
WebServer configServer(80);
bool inConfigMode = false;

void loadConfig() {
  prefs.begin("deskbot", true);
  wifiSsid    = prefs.getString("ssid", "");
  wifiPass    = prefs.getString("pass", "");
  apiKey      = prefs.getString("apikey", "");
  city        = prefs.getString("city", "");
  countryCode = prefs.getString("country", "");
  tzString    = prefs.getString("tz", "");
  prefs.end();
  if (wifiSsid.isEmpty()) {
    wifiSsid    = "";
    wifiPass    = "";
    apiKey      = "";
    city        = "";
    countryCode = "IN";
    tzString    = "IST-5:30";
  } else {
    if (apiKey.isEmpty())      apiKey      = "";
    if (city.isEmpty())        city        = "";
    if (countryCode.isEmpty()) countryCode = "IN";
    if (tzString.isEmpty())    tzString    = "IST-5:30";
  }
}

void saveConfig(const String& s, const String& p, const String& ak,
                const String& cty, const String& ctry, const String& tz) {
  prefs.begin("deskbuddy", false);
  prefs.putString("ssid", s);
  prefs.putString("pass", p);
  prefs.putString("apikey", ak);
  prefs.putString("city", cty);
  prefs.putString("country", ctry);
  prefs.putString("tz", tz);
  prefs.end();
}

void handleConfigRoot() {
  prefs.begin("deskbuddy", true);
  String sSsid = prefs.getString("ssid", "");
  String sApik = prefs.getString("apikey", "");
  String sCity = prefs.getString("city", "");
  String sCtry = prefs.getString("country", "IN");
  String sTz   = prefs.getString("tz", "IST-5:30");
  prefs.end();

  String html = R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
<title>DeskBuddy Config</title>
<style>
body{font-family:sans-serif;max-width:420px;margin:30px auto;padding:24px;background:#0c1929;color:#e8f4fc;}
h1{color:#5ba3f5;margin-bottom:8px;}
input{width:100%;padding:10px;margin:6px 0;border:1px solid #2d4a6f;border-radius:6px;box-sizing:border-box;background:#1a2d47;color:#e8f4fc;}
input:focus{outline:none;border-color:#5ba3f5;}
button{width:100%;padding:12px;background:#3498db;color:#fff;border:none;border-radius:6px;font-size:16px;cursor:pointer;margin-top:16px;}
button:hover{background:#2980b9;}
label{display:block;margin-top:14px;color:#8ab4e8;font-size:14px;}
.section{margin-top:20px;padding-top:16px;border-top:1px solid #1e3a5f;}
.section-title{color:#5ba3f5;font-size:13px;margin-bottom:8px;}
</style></head><body>
<h1>DeskBuddy Setup</h1>
<form action="/save" method="POST">
<label>WiFi SSID</label><input name="ssid" placeholder="Your WiFi name" value=")rawliteral";
  html += sSsid;
  html += R"rawliteral(">
<label>WiFi Password</label><input name="pass" type="password" placeholder="WiFi password">
<div class="section"><div class="section-title">Weather (OpenWeatherMap)</div>
<label>API Key</label><input name="apikey" placeholder="API key" value=")rawliteral";
  html += sApik;
  html += R"rawliteral(">
<label>City</label><input name="city" placeholder="e.g. London" value=")rawliteral";
  html += sCity;
  html += R"rawliteral(">
<label>Country Code</label><input name="country" placeholder="e.g. IN, US, GB" value=")rawliteral";
  html += sCtry;
  html += R"rawliteral(">
</div>
<div class="section"><div class="section-title">Time</div>
<label>Timezone</label><input name="tz" placeholder="e.g. IST-5:30, EST5EDT" value=")rawliteral";
  html += sTz;
  html += R"rawliteral(">
</div>
<button type="submit">Save &amp; Reboot</button>
</form></body></html>)rawliteral";
  configServer.send(200, "text/html", html);
}

void handleConfigSave() {
  if (!configServer.hasArg("ssid") || configServer.arg("ssid").length() == 0) {
    configServer.send(400, "text/plain", "SSID required");
    return;
  }
  String s   = configServer.arg("ssid");
  String p   = configServer.arg("pass");
  String ak  = configServer.arg("apikey");
  String cty = configServer.arg("city");
  String ctr = configServer.arg("country");
  String tz  = configServer.arg("tz");
  prefs.begin("deskbuddy", true);
  if (ak.isEmpty())  ak  = prefs.getString("apikey", "");
  if (cty.isEmpty()) cty = prefs.getString("city", "");
  if (ctr.isEmpty()) ctr = prefs.getString("country", "IN");
  if (tz.isEmpty())  tz  = prefs.getString("tz", "IST-5:30");
  prefs.end();
  saveConfig(s, p, ak, cty, ctr, tz);
  configServer.send(200, "text/html",
    "<html><body style='font-family:sans-serif;background:#0c1929;color:#e8f4fc;padding:40px;'>"
    "<h2 style='color:#5ba3f5'>Saved!</h2><p>Rebooting in 2 seconds...</p></body></html>");
  delay(2000);
  ESP.restart();
}

void startConfigPortal() {
  inConfigMode = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAP(CONFIG_AP_SSID, CONFIG_AP_PASS);
  configServer.on("/", handleConfigRoot);
  configServer.on("/save", HTTP_POST, handleConfigSave);
  configServer.begin();
  display.clearDisplay();
  display.setFont(NULL);
  display.setCursor(0, 0);
  display.print("Config mode\n\nConnect to:\n");
  display.print(CONFIG_AP_SSID);
  display.print("\n\nThen open:\n192.168.4.1");
  display.display();
}

// ==================================================
// 3. LOGIC & NETWORK
// ==================================================
const unsigned char* getBigIcon(String w) {
  if (w == "Clear")                    return bmp_clear;
  if (w == "Clouds")                   return bmp_clouds;
  if (w == "Rain" || w == "Drizzle")   return bmp_rain;
  return bmp_clouds;
}
const unsigned char* getMiniIcon(String w) {
  if (w == "Clear")                                           return mini_sun;
  if (w == "Rain" || w == "Drizzle" || w == "Thunderstorm")  return mini_rain;
  return mini_cloud;
}

void updateMoodBasedOnWeather() {
  int m = MOOD_NORMAL;
  if (weatherMain == "Clear")                              m = MOOD_HAPPY;
  else if (weatherMain == "Rain" || weatherMain == "Drizzle") m = MOOD_SAD;
  else if (weatherMain == "Thunderstorm")                  m = MOOD_SURPRISED;
  else if (weatherMain == "Clouds")                        m = MOOD_NORMAL;
  else if (temperature > 25)                               m = MOOD_EXCITED;
  else if (temperature < 5)                                m = MOOD_SLEEPY;
  currentMood = m;
}

// ==================================================
// TOUCH HANDLER
// Tap (short press)  → action depends on page
// Long press (>800ms) → action depends on page/game state
// ==================================================
void handleTouch() {
  bool currentPinState = digitalRead(TOUCH_PIN);
  unsigned long now = millis();

  // --- PRESS START ---
  if (currentPinState && !lastPinState) {
    pressStartTime = now;
    isLongPressHandled = false;
  }

  // --- HELD DOWN ---
  if (currentPinState && lastPinState) {
    if ((now - pressStartTime > LONG_PRESS_TIME) && !isLongPressHandled) {
      isLongPressHandled = true;
      lastPageSwitch = now;

      if (currentPage == PAGE_GAME) {
        // Long press controls game entry/exit
        if (gameState == GAME_IDLE) {
          // Start the game
          resetFlappyGame();
          gameState = GAME_PLAYING;
        } else if (gameState == GAME_OVER || gameState == GAME_PLAYING) {
          // Exit game back to weather page
          gameState = GAME_IDLE;
          currentPage = 2;  // Back to weather
        }
      } else if (currentPage == 0) {
        // On eyes page: long press cycles moods
        currentMood++;
        if (currentMood > MOOD_SUSPICIOUS) currentMood = 0;
        lastSaccade = 0;
      } else if (currentPage == 1) {
        currentPage = 4;  // Clock → World Clock
      } else if (currentPage == 2) {
        currentPage = 5;  // Weather → Forecast
      }
    }
  }

  // --- RELEASED ---
  if (!currentPinState && lastPinState) {
    if ((now - pressStartTime < LONG_PRESS_TIME) && !isLongPressHandled) {
      // Short tap registered
      tapCounter++;
      lastTapTime = now;
    }
  }

  lastPinState = currentPinState;

  // --- PROCESS TAPS after debounce window ---
  if (tapCounter > 0 && (now - lastTapTime > DOUBLE_TAP_DELAY)) {

    // On game page: behaviour depends on game state
    if (currentPage == PAGE_GAME) {
      if (gameState == GAME_PLAYING) {
        // ONLY flap — do not navigate anywhere
        birdVel = FLAP_VEL;
      } else if (gameState == GAME_OVER) {
        // Game over: tap resets back to idle screen (stay on page 3)
        gameState = GAME_IDLE;
      } else if (gameState == GAME_IDLE) {
        // Idle/title screen: single tap exits to page 0 (eyes)
        currentPage = 0;
      }
      // GAME_IDLE: tap goes to page 0 (eyes)
      tapCounter = 0;
      return;
    }

    lastPageSwitch = now;

    if (tapCounter >= 2) {
      // Double tap brightness toggle: ONLY on page 0 (eyes)
      if (currentPage == 0) {
        highBrightness = !highBrightness;
        display.ssd1306_command(SSD1306_SETCONTRAST);
        display.ssd1306_command(highBrightness ? 0xFF : 0x10);
      }
    } else {
      // Single tap: page navigation
      if (currentPage == 4) {
        currentPage = 1;  // World clock → Clock
      } else if (currentPage == 5) {
        currentPage = 2;  // Forecast → Weather
      } else {
        // Pages 0, 1, 2 cycle forward
        currentPage++;
        if (currentPage > 3) currentPage = 0;
        if (currentPage == PAGE_GAME) {
          gameState = GAME_IDLE;
        }
      }
    }
    tapCounter = 0;
  }
}

void getWeatherAndForecast() {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q="+city+",IN&appid="+apikey+"&units=metric";
  http.begin(url);
  if (http.GET() == 200) {
    String payload = http.getString();
    JSONVar myObject = JSON.parse(payload);
    if (JSON.typeof(myObject) != "undefined") {
      temperature  = double(myObject["main"]["temp"]);
      feelsLike    = double(myObject["main"]["feels_like"]);
      humidity     = int(myObject["main"]["humidity"]);
      weatherMain  = (const char*)myObject["weather"][0]["main"];
      weatherDesc  = (const char*)myObject["weather"][0]["description"];
      weatherDesc[0] = toupper(weatherDesc[0]);
      updateMoodBasedOnWeather();
    }
  }
  http.end();

  // Forecast (3-hourly)
  url = "http://api.openweathermap.org/data/2.5/forecast?q="+city+",IN&appid="+apikey+"&units=metric";
  http.begin(url);
  if (http.GET() == 200) {
    String payload = http.getString();
    JSONVar fo = JSON.parse(payload);
    if (JSON.typeof(fo) != "undefined") {
      struct tm t;
      getLocalTime(&t);
      int today = t.tm_wday;
      const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
      int indices[3] = { 7, 15, 23 };
      for (int i = 0; i < 3; i++) {
        int idx = indices[i];
        fcast[i].temp     = (int)double(fo["list"][idx]["main"]["temp"]);
        fcast[i].iconType = (const char*)fo["list"][idx]["weather"][0]["main"];
        fcast[i].dayName  = days[(today + i + 1) % 7];
      }
    }
  }
  http.end();
}

// ==================================================
// 4. DRAWING & ANIMATION
// ==================================================

void drawEyelidMask(float x, float y, float w, float h, int mood, bool isLeft) {
  int ix = (int)x, iy = (int)y, iw = (int)w, ih = (int)h;
  display.setTextColor(SSD1306_BLACK);
  if (mood == MOOD_ANGRY) {
    if (isLeft)
      for (int i = 0; i < 16; i++) display.drawLine(ix, iy + i, ix + iw, iy - 6 + i, SSD1306_BLACK);
    else
      for (int i = 0; i < 16; i++) display.drawLine(ix, iy - 6 + i, ix + iw, iy + i, SSD1306_BLACK);
  } else if (mood == MOOD_SAD) {
    if (isLeft)
      for (int i = 0; i < 16; i++) display.drawLine(ix, iy - 6 + i, ix + iw, iy + i, SSD1306_BLACK);
    else
      for (int i = 0; i < 16; i++) display.drawLine(ix, iy + i, ix + iw, iy - 6 + i, SSD1306_BLACK);
  } else if (mood == MOOD_HAPPY || mood == MOOD_LOVE || mood == MOOD_EXCITED) {
    display.fillRect(ix, iy + ih - 12, iw, 14, SSD1306_BLACK);
    display.fillCircle(ix + iw / 2, iy + ih + 6, (int)(iw / 1.3), SSD1306_BLACK);
  } else if (mood == MOOD_SLEEPY) {
    display.fillRect(ix, iy, iw, ih / 2 + 2, SSD1306_BLACK);
  } else if (mood == MOOD_SUSPICIOUS) {
    if (isLeft) display.fillRect(ix, iy, iw, ih / 2 - 2, SSD1306_BLACK);
    else        display.fillRect(ix, iy + ih - 8, iw, 8, SSD1306_BLACK);
  }
}

void drawUltraProEye(Eye& e, bool isLeft) {
  int ix = (int)e.x, iy = (int)e.y, iw = (int)e.w, ih = (int)e.h;
  int r = (iw < 20) ? 3 : 8;
  display.fillRoundRect(ix, iy, iw, ih, r, SSD1306_WHITE);
  int cx = ix + iw / 2;
  int cy = iy + ih / 2;
  int pw = iw / 2.2;
  int ph = ih / 2.2;
  int px = cx + (int)e.pupilX - (pw / 2);
  int py = cy + (int)e.pupilY - (ph / 2);
  if (px < ix)           px = ix;
  if (px + pw > ix + iw) px = ix + iw - pw;
  if (py < iy)           py = iy;
  if (py + ph > iy + ih) py = iy + ih - ph;
  display.fillRoundRect(px, py, pw, ph, r / 2, SSD1306_BLACK);
  if (iw > 15 && ih > 15)
    display.fillCircle(px + pw - 4, py + 4, 2, SSD1306_WHITE);
  drawEyelidMask(e.x, e.y, e.w, e.h, currentMood, isLeft);
}

void updatePhysicsAndMood() {
  unsigned long now = millis();
  breathVal = sin(now / 800.0) * 1.5;

  if (now > leftEye.nextBlinkTime) {
    leftEye.blinking = rightEye.blinking = true;
    leftEye.lastBlink = now;
    leftEye.nextBlinkTime = now + random(2000, 6000);
  }
  if (leftEye.blinking) {
    leftEye.targetH = rightEye.targetH = 2;
    if (now - leftEye.lastBlink > 120)
      leftEye.blinking = rightEye.blinking = false;
  }

  if (!leftEye.blinking && now - lastSaccade > saccadeInterval) {
    lastSaccade = now;
    saccadeInterval = random(500, 3000);
    int dir = random(0, 10);
    float lx = 0, ly = 0;
    if      (dir == 4) { lx = -6; ly = -4; }
    else if (dir == 5) { lx =  6; ly = -4; }
    else if (dir == 6) { lx = -6; ly =  4; }
    else if (dir == 7) { lx =  6; ly =  4; }
    else if (dir == 8) { lx =  8; ly =  0; }
    else if (dir == 9) { lx = -8; ly =  0; }
    leftEye.targetPupilX  = lx;
    leftEye.targetPupilY  = ly;
    rightEye.targetPupilX = lx;
    rightEye.targetPupilY = ly;
    leftEye.targetX  = 18 + (lx * 0.3);
    leftEye.targetY  = 14 + (ly * 0.3);
    rightEye.targetX = 74 + (lx * 0.3);
    rightEye.targetY = 14 + (ly * 0.3);
  }

  if (!leftEye.blinking) {
    float baseW = 36, baseH = 36 + breathVal;
    switch (currentMood) {
      case MOOD_NORMAL:
        leftEye.targetW = rightEye.targetW = baseW;
        leftEye.targetH = rightEye.targetH = baseH;
        break;
      case MOOD_HAPPY: case MOOD_LOVE:
        leftEye.targetW = rightEye.targetW = 40;
        leftEye.targetH = rightEye.targetH = 32;
        break;
      case MOOD_SURPRISED:
        leftEye.targetW = rightEye.targetW = 30;
        leftEye.targetH = rightEye.targetH = 45;
        leftEye.targetPupilX += random(-1, 2);
        break;
      case MOOD_SLEEPY:
        leftEye.targetW = rightEye.targetW = 38;
        leftEye.targetH = rightEye.targetH = 30;
        break;
      case MOOD_ANGRY:
        leftEye.targetW = rightEye.targetW = 34;
        leftEye.targetH = rightEye.targetH = 32;
        break;
      case MOOD_SAD:
        leftEye.targetW = rightEye.targetW = 34;
        leftEye.targetH = rightEye.targetH = 40;
        break;
      case MOOD_SUSPICIOUS:
        leftEye.targetW  = rightEye.targetW = 36;
        leftEye.targetH  = 20;
        rightEye.targetH = 42;
        break;
    }
  }
  leftEye.update();
  rightEye.update();
}

void drawEmoPage() {
  updatePhysicsAndMood();
  if (currentMood == MOOD_LOVE)
    display.drawBitmap(56, 0, bmp_heart, 16, 16, SSD1306_WHITE);
  else if (currentMood == MOOD_SLEEPY)
    display.drawBitmap(110, 0, bmp_zzz, 16, 16, SSD1306_WHITE);
  else if (currentMood == MOOD_ANGRY)
    display.drawBitmap(56, 0, bmp_anger, 16, 16, SSD1306_WHITE);
  drawUltraProEye(leftEye, true);
  drawUltraProEye(rightEye, false);
}

// ==================================================
// FLAPPY BIRD DRAW
// ==================================================
void drawFlappyGame() {
  display.setFont(NULL);
  display.setTextColor(SSD1306_WHITE);

  // ---- IDLE SCREEN ----
  if (gameState == GAME_IDLE) {
    // Draw a static bird preview
    display.fillCircle(BIRD_X, 32, BIRD_RADIUS + 1, SSD1306_WHITE);
    display.fillCircle(BIRD_X + 2, 30, 1, SSD1306_BLACK);  // Eye
    // Wing
    display.drawLine(BIRD_X - 2, 33, BIRD_X - 5, 36, SSD1306_WHITE);

    // Ground line
    display.drawLine(0, 60, 128, 60, SSD1306_WHITE);

    display.setCursor(28, 10);
    display.print("FLAPPY BUDDY");
    display.drawLine(28, 18, 100, 18, SSD1306_WHITE);
    display.setCursor(18, 28);
    display.print("Hold = Start");
    display.setCursor(18, 40);
    display.print("Tap  = Flap");
    if (hiScore > 0) {
      display.setCursor(18, 52);
      display.print("Best:");
      display.print(hiScore);
    }
    return;
  }

  // ---- PLAYING ----
  if (gameState == GAME_PLAYING) {
    // Physics
    birdVel += GRAVITY;
    birdY   += birdVel;

    // Pipe movement
    pipeX -= PIPE_SPEED;
    if (pipeX < -PIPE_WIDTH) {
      pipeX    = 128;
      pipeGapY = random(8, 36);
      score++;
    }

    // Ground scroll
    groundScroll = (groundScroll + 1) % 8;

    // --- Collision detection ---
    // Walls
    if ((int)birdY - BIRD_RADIUS < 0 || (int)birdY + BIRD_RADIUS > 59) {
      gameState = GAME_OVER;
      if (score > hiScore) hiScore = score;
      gameOverFlashTime = millis();
    }
    // Pipe (bird is a circle at BIRD_X, birdY radius BIRD_RADIUS)
    if (pipeX < BIRD_X + BIRD_RADIUS && pipeX + PIPE_WIDTH > BIRD_X - BIRD_RADIUS) {
      if ((int)birdY - BIRD_RADIUS < pipeGapY ||
          (int)birdY + BIRD_RADIUS > pipeGapY + GAP_HEIGHT) {
        gameState = GAME_OVER;
        if (score > hiScore) hiScore = score;
        gameOverFlashTime = millis();
      }
    }
  }

  // ---- DRAW (PLAYING + GAME_OVER share the same scene) ----

  // Ground with scroll ticks
  display.drawLine(0, 60, 128, 60, SSD1306_WHITE);
  for (int gx = (8 - groundScroll) % 8; gx < 128; gx += 8)
    display.drawLine(gx, 60, gx + 4, 63, SSD1306_WHITE);

  // Pipes - top pipe
  display.fillRect(pipeX, 0, PIPE_WIDTH, pipeGapY, SSD1306_WHITE);
  // Pipe cap top
  display.fillRect(pipeX - 1, pipeGapY - 4, PIPE_WIDTH + 2, 4, SSD1306_WHITE);
  // Pipes - bottom pipe
  display.fillRect(pipeX, pipeGapY + GAP_HEIGHT, PIPE_WIDTH,
                   60 - (pipeGapY + GAP_HEIGHT), SSD1306_WHITE);
  // Pipe cap bottom
  display.fillRect(pipeX - 1, pipeGapY + GAP_HEIGHT, PIPE_WIDTH + 2, 4, SSD1306_WHITE);

  // Bird body
  display.fillCircle(BIRD_X, (int)birdY, BIRD_RADIUS + 1, SSD1306_WHITE);
  display.fillCircle(BIRD_X + 2, (int)birdY - 1, 1, SSD1306_BLACK);  // Eye
  // Wing flap based on velocity
  int wingY = (birdVel > 0) ? (int)birdY + 2 : (int)birdY - 2;
  display.drawLine(BIRD_X - 2, (int)birdY + 1, BIRD_X - 5, wingY, SSD1306_WHITE);

  // Score
  display.setCursor(2, 0);
  display.print(score);

  // ---- GAME OVER OVERLAY ----
  if (gameState == GAME_OVER) {
    // Blink "GAME OVER" box
    unsigned long now = millis();
    if (now - gameOverFlashTime > 400) {
      gameOverFlashOn = !gameOverFlashOn;
      gameOverFlashTime = now;
    }
    if (gameOverFlashOn) {
      display.fillRect(24, 22, 80, 20, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setCursor(30, 26);
      display.print("GAME OVER");
      display.setCursor(32, 34);
      display.print("Score:");
      display.print(score);
      display.setTextColor(SSD1306_WHITE);
    }
    display.setCursor(14, 52);
    display.print("Hold to exit");
  }
}

// --- STANDARD PAGES ---
void drawForecastPage() {
  display.fillRect(0, 0, 128, 16, SSD1306_WHITE);
  display.setFont(NULL);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(20, 4);
  display.print("3-DAY FORECAST");
  display.setTextColor(SSD1306_WHITE);
  display.drawLine(42, 16, 42, 64, SSD1306_WHITE);
  display.drawLine(85, 16, 85, 64, SSD1306_WHITE);
  for (int i = 0; i < 3; i++) {
    int xStart  = i * 43;
    int centerX = xStart + 21;
    display.setFont(NULL);
    String d = fcast[i].dayName;
    if (d == "") d = "Wait";
    display.setCursor(centerX - (d.length() * 3), 20);
    display.print(d);
    display.drawBitmap(centerX - 8, 28, getMiniIcon(fcast[i].iconType), 16, 16, SSD1306_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    int16_t x1, y1; uint16_t w, h;
    display.getTextBounds(String(fcast[i].temp).c_str(), 0, 0, &x1, &y1, &w, &h);
    display.setCursor(centerX - (w / 2) - 2, 60);
    display.print(fcast[i].temp);
    display.fillCircle(centerX + (w / 2) + 1, 52, 2, SSD1306_WHITE);
  }
}

void drawClock() {
  struct tm t;
  if (!getLocalTime(&t)) {
    display.setFont(NULL);
    display.setCursor(30, 30);
    display.print("Syncing...");
    return;
  }
  String ampm = (t.tm_hour >= 12) ? "PM" : "AM";
  int h12 = t.tm_hour % 12;
  if (h12 == 0) h12 = 12;
  display.setTextColor(SSD1306_WHITE);
  display.setFont(NULL);
  display.setTextSize(1);
  display.setCursor(114, 0);
  display.print(ampm);
  display.setFont(&FreeSansBold18pt7b);
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", h12, t.tm_min);
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 42);
  display.print(timeStr);
  display.setFont(&FreeSans9pt7b);
  char dateStr[20];
  strftime(dateStr, 20, "%a, %b %d", &t);
  display.getTextBounds(dateStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 62);
  display.print(dateStr);
}

void drawWeatherCard() {
  if (WiFi.status() != WL_CONNECTED) {
    display.setFont(NULL);
    display.setCursor(0, 0);
    display.print("No WiFi");
    return;
  }
  display.drawBitmap(96, 0, getBigIcon(weatherMain), 32, 32, SSD1306_WHITE);
  display.setFont(&FreeSansBold9pt7b);
  String c = city;
  c.toUpperCase();
  display.setCursor(0, 14);
  if (c.length() > 9) c = c.substring(0, 8) + ".";
  display.print(c);
  display.setFont(&FreeSansBold18pt7b);
  int tempInt = (int)temperature;
  display.setCursor(0, 48);
  display.print(tempInt);
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(String(tempInt).c_str(), 0, 48, &x1, &y1, &w, &h);
  display.fillCircle(x1 + w + 5, 26, 4, SSD1306_WHITE);
  display.setFont(NULL);
  display.drawBitmap(88, 32, bmp_tiny_drop, 8, 8, SSD1306_WHITE);
  display.setCursor(100, 32);
  display.print(humidity);
  display.print("%");
  display.setCursor(88, 45);
  display.print("~");
  display.print((int)feelsLike);
  display.drawLine(0, 52, 128, 52, SSD1306_WHITE);
  display.setCursor(0, 55);
  display.print(weatherDesc);
}

void drawWorldClock() {
  time_t now;
  time(&now);
  time_t indiaEpoch  = now + (5 * 3600) + (30 * 60);
  time_t sydneyEpoch = now + (11 * 3600);
  struct tm* indiatm  = gmtime(&indiaEpoch);
  struct tm* sydneytm = gmtime(&sydneyEpoch);
  display.fillRect(0, 0, 128, 16, SSD1306_WHITE);
  display.setFont(NULL);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(32, 4);
  display.print("WORLD CLOCK");
  display.setTextColor(SSD1306_WHITE);
  display.drawLine(64, 18, 64, 54, SSD1306_WHITE);
  display.setFont(NULL);
  display.setCursor(16, 22);
  display.print("INDIA");
  display.setFont(&FreeSansBold9pt7b);
  char iStr[10];
  sprintf(iStr, "%02d:%02d", indiatm->tm_hour, indiatm->tm_min);
  display.setCursor(5, 46);
  display.print(iStr);
  display.setFont(NULL);
  display.setCursor(78, 22);
  display.print("SYDNEY");
  display.setFont(&FreeSansBold9pt7b);
  char sStr[10];
  sprintf(sStr, "%02d:%02d", sydneytm->tm_hour, sydneytm->tm_min);
  display.setCursor(69, 46);
  display.print(sStr);
  display.setFont(NULL);
  display.setCursor(35, 56);
  display.print("Tap to Exit");
}

// ==================================================
// 5. BOOT & MAIN
// ==================================================

void playBootAnimation() {
  display.setTextColor(SSD1306_WHITE);
  int cx = 64, cy = 32;
  for (int r = 0; r < 80; r += 4) {
    display.clearDisplay();
    display.fillCircle(cx, cy, r, SSD1306_WHITE);
    display.display();
    delay(10);
  }
  for (int r = 0; r < 80; r += 4) {
    display.clearDisplay();
    display.fillCircle(cx, cy, 80, SSD1306_WHITE);
    display.fillCircle(cx, cy, r, SSD1306_BLACK);
    display.display();
    delay(10);
  }
  display.setFont(&FreeSansBold9pt7b);
  String bootText = "ROBOVEDA";
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(bootText, 0, 0, &x1, &y1, &w, &h);
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - w) / 2, 36);
  display.print(bootText);
  display.display();
  delay(2000);
}

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(TOUCH_PIN, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.setTextColor(SSD1306_WHITE);

  bool forceConfig = false;
  for (unsigned long t = millis(); millis() - t < CONFIG_HOLD_MS; ) {
    if (digitalRead(TOUCH_PIN)) { forceConfig = true; break; }
    delay(80);
  }

  loadConfig();

  if (forceConfig) {
    startConfigPortal();
    return;
  }

  leftEye.init(18, 14, 36, 36);
  rightEye.init(74, 14, 36, 36);

  playBootAnimation();

  display.clearDisplay();
  display.setFont(NULL);
  display.setCursor(40, 30);
  display.print("connecting");
  display.display();
  WiFi.begin(wifiSsid.c_str(), wifiPass.c_str());
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - wifiStart < 15000))
    delay(200);

  if (WiFi.status() != WL_CONNECTED) {
    startConfigPortal();
    return;
  }

  configTime(0, 0, ntpServer);
  setenv("TZ", tzString.c_str(), 1);
  tzset();
  getWeatherAndForecast();
  lastWeatherUpdate = millis();
  lastPageSwitch    = millis();

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (inConfigMode) {
    configServer.handleClient();
    return;
  }

  unsigned long now = millis();
  handleTouch();

  if (now - lastWeatherUpdate > 600000) {
    getWeatherAndForecast();
    lastWeatherUpdate = now;
  }

  display.clearDisplay();

  // PAGE MAP:
  // 0 = Eyes   1 = Clock   2 = Weather   3 = Flappy   4 = WorldClock   5 = Forecast
  switch (currentPage) {
    case 0: drawEmoPage();      break;
    case 1: drawClock();        break;
    case 2: drawWeatherCard();  break;
    case 3: drawFlappyGame();   break;
    case 4: drawWorldClock();   break;
    case 5: drawForecastPage(); break;
  }

  display.display();
}
