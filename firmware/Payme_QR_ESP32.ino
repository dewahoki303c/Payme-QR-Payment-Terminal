/**
 * Payme QR Payment Terminal — ESP32 Firmware
 * 
 * Standalone payment terminal that works with the Flask server (app.py).
 * Receives payment notifications via MQTT and controls hardware outputs.
 * 
 * Flow:
 *   1. ESP32 sends POST /api/create-perfume-order to server
 *   2. Server generates Payme QR URL, publishes via MQTT
 *   3. ESP32 receives MQTT → displays QR on Serial (or LCD)
 *   4. Customer pays via Payme app
 *   5. Server receives webhook → publishes "confirmed" via MQTT
 *   6. ESP32 receives confirmation → activates output (relay/pump/LED)
 * 
 * Hardware: ESP32 DevKit (any model)
 * Dependencies: WiFi, PubSubClient, ArduinoJson, HTTPClient
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// ============================================================================
// Configuration — UPDATE THESE
// ============================================================================

// WiFi
const char* ssid     = "Your_WiFi";
const char* password = "Your_Password";

// MQTT Broker
const char* mqtt_server = "broker.hivemq.com";
const int   mqtt_port   = 1883;

// Your merchant ID (must match app.py MERCHANT_ID)
const char* merchant_id = "your_merchant_id";

// Server URL (where app.py is running)
const char* server_url = "https://your-server.com/api";

// Device ID (unique per terminal)
const char* device_id = "ESP32-Terminal-01";

// MQTT topic (auto-generated from merchant_id)
char mqtt_topic[64];

// Output pins — activate on successful payment
#define OUTPUT_PIN_1    2       // Built-in LED or relay
#define OUTPUT_PIN_2    4       // Optional second output
#define OUTPUT_DURATION 5000    // How long to activate (ms)

// ============================================================================
// State
// ============================================================================

WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool orderPending   = false;
String currentQrUrl = "";
unsigned long paymentTime   = 0;
unsigned long outputTimer   = 0;
bool outputActive   = false;
int lastPaidAmount  = 0;

// ============================================================================
// WiFi Setup
// ============================================================================

void setup_wifi() {
    Serial.printf("\n[WiFi] Connecting to %s", ssid);
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\n[WiFi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
    } else {
        Serial.println("\n[WiFi] Connection failed!");
    }
}

// ============================================================================
// MQTT Callback — handles payment notifications from server
// ============================================================================

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    // Parse incoming message
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.println("\n========== MQTT Message ==========");
    Serial.println(message);
    
    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
        Serial.printf("[MQTT] JSON parse error: %s\n", error.c_str());
        return;
    }
    
    String status = doc["status"].as<String>();
    
    // ----- ORDER CREATED: QR code ready -----
    if (status == "created") {
        String qr_url = doc["qr_url"].as<String>();
        int amount    = doc["amount"].as<int>();
        
        currentQrUrl = qr_url;
        orderPending = true;
        paymentTime  = millis();
        
        Serial.println("[Payment] Order created!");
        Serial.printf("[Payment] Amount: %d UZS\n", amount);
        Serial.printf("[Payment] QR URL: %s\n", qr_url.c_str());
        Serial.println("[Payment] Waiting for customer to scan and pay...");
        
        // ================================================
        // TODO: Display QR code on your LCD/OLED here
        // Example with LVGL: generateQrToImage(qr_url);
        //                     lv_scr_load(ui_QRScreen);
        // ================================================
    }
    
    // ----- PAYMENT CONFIRMED: activate output -----
    else if (status == "confirmed") {
        int amount = doc["amount"].as<int>();
        lastPaidAmount = amount;
        
        Serial.println("=================================");
        Serial.printf("[Payment] CONFIRMED: %d UZS\n", amount);
        Serial.println("=================================");
        
        // Activate output
        activateOutput(amount);
        
        // Reset order state
        orderPending = false;
        currentQrUrl = "";
        
        // ================================================
        // TODO: Show success screen on LCD
        // Example: lv_scr_load(ui_SuccessScreen);
        // ================================================
    }
    
    // ----- ORDER CANCELLED -----
    else if (status == "cancelled") {
        Serial.println("[Payment] Order cancelled");
        
        orderPending = false;
        currentQrUrl = "";
        
        // ================================================
        // TODO: Show cancelled screen on LCD
        // Example: lv_scr_load(ui_ErrorScreen);
        // ================================================
    }
}

// ============================================================================
// Output Control — activate relay/pump/LED on payment
// ============================================================================

void activateOutput(int amount) {
    Serial.printf("[Output] Activating for %d UZS payment\n", amount);
    
    // You can use different pins based on amount or product ID
    digitalWrite(OUTPUT_PIN_1, HIGH);
    
    outputActive = true;
    outputTimer = millis();
}

void processOutput() {
    // Turn off output after duration
    if (outputActive && (millis() - outputTimer >= OUTPUT_DURATION)) {
        digitalWrite(OUTPUT_PIN_1, LOW);
        outputActive = false;
        Serial.println("[Output] Deactivated");
    }
}

// ============================================================================
// Create Order — call server to generate Payme QR
// ============================================================================

bool createOrder(int productId, int priceUZS) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[Order] No WiFi!");
        return false;
    }
    
    if (orderPending) {
        Serial.println("[Order] Already have a pending order");
        return false;
    }
    
    Serial.printf("[Order] Creating: product=%d, price=%d UZS\n", productId, priceUZS);
    
    WiFiClientSecure client;
    client.setInsecure();  // Skip certificate verification
    
    HTTPClient http;
    String url = String(server_url) + "/create-perfume-order";
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    
    // Build request body
    JsonDocument doc;
    doc["device_id"] = device_id;
    doc["parfum_id"] = productId;
    doc["amount"]    = priceUZS;
    
    String body;
    serializeJson(doc, body);
    
    int httpCode = http.POST(body);
    
    if (httpCode == 200) {
        String response = http.getString();
        Serial.printf("[Order] Server response: %s\n", response.c_str());
        
        paymentTime = millis();
        return true;
    } else {
        Serial.printf("[Order] HTTP error: %d\n", httpCode);
        return false;
    }
    
    http.end();
}

// ============================================================================
// Cancel Order
// ============================================================================

void cancelOrder() {
    if (!orderPending) return;
    
    Serial.println("[Order] Cancelling...");
    orderPending = false;
    currentQrUrl = "";
}

// ============================================================================
// MQTT Connection
// ============================================================================

void mqtt_reconnect() {
    static unsigned long lastAttempt = 0;
    
    if (!mqttClient.connected() && millis() - lastAttempt > 3000) {
        Serial.print("[MQTT] Connecting...");
        
        String clientId = "ESP32-Payme-" + String(random(0xFFFF), HEX);
        
        if (mqttClient.connect(clientId.c_str())) {
            Serial.println(" connected!");
            mqttClient.subscribe(mqtt_topic);
            Serial.printf("[MQTT] Subscribed to: %s\n", mqtt_topic);
        } else {
            Serial.printf(" failed (rc=%d)\n", mqttClient.state());
        }
        
        lastAttempt = millis();
    }
}

// ============================================================================
// Payment Timeout — cancel if not paid within 3 minutes
// ============================================================================

#define PAYMENT_TIMEOUT_MS  180000  // 3 minutes

void checkPaymentTimeout() {
    if (orderPending && (millis() - paymentTime >= PAYMENT_TIMEOUT_MS)) {
        Serial.println("[Payment] Timeout — cancelling order");
        cancelOrder();
    }
}

// ============================================================================
// Setup & Loop
// ============================================================================

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== Payme QR Payment Terminal ===");
    
    // Build MQTT topic
    snprintf(mqtt_topic, sizeof(mqtt_topic), "payments/%s", merchant_id);
    
    // Output pins
    pinMode(OUTPUT_PIN_1, OUTPUT);
    pinMode(OUTPUT_PIN_2, OUTPUT);
    digitalWrite(OUTPUT_PIN_1, LOW);
    digitalWrite(OUTPUT_PIN_2, LOW);
    
    // Connect WiFi
    setup_wifi();
    
    // Setup MQTT
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(mqtt_callback);
    mqttClient.setBufferSize(1024);
    
    Serial.printf("[Init] MQTT topic: %s\n", mqtt_topic);
    Serial.printf("[Init] Server: %s\n", server_url);
    Serial.println("[Init] Ready! Use createOrder(productId, price) to start payment.");
    Serial.println();
    
    // ================================================
    // Example: create a test order on startup
    // Uncomment to test:
    // createOrder(1, 5000);  // Product 1, 5000 UZS
    // ================================================
}

void loop() {
    // MQTT keepalive
    if (!mqttClient.connected()) {
        mqtt_reconnect();
    }
    mqttClient.loop();
    
    // Check payment timeout
    checkPaymentTimeout();
    
    // Process output (turn off after duration)
    processOutput();
    
    // ================================================
    // TODO: Add your button/touchscreen input here
    // Example:
    //   if (button1Pressed) createOrder(1, 5000);
    //   if (button2Pressed) createOrder(2, 7000);
    //   if (cancelPressed)  cancelOrder();
    // ================================================
    
    // Serial command interface for testing
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        if (cmd == "pay1") {
            createOrder(1, 5000);
        } else if (cmd == "pay2") {
            createOrder(2, 7000);
        } else if (cmd == "pay3") {
            createOrder(3, 8000);
        } else if (cmd == "cancel") {
            cancelOrder();
        } else if (cmd == "status") {
            Serial.printf("[Status] Pending: %s, QR: %s\n",
                orderPending ? "yes" : "no",
                currentQrUrl.c_str());
        }
    }
}
