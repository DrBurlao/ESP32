#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>

// Pines y configuraciones
#define SDA_PIN 21
#define SCL_PIN 22
#define ADXL345_ADDR 0x53

// Configuración del Wi-Fi
const char* ssid = "RC_Car_Telemetry";
const char* password = "12345678";

// Crear el servidor web
WebServer server(80);

// Variables globales
float accX = 0, accY = 0, accZ = 0;
float baseX = 0, baseY = 0, baseZ = 0; // Valores base para calibración
float speed = 0;                        // Velocidad calculada del eje X (m/s)
float direction = 0;                    // Dirección calculada del eje Y
float height = 0;                       // Altura calculada del eje Z
const float threshold = 0.1;            // Umbral para evitar oscilaciones (en G)

// Configurar el ADXL345
void setupADXL345() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(0x2D); // Dirección del registro Power Control
  Wire.write(0x08); // Activar mediciones
  Wire.endTransmission();
}

// Leer valores del acelerómetro ADXL345
void readADXL345() {
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(0x32); // Dirección del primer registro de datos
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345_ADDR, 6, true); // Leer 6 bytes

  if (Wire.available() == 6) {
    int16_t x = Wire.read() | (Wire.read() << 8);
    int16_t y = Wire.read() | (Wire.read() << 8);
    int16_t z = Wire.read() | (Wire.read() << 8);

    accX = x * 0.0039; // Convertir a G
    accY = y * 0.0039;
    accZ = z * 0.0039;

    // Aplicar calibración y umbral
    accX = fabs(accX - baseX) > threshold ? accX - baseX : 0;
    accY = fabs(accY - baseY) > threshold ? accY - baseY : 0;
    accZ = fabs(accZ - baseZ) > threshold ? accZ - baseZ : 0;

    // Calcular velocidad usando solo el eje X
    speed = accX * 9.8; // Convertir G a m/s^2

    // Determinar dirección usando el eje Y
    if (fabs(accY) > threshold) {
      direction = accY > 0 ? 1 : -1; // 1: Derecha, -1: Izquierda
    } else {
      direction = 0; // Recto
    }

    // Calcular altura usando el eje Z
    height = accZ * 9.8; // Convertir G a m/s^2
  }
}

// Calibrar el acelerómetro al inicio
void calibrateADXL345() {
  int numReadings = 100; // Número de lecturas para calibración
  float sumX = 0, sumY = 0, sumZ = 0;

  Serial.println("Calibrando acelerómetro...");
  for (int i = 0; i < numReadings; i++) {
    readADXL345();
    sumX += accX;
    sumY += accY;
    sumZ += accZ;
    delay(10);
  }

  baseX = sumX / numReadings;
  baseY = sumY / numReadings;
  baseZ = sumZ / numReadings;

  Serial.println("Calibración completada:");
  Serial.print("Base X: "); Serial.println(baseX);
  Serial.print("Base Y: "); Serial.println(baseY);
  Serial.print("Base Z: "); Serial.println(baseZ);
}

// Página principal HTML
void handleRoot() {
  String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>RC Car Telemetry</title>
      <style>
        body { font-family: Arial; text-align: center; }
        .data { font-size: 24px; margin-top: 20px; }
      </style>
      <script>
        function fetchData() {
          fetch('/telemetry_data')
            .then(response => response.json())
            .then(data => {
              document.getElementById('speed').innerText = `Speed: ${data.speed.toFixed(2)} m/s`;
              document.getElementById('direction').innerText = 
                `Direction: ${data.direction > 0 ? 'Right' : data.direction < 0 ? 'Left' : 'Straight'}`;
              document.getElementById('height').innerText = `Height: ${data.height.toFixed(2)} m`;
            })
            .catch(error => console.error('Error:', error));
        }
        setInterval(fetchData, 20); // Actualizar cada 20ms
      </script>
    </head>
    <body>
      <h1>RC Car Telemetry</h1>
      <div class="data" id="speed">Speed: Loading...</div>
      <div class="data" id="direction">Direction: Loading...</div>
      <div class="data" id="height">Height: Loading...</div>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", page);
}

// Enviar datos de telemetría como JSON
void handleTelemetryData() {
  String json = "{\"speed\":" + String(speed, 2) + 
                ",\"direction\":" + String(direction) + 
                ",\"height\":" + String(height, 2) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  setupADXL345();
  calibrateADXL345(); // Calibrar al iniciar

  // Configurar Wi-Fi en modo AP
  WiFi.softAP(ssid, password);
  Serial.println("WiFi iniciado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.softAPIP());

  // Configurar rutas del servidor web
  server.on("/", handleRoot);
  server.on("/telemetry_data", handleTelemetryData);
  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  server.handleClient();  // Manejar solicitudes del servidor
  readADXL345();          // Leer valores del acelerómetro
  delay(20);              // Actualizar cada 20ms
}
