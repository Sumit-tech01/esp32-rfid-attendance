/*
  ============================================================
  ESP32 RFID ATTENDANCE SYSTEM
  PHASE 5 - MAKE.COM INTEGRATION
  ============================================================

  FEATURES:
  - Any RFID card is accepted
  - No registered card database
  - Each RFID UID has its own CLOCK IN / CLOCK OUT state
  - Same card can be scanned unlimited times
  - Every scan is sent to Make.com
  - Every scan should create a new Google Sheets row
  - No duplicate protection
  - OLED display
  - LED + buzzer feedback
  - ESP32 Arduino Core 3.x LEDC API
  - Optional song/beat demo preserved

  RFID:
    SS   -> GPIO 5
    RST  -> GPIO 27
    SCK  -> GPIO 18
    MISO -> GPIO 19
    MOSI -> GPIO 23
    VCC  -> 3.3V
    GND  -> GND

  OLED:
    SDA -> GPIO 21
    SCL -> GPIO 22
    VCC -> 3.3V
    GND -> GND

  Buzzer:
    I/O -> GPIO 4
    VCC -> 3.3V
    GND -> GND

  Onboard LED:
    GPIO 2
*/

// ============================================================
// LIBRARIES
// ============================================================
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <SPI.h>
#include <MFRC522.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// ============================================================
// WIFI CONFIGURATION
// ============================================================

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";


// ============================================================
// MAKE.COM WEBHOOK
// ============================================================

const char* MAKE_WEBHOOK_URL =
  "YOUR_MAKE_WEBHOOK_URL";

// ============================================================
// DEVICE INFORMATION
// ============================================================

const char* DEVICE_NAME = "ESP32-RFID-01";


// ============================================================
// PIN CONFIGURATION
// ============================================================

#define LED_PIN 2

#define BUZZER_PIN 4

#define OLED_SDA 21
#define OLED_SCL 22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

#define RFID_SS   5
#define RFID_RST  27
#define RFID_SCK  18
#define RFID_MISO 19
#define RFID_MOSI 23

// ============================================================
// HARDWARE OBJECTS
// ============================================================

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

MFRC522 rfid(
  RFID_SS,
  RFID_RST
);


// ============================================================
// BUZZER SETTINGS
// ============================================================

// ESP32 Arduino Core 3.x

#define BUZZER_FREQUENCY 2000
#define BUZZER_RESOLUTION 8


// ============================================================
// MULTI-CARD STATE DATABASE
// ============================================================

// IMPORTANT:
//
// There is NO registered-card restriction.
//
// Every new UID is automatically accepted.
//
// Each UID gets its own state:
//
// New card:
//   CLOCK IN
//
// Same card again:
//   CLOCK OUT
//
// Again:
//   CLOCK IN
//
// Again:
//   CLOCK OUT
//

struct CardState {

  String uid;

  bool isClockedIn;

  unsigned long clockInTime;

};


// Maximum number of different cards remembered
// during the current ESP32 power session.

const int MAX_CARDS = 50;

CardState cardStates[MAX_CARDS];

int cardCount = 0;


// ============================================================
// FUNCTION DECLARATIONS
// ============================================================

String getUIDString();

int findCardState(String uid);

int createCardState(String uid);

void handleRFIDScan(String uid);

String formatDuration(unsigned long milliseconds);

void showReadyScreen();

void showMessage(
  String line1,
  String line2
);

void showWiFiStatus();

void feedback(int count);

void startupAnimation();

void playSongDemo();

void playBeat(
  int frequency,
  int duration
);

void connectWiFi();

bool sendEventToMake(
  String uid,
  String eventType
);


// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("==============================================");
  Serial.println("       ESP32 RFID ATTENDANCE SYSTEM");
  Serial.println("              PHASE 5");
  Serial.println("          MAKE.COM INTEGRATION");
  Serial.println("==============================================");
  Serial.println();


  // ==========================================================
  // LED
  // ==========================================================

  pinMode(
    LED_PIN,
    OUTPUT
  );

  digitalWrite(
    LED_PIN,
    LOW
  );


  // ==========================================================
  // OLED
  // ==========================================================

  Serial.println("Initializing OLED...");

  Wire.begin(
    OLED_SDA,
    OLED_SCL
  );

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS
      )) {

    Serial.println("ERROR: OLED NOT FOUND!");

  } else {

    Serial.println("OLED: OK");

    startupAnimation();
  }


  // ==========================================================
  // BUZZER
  // ==========================================================

  Serial.println();
  Serial.println("Initializing buzzer...");

  bool buzzerAttached = ledcAttach(
    BUZZER_PIN,
    BUZZER_FREQUENCY,
    BUZZER_RESOLUTION
  );

  if (!buzzerAttached) {

    Serial.println(
      "ERROR: BUZZER LEDC ATTACH FAILED!"
    );

  } else {

    Serial.println("BUZZER: OK");

    // Startup beep

    digitalWrite(
      LED_PIN,
      HIGH
    );

    ledcWriteTone(
      BUZZER_PIN,
      BUZZER_FREQUENCY
    );

    delay(200);

    ledcWriteTone(
      BUZZER_PIN,
      0
    );

    digitalWrite(
      LED_PIN,
      LOW
    );
  }


  // ==========================================================
  // RFID
  // ==========================================================

  Serial.println();
  Serial.println("Initializing RC522 RFID...");

  SPI.begin(
    RFID_SCK,
    RFID_MISO,
    RFID_MOSI,
    RFID_SS
  );

  rfid.PCD_Init();

  delay(100);

  Serial.println("RFID: OK");


  // ==========================================================
  // WIFI
  // ==========================================================

  connectWiFi();


  // ==========================================================
  // SYSTEM READY
  // ==========================================================

  showReadyScreen();

  Serial.println();
  Serial.println("==============================================");
  Serial.println("              SYSTEM READY");
  Serial.println("==============================================");
  Serial.println();
  Serial.println("Any RFID card is accepted.");
  Serial.println("Every scan is sent to Make.com.");
  Serial.println();
}


// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // Keep WiFi connected
  // ----------------------------------------------------------

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi disconnected.");

    connectWiFi();
  }


  // ----------------------------------------------------------
  // Check for new RFID card
  // ----------------------------------------------------------

  if (!rfid.PICC_IsNewCardPresent()) {

    return;
  }


  // ----------------------------------------------------------
  // Read RFID card
  // ----------------------------------------------------------

  if (!rfid.PICC_ReadCardSerial()) {

    return;
  }


  // ----------------------------------------------------------
  // Get UID
  // ----------------------------------------------------------

  String uid = getUIDString();


  Serial.println();
  Serial.println("----------------------------------------------");

  Serial.print(
    "RFID UID: "
  );

  Serial.println(uid);


  // ----------------------------------------------------------
  // Handle card
  // ----------------------------------------------------------

  handleRFIDScan(uid);


  // ----------------------------------------------------------
  // Stop RFID communication
  // ----------------------------------------------------------

  rfid.PICC_HaltA();

  rfid.PCD_StopCrypto1();


  // ----------------------------------------------------------
  // RFID debounce
  // ----------------------------------------------------------
  //
  // This does NOT prevent legitimate future scans.
  //
  // It only prevents the same physical tap from being
  // detected repeatedly within a very short time.
  //

  delay(1000);
}


// ============================================================
// GET RFID UID
// ============================================================

String getUIDString() {

  String result = "";


  for (
    byte i = 0;
    i < rfid.uid.size;
    i++
  ) {

    if (
      rfid.uid.uidByte[i] < 0x10
    ) {

      result += "0";
    }

    result += String(
      rfid.uid.uidByte[i],
      HEX
    );
  }


  result.toUpperCase();

  return result;
}


// ============================================================
// FIND CARD STATE
// ============================================================

int findCardState(
  String uid
) {

  for (
    int i = 0;
    i < cardCount;
    i++
  ) {

    if (
      cardStates[i].uid == uid
    ) {

      return i;
    }
  }


  return -1;
}


// ============================================================
// CREATE NEW CARD STATE
// ============================================================

int createCardState(
  String uid
) {

  if (
    cardCount >= MAX_CARDS
  ) {

    Serial.println(
      "ERROR: Maximum card limit reached."
    );

    return -1;
  }


  cardStates[cardCount].uid = uid;

  cardStates[cardCount].isClockedIn = false;

  cardStates[cardCount].clockInTime = 0;


  int newIndex = cardCount;

  cardCount++;


  Serial.print(
    "New card added to memory: "
  );

  Serial.println(uid);


  return newIndex;
}


// ============================================================
// HANDLE RFID SCAN
// ============================================================

void handleRFIDScan(
  String uid
) {

  // ----------------------------------------------------------
  // Find existing card
  // ----------------------------------------------------------

  int cardIndex = findCardState(uid);


  // ----------------------------------------------------------
  // If new card, create automatically
  // ----------------------------------------------------------

  if (
    cardIndex == -1
  ) {

    cardIndex = createCardState(uid);

    if (
      cardIndex == -1
    ) {

      showMessage(
        "MEMORY FULL",
        "TRY AGAIN"
      );

      feedback(3);

      delay(1800);

      showReadyScreen();

      return;
    }
  }


  // Get reference

  CardState &card =
    cardStates[cardIndex];


  // ==========================================================
  // CLOCK IN
  // ==========================================================

  if (
    !card.isClockedIn
  ) {

    // Store clock-in state

    card.isClockedIn = true;


    // Store runtime

    card.clockInTime = millis();


    Serial.println(
      "STATUS: CLOCK IN"
    );

    Serial.print(
      "UID: "
    );

    Serial.println(uid);


    Serial.println(
      "NAME: RFID User"
    );


    // --------------------------------------------------------
    // OLED
    // --------------------------------------------------------

    showMessage(
      "CARD ACCEPTED",
      "CLOCKED IN"
    );


    // --------------------------------------------------------
    // Feedback
    // --------------------------------------------------------

    feedback(1);


    // --------------------------------------------------------
    // Send to Make.com
    // --------------------------------------------------------

    bool sent = sendEventToMake(
      uid,
      "CLOCK_IN"
    );


    if (sent) {

      Serial.println(
        "MAKE: CLOCK IN SENT"
      );

    } else {

      Serial.println(
        "MAKE: CLOCK IN FAILED"
      );
    }


    delay(1800);

    showReadyScreen();

    return;
  }


  // ==========================================================
  // CLOCK OUT
  // ==========================================================

  else {

    // Calculate duration

    unsigned long durationMs =
      millis() -
      card.clockInTime;


    // Format duration

    String duration =
      formatDuration(
        durationMs
      );


    // Change state

    card.isClockedIn = false;

    card.clockInTime = 0;


    Serial.println(
      "STATUS: CLOCK OUT"
    );

    Serial.print(
      "UID: "
    );

    Serial.println(uid);


    Serial.println(
      "NAME: RFID User"
    );


    Serial.print(
      "DURATION: "
    );

    Serial.println(duration);


    // --------------------------------------------------------
    // OLED
    // --------------------------------------------------------

    showMessage(
      "CARD ACCEPTED",
      "OUT " + duration
    );


    // --------------------------------------------------------
    // Feedback
    // --------------------------------------------------------

    feedback(2);


    // --------------------------------------------------------
    // Send to Make.com
    // --------------------------------------------------------

    bool sent = sendEventToMake(
      uid,
      "CLOCK_OUT"
    );


    if (sent) {

      Serial.println(
        "MAKE: CLOCK OUT SENT"
      );

    } else {

      Serial.println(
        "MAKE: CLOCK OUT FAILED"
      );
    }


    delay(2200);

    showReadyScreen();

    return;
  }
}


// ============================================================
// SEND EVENT TO MAKE.COM
// ============================================================

bool sendEventToMake(
  String uid,
  String eventType
) {

  // ----------------------------------------------------------
  // Check WiFi
  // ----------------------------------------------------------

  if (
    WiFi.status() != WL_CONNECTED
  ) {

    Serial.println(
      "ERROR: WiFi not connected."
    );

    return false;
  }


  // ----------------------------------------------------------
  // Check webhook URL
  // ----------------------------------------------------------

  if (
    String(MAKE_WEBHOOK_URL) ==
    "PASTE_YOUR_NEW_MAKE_WEBHOOK_URL_HERE"
  ) {

    Serial.println(
      "ERROR: Make webhook URL not configured."
    );

    return false;
  }


  // ----------------------------------------------------------
  // JSON payload
  // ----------------------------------------------------------

  JsonDocument doc;

  doc["uid"] = uid;

  doc["name"] = "RFID User";

  doc["event_type"] = eventType;

  doc["device"] = DEVICE_NAME;


  String jsonPayload;

  serializeJson(
    doc,
    jsonPayload
  );


  Serial.println();
  Serial.println(
    "Sending to Make.com..."
  );

  Serial.print(
    "Payload: "
  );

  Serial.println(
    jsonPayload
  );


  // ----------------------------------------------------------
  // HTTP
  // ----------------------------------------------------------

  HTTPClient http;


  http.begin(
    MAKE_WEBHOOK_URL
  );


  http.addHeader(
    "Content-Type",
    "application/json"
  );


  int httpCode =
    http.POST(
      jsonPayload
    );


  Serial.print(
    "HTTP Response: "
  );

  Serial.println(
    httpCode
  );


  bool success = false;


  // Any 2xx response = success

  if (
    httpCode >= 200 &&
    httpCode < 300
  ) {

    success = true;

    Serial.println(
      "MAKE: SUCCESS"
    );

  } else {

    Serial.println(
      "MAKE: FAILED"
    );

    if (
      httpCode > 0
    ) {

      Serial.print(
        "Response: "
      );

      Serial.println(
        http.getString()
      );
    }
  }


  http.end();


  return success;
}


// ============================================================
// WIFI CONNECTION
// ============================================================

void connectWiFi() {

  Serial.println();
  Serial.println(
    "Connecting to WiFi..."
  );


  WiFi.mode(
    WIFI_STA
  );


  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );


  int attempts = 0;


  while (
    WiFi.status() != WL_CONNECTED &&
    attempts < 30
  ) {

    delay(500);

    Serial.print(
      "."
    );

    attempts++;
  }


  Serial.println();


  if (
    WiFi.status() == WL_CONNECTED
  ) {

    Serial.println(
      "WiFi: CONNECTED"
    );

    Serial.print(
      "IP Address: "
    );

    Serial.println(
      WiFi.localIP()
    );

    Serial.print(
      "Signal RSSI: "
    );

    Serial.println(
      WiFi.RSSI()
    );

  } else {

    Serial.println(
      "WiFi: CONNECTION FAILED"
    );
  }
}


// ============================================================
// FORMAT DURATION
// ============================================================

String formatDuration(
  unsigned long milliseconds
) {

  unsigned long totalSeconds =
    milliseconds / 1000;


  unsigned long hours =
    totalSeconds / 3600;


  unsigned long minutes =
    (totalSeconds % 3600) / 60;


  unsigned long seconds =
    totalSeconds % 60;


  String result = "";


  if (
    hours > 0
  ) {

    result += String(hours);

    result += "h ";
  }


  result += String(minutes);

  result += "m ";


  result += String(seconds);

  result += "s";


  return result;
}


// ============================================================
// OLED READY SCREEN
// ============================================================

void showReadyScreen() {

  display.clearDisplay();

  display.setTextColor(
    SSD1306_WHITE
  );


  // ----------------------------------------------------------
  // Title
  // ----------------------------------------------------------

  display.setTextSize(2);

  display.setCursor(
    17,
    3
  );

  display.println(
    "RFID"
  );


  // ----------------------------------------------------------
  // Subtitle
  // ----------------------------------------------------------

  display.setTextSize(1);

  display.setCursor(
    18,
    28
  );

  display.println(
    "ATTENDANCE"
  );


  display.setCursor(
    22,
    42
  );

  display.println(
    "SYSTEM READY"
  );


  display.setCursor(
    22,
    55
  );

  display.println(
    "SCAN CARD"
  );


  display.display();
}


// ============================================================
// OLED MESSAGE
// ============================================================

void showMessage(
  String line1,
  String line2
) {

  display.clearDisplay();

  display.setTextColor(
    SSD1306_WHITE
  );


  // ----------------------------------------------------------
  // First line
  // ----------------------------------------------------------

  display.setTextSize(1);


  int16_t x1;
  int16_t y1;

  uint16_t width;
  uint16_t height;


  display.getTextBounds(
    line1,
    0,
    0,
    &x1,
    &y1,
    &width,
    &height
  );


  int xPosition =
    (SCREEN_WIDTH - width) / 2;


  if (
    xPosition < 0
  ) {

    xPosition = 0;
  }


  display.setCursor(
    xPosition,
    10
  );


  display.println(
    line1
  );


  // ----------------------------------------------------------
  // Second line
  // ----------------------------------------------------------

  display.getTextBounds(
    line2,
    0,
    0,
    &x1,
    &y1,
    &width,
    &height
  );


  xPosition =
    (SCREEN_WIDTH - width) / 2;


  if (
    xPosition < 0
  ) {

    xPosition = 0;
  }


  display.setCursor(
    xPosition,
    36
  );


  display.println(
    line2
  );


  display.display();
}


// ============================================================
// WIFI STATUS
// ============================================================

void showWiFiStatus() {

  if (
    WiFi.status() == WL_CONNECTED
  ) {

    showMessage(
      "WIFI CONNECTED",
      "MAKE READY"
    );

  } else {

    showMessage(
      "WIFI ERROR",
      "CHECK NETWORK"
    );
  }
}


// ============================================================
// SYNCHRONIZED FEEDBACK
// ============================================================
//
// 1 = CLOCK IN
// 2 = CLOCK OUT
// 3 = ERROR
//
// LED + BUZZER happen together.
//

void feedback(
  int count
) {

  for (
    int i = 0;
    i < count;
    i++
  ) {

    // --------------------------------------------------------
    // ON
    // --------------------------------------------------------

    digitalWrite(
      LED_PIN,
      HIGH
    );


    ledcWriteTone(
      BUZZER_PIN,
      BUZZER_FREQUENCY
    );


    delay(180);


    // --------------------------------------------------------
    // OFF
    // --------------------------------------------------------

    ledcWriteTone(
      BUZZER_PIN,
      0
    );


    digitalWrite(
      LED_PIN,
      LOW
    );


    delay(150);
  }
}


// ============================================================
// STARTUP ANIMATION
// ============================================================

void startupAnimation() {

  // ----------------------------------------------------------
  // Screen 1
  // ----------------------------------------------------------

  display.clearDisplay();

  display.setTextColor(
    SSD1306_WHITE
  );


  display.setTextSize(2);

  display.setCursor(
    20,
    5
  );

  display.println(
    "ESP32"
  );


  display.setTextSize(1);

  display.setCursor(
    20,
    32
  );

  display.println(
    "RFID SYSTEM"
  );


  display.setCursor(
    20,
    48
  );

  display.println(
    "PHASE 5"
  );


  display.display();


  delay(1200);


  // ----------------------------------------------------------
  // Screen 2
  // ----------------------------------------------------------

  display.clearDisplay();


  display.setTextSize(2);

  display.setCursor(
    12,
    10
  );

  display.println(
    "LOADING"
  );


  display.setTextSize(1);

  display.setCursor(
    22,
    40
  );

  display.println(
    "Make.com Ready"
  );


  display.display();


  delay(800);
}


// ============================================================
// OPTIONAL SONG / BEAT DEMO
// ============================================================

void playBeat(
  int frequency,
  int duration
) {

  digitalWrite(
    LED_PIN,
    HIGH
  );


  if (
    frequency > 0
  ) {

    ledcWriteTone(
      BUZZER_PIN,
      frequency
    );

  } else {

    ledcWriteTone(
      BUZZER_PIN,
      0
    );
  }


  delay(
    duration
  );


  ledcWriteTone(
    BUZZER_PIN,
    0
  );


  digitalWrite(
    LED_PIN,
    LOW
  );


  delay(50);
}


// ============================================================
// SONG DEMO
// ============================================================
//
// Earlier beat notes:
//
// LOW   = 330 Hz
// MID   = 392 Hz
// HIGH  = 494 Hz
// HIGH2 = 587 Hz
// HIGH3 = 659 Hz
//
// Not automatically called.
//

void playSongDemo() {

  // Tum prem ho

  playBeat(330, 250);
  playBeat(392, 250);
  playBeat(494, 350);


  // Tum preet ho

  playBeat(392, 250);
  playBeat(494, 250);
  playBeat(587, 350);


  // Meri bansuri ka geet ho

  playBeat(494, 250);
  playBeat(392, 250);
  playBeat(330, 250);
  playBeat(392, 300);


  // Tum prem ho

  playBeat(330, 250);
  playBeat(392, 250);
  playBeat(494, 350);


  // Tum preet ho

  playBeat(392, 250);
  playBeat(494, 250);
  playBeat(587, 350);


  // Ending

  playBeat(659, 400);
  playBeat(587, 400);
  playBeat(494, 500);
}