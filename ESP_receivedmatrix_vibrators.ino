#include <WiFi.h>
#include <PubSubClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// WiFi credentials
const char* ssid = "pathpal"; // Replace with your Wi-Fi SSID
const char* password = "pathpal1"; // Replace with your Wi-Fi password

// MQTT broker address
const char* mqtt_server = "test.mosquitto.org"; // Public broker for MQTT communication

// Define vibrator pins
const int vibrators[3][3] = {  
  {14, 32, 33},   // First row of vibrator pins
  {12, 26, 25},   // Second row of vibrator pins
  {13, 27, 4}   // Third row of vibrator pins
};

WiFiClient espClient;
PubSubClient client(espClient); // MQTT client using the WiFi client

// Global variable for the matrix to control vibrators
volatile int matrix[3][3] = {0}; // Matrix will be updated based on incoming MQTT messages

/* 
 * Callback function to handle incoming MQTT messages 
 * This function is triggered when a message is received on a subscribed topic.
 */
/* 
 * Callback function to handle incoming MQTT messages 
 * This function is triggered when a message is received on a subscribed topic.
 */
void callback(char* topic, byte* message, unsigned int length) {
  // Process incoming message and update the matrix
  if (length == 0) return; // If no message content, return

  // Clear previous matrix values
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      matrix[i][j] = 0; // Reset matrix to default state
    }
  }

  // Convert the incoming message to a string
  String messageStr = String((char*)message).substring(0, length);
  Serial.println("Received: " + messageStr); // Debug print to show the message

  // Update the matrix based on the message content (assuming it is a JSON-like string)
  int index = 0;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      matrix[i][j] = messageStr[index] == '1' ? 1 : 0; // Set 1 or 0 based on the message
      index += 2; // Move to the next character in the message
    }
  }
}



/*
 * Function to connect to WiFi
 * This function establishes a connection to the specified WiFi network.
 */
void setup_wifi() {
  delay(10);
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password); // Begin the connection process
  
  // Continuously check the connection status until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); // Print dots to indicate connection attempt
  }
  
  Serial.println(" connected"); // Debug message when successfully connected
}

/*
 * Task to control vibrators based on the matrix values
 * This task runs on core 1 and is responsible for turning vibrators on or off.
 */
void controlVibratorsTask(void *param) {
  while (1) { // Infinite loop to keep controlling the vibrators
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        digitalWrite(vibrators[i][j], matrix[i][j]); // Write values from matrix to control pins
        Serial.print(matrix[i][j]);
        Serial.print(" ");
      }
      Serial.println();
    }
    // Serial.println();
    delay(100); // Small delay to avoid overloading the system
  }
}

/*
 * Task to handle MQTT messages
 * This task runs on core 0 and is responsible for managing MQTT communication.
 */
void mqttTask(void *param) {
  client.setCallback(callback); // Set the callback to handle incoming MQTT messages
  
  while (1) { // Infinite loop to continuously handle MQTT connection and messages
    if (!client.connected()) {
      reconnect(); // If not connected, attempt to reconnect to the broker
    }
    client.loop(); // Keep the MQTT client loop running to process messages
    delay(10); // Small delay to avoid hogging CPU resources
  }
}

/*
 * Function to reconnect to the MQTT broker
 * This function attempts to connect to the MQTT broker and subscribes to the topic.
 */
void reconnect() {
  while (!client.connected()) { // Keep trying until connected
    Serial.print("Attempting MQTT connection...");
      client.setKeepAlive(60); // Set a keep-alive period for MQTT connection
    if (client.connect("ESP32Clientpathpal")) { // Connect with a unique client ID
      Serial.println("connected"); // Debug message when connected
      client.subscribe("vibrator/matrix"); // Subscribe to the topic to receive matrix data
    } else {
      Serial.print("failed, rc="); 
      Serial.print(client.state()); // Print the reason for connection failure
      delay(5000); // Wait 5 seconds before trying again
    }
  }
}

void setup() {
  Serial.begin(115200); // Start serial communication for debugging
  
  // Initialize vibrator pins as OUTPUT
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      pinMode(vibrators[i][j], OUTPUT); // Set each vibrator pin as an output pin
    }
  }

  setup_wifi(); // Connect to WiFi
  client.setServer(mqtt_server, 1883); // Set the MQTT broker address and port

  // Create a task to handle MQTT communication on core 0
  xTaskCreatePinnedToCore(
    mqttTask,               // Task function
    "MQTT Task",            // Name of the task
    10000,                  // Stack size for the task (in bytes)
    NULL,                   // Task input parameter (not used)
    1,                      // Priority of the task (1 is lowest priority)
    NULL,                   // Task handle (not used here)
    0                       // Core 0 to run the task (handling MQTT)
  );

  // Create a task to control vibrators on core 1
  xTaskCreatePinnedToCore(
    controlVibratorsTask,   // Task function
    "Control Vibrators Task", // Name of the task
    10000,                  // Stack size for the task (in bytes)
    NULL,                   // Task input parameter (not used)
    1,                      // Priority of the task (1 is lowest priority)
    NULL,                   // Task handle (not used here)
    1                       // Core 1 to run the task (handling vibrators)
  );
}

void loop() {
  // No code in the main loop, as tasks are running independently on different cores
}
