#include <WiFi.h>
#include <WebServer.h>
#include <math.h>

// Definición de pines
#define MIC_PIN 34

// Configuración del servidor web
WebServer server(80);

// Variables para el análisis de datos
const int numReadings = 1000;
int readings[numReadings];
int readIndex = 0;
long total = 0;
int minValue = 1023;
int maxValue = 0;
float mean = 0.0;
float stddev = 0.0;
int peakLevel = 0;
float rmsLevel = 0.0;

// Tiempo de muestreo
const unsigned long sampleInterval = 100; // Intervalo de muestreo en milisegundos
unsigned long lastSampleTime = 0;

// Funciones auxiliares
void updateStatistics(int micValue) {
  // Actualiza la suma total
  total -= readings[readIndex];
  readings[readIndex] = micValue;
  total += readings[readIndex];

  // Actualiza minValue y maxValue
  minValue = min(minValue, micValue);
  maxValue = max(maxValue, micValue);

  // Actualiza la media
  mean = (float)total / numReadings;

  // Calcula la desviación estándar y el nivel RMS
  float sumSquaredDiff = 0.0;
  float sumSquares = 0.0;
  for (int i = 0; i < numReadings; i++) {
    float diff = readings[i] - mean;
    sumSquaredDiff += diff * diff;
    sumSquares += readings[i] * readings[i];
  }
  stddev = sqrt(sumSquaredDiff / numReadings);
  rmsLevel = sqrt(sumSquares / numReadings);

  // Actualiza el nivel de pico
  peakLevel = max(peakLevel, micValue);

  readIndex = (readIndex + 1) % numReadings;
}

void setup() {
  Serial.begin(115200);

  // Configuración de pines de LEDs
  pinMode(LED_AZUL_1, OUTPUT);
  pinMode(LED_BLANCO_1, OUTPUT);
  pinMode(LED_AZUL_2, OUTPUT);
  pinMode(LED_BLANCO_2, OUTPUT);
  pinMode(LED_CENTRAL_R, OUTPUT);
  pinMode(LED_CENTRAL_G, OUTPUT);
  pinMode(LED_CENTRAL_B, OUTPUT);
  pinMode(LED_BLANCO_3, OUTPUT);
  pinMode(LED_AZUL_3, OUTPUT);
  pinMode(LED_BLANCO_4, OUTPUT);
  pinMode(LED_AZUL_4, OUTPUT);

  // Configuración del pin del micrófono
  pinMode(MIC_PIN, INPUT);

  // Configuración del punto de acceso WiFi
  WiFi.softAP("ESP32_Mic_Analyzer");

  // Configuración de la página web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/reset", HTTP_GET, handleReset);

  // Inicia el servidor
  server.begin();

  Serial.println("Servidor web iniciado. Conéctate a 'ESP32_Mic_Analyzer' para ver los datos.");
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastSampleTime >= sampleInterval) {
    // Lee el valor del micrófono
    int micValue = analogRead(MIC_PIN);

    // Ajuste de la lectura del micrófono si es necesario
    // Por ejemplo, aplicar un factor de escala o ajustar el offset si el micrófono está sesgado
    micValue = map(micValue, 0, 4095, 0, 255); // Ajusta según el rango real

    // Actualiza las estadísticas
    updateStatistics(micValue);

    // Imprime en el monitor serie
    Serial.print("Mic Value: ");
    Serial.print(micValue);
    Serial.print(" | Min: ");
    Serial.print(minValue);
    Serial.print(" | Max: ");
    Serial.print(maxValue);
    Serial.print(" | Mean: ");
    Serial.print(mean);
    Serial.print(" | StdDev: ");
    Serial.print(stddev);
    Serial.print(" | Peak: ");
    Serial.print(peakLevel);
    Serial.print(" | RMS: ");
    Serial.println(rmsLevel);

    lastSampleTime = currentTime;
  }

  // Maneja las solicitudes HTTP
  server.handleClient();
}

void handleRoot() {
  String html = "<html><head><title>Datos del Micrófono</title></head><body>";
  html += "<h1>Datos del Micrófono</h1>";
  html += "<p>Valor Mínimo: " + String(minValue) + "</p>";
  html += "<p>Valor Máximo: " + String(maxValue) + "</p>";
  html += "<p>Valor Promedio: " + String(mean) + "</p>";
  html += "<p>Desviación Estándar: " + String(stddev) + "</p>";
  html += "<p>Nivel de Pico: " + String(peakLevel) + "</p>";
  html += "<p>Nivel RMS: " + String(rmsLevel) + "</p>";
  html += "<p><a href='/reset'>Restablecer Estadísticas</a></p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleReset() {
  minValue = 1023;
  maxValue = 0;
  total = 0;
  peakLevel = 0;
  rmsLevel = 0.0;
  readIndex = 0;
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
  mean = 0.0;
  stddev = 0.0;

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Resetting...");
}
