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
float velX = 0, velY = 0;              // Velocidades en m/s
float posX = 0, posY = 0;              // Posiciones en metros
float speedKmH = 0;                    // Velocidad en km/h
float acceleration = 0;                // Aceleración en m/s²
float displacement = 0;                // Desplazamiento acumulado (m)
float threshold = 0.1;                 // Umbral para evitar oscilaciones (en G)
const float gravity = 9.8;             // Conversión de G a m/s^2
bool captureActive = true;             // Bandera para captura de datos
unsigned long lastUpdateTime = 0;      // Tiempo de la última actualización
unsigned long sessionStartTime;        // Tiempo de inicio de la sesión
String logData = "Time (s),Speed (km/h),Acceleration (m/s²),Displacement (m),PosX (m),PosY (m)\n";

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
  Wire.write(0x32); // Dirección del registro de datos
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
  }
}

// Calibrar el acelerómetro al inicio
void calibrateADXL345() {
  int numReadings = 100; // Número de lecturas para calibración
  float sumX = 0, sumY = 0;

  Serial.println("Calibrando acelerómetro...");
  for (int i = 0; i < numReadings; i++) {
    readADXL345();
    sumX += accX;
    sumY += accY;
    delay(10);
  }

  baseX = sumX / numReadings;
  baseY = sumY / numReadings;

  Serial.println("Calibración completada.");
}

// Calcular telemetría
void calculateTelemetry() {
  if (!captureActive) return; // Pausar cálculo si la captura está desactivada

  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastUpdateTime) / 1000.0; // Tiempo en segundos
  lastUpdateTime = currentTime;

  // Calcular aceleración
  float accelX = accX * gravity;
  float accelY = accY * gravity;

  // Actualizar velocidades en X e Y
  velX += accelX * deltaTime;
  velY += accelY * deltaTime;

  // Actualizar posiciones en el plano
  posX += velX * deltaTime;
  posY += velY * deltaTime;

  // Calcular velocidad total (km/h)
  speedKmH = sqrt(velX * velX + velY * velY) * 3.6;

  // Calcular desplazamiento acumulado
  displacement += sqrt(pow(velX * deltaTime, 2) + pow(velY * deltaTime, 2));

  // Guardar en log
  logData += String((millis() - sessionStartTime) / 1000.0) + "," + String(speedKmH, 2) + "," +
             String(acceleration, 2) + "," + String(displacement, 2) + "," +
             String(posX, 2) + "," + String(posY, 2) + "\n";
}

// Página principal HTML
void handleRoot() {
  String page = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>RC Car Telemetry</title>
      <style>
        body {
          font-family: Arial, sans-serif;
          background-color: #2c3e50;
          color: #ecf0f1;
          text-align: center;
        }
        canvas {
          border: 2px solid #3498db;
          margin: 20px auto;
          display: block;
        }
        button, input[type=range], select {
          margin: 10px;
          padding: 10px;
          font-size: 16px;
          border: none;
          border-radius: 5px;
          background-color: #3498db;
          color: white;
          cursor: pointer;
        }
        button:hover, input[type=range]:hover, select:hover {
          background-color: #2980b9;
        }
        input[type=range] {
          width: 300px;
        }
      </style>
      <script>
        let points = [];
        let scale = 10;
        let captureActive = true;

        function fetchData() {
          fetch('/telemetry_data')
            .then(response => response.json())
            .then(data => {
              document.getElementById('speed').innerText = `Speed: ${data.speedKmH.toFixed(2)} km/h`;
              document.getElementById('threshold').innerText = `Threshold: ${data.threshold.toFixed(2)} G`;
              document.getElementById('status').innerText = captureActive ? 'Status: Active' : 'Status: Paused';
              document.getElementById('displacement').innerText = `Displacement: ${data.displacement.toFixed(2)} m`;
              document.getElementById('position').innerText = `Position: (${data.posX.toFixed(2)} m, ${data.posY.toFixed(2)} m)`;
              points.push({ x: data.posX, y: data.posY });
              drawMap();
            })
            .catch(error => console.error('Error:', error));
        }

        function drawMap() {
          const canvas = document.getElementById('map');
          const ctx = canvas.getContext('2d');
          ctx.clearRect(0, 0, canvas.width, canvas.height);

          ctx.beginPath();
          ctx.moveTo(canvas.width / 2, canvas.height / 2);

          points.forEach(point => {
            const x = canvas.width / 2 + point.x * scale;
            const y = canvas.height / 2 - point.y * scale;
            ctx.lineTo(x, y);
          });

          ctx.strokeStyle = '#3498db';
          ctx.lineWidth = 2;
          ctx.stroke();
        }

        function toggleCapture() {
          captureActive = !captureActive;
          fetch(`/toggle_capture?active=${captureActive}`);
        }

        function downloadLog() {
          fetch('/download_log')
            .then(response => response.text())
            .then(data => {
              const blob = new Blob([data], { type: 'text/csv' });
              const url = window.URL.createObjectURL(blob);
              const a = document.createElement('a');
              a.style.display = 'none';
              a.href = url;
              a.download = 'telemetry_log.csv';
              document.body.appendChild(a);
              a.click();
              window.URL.revokeObjectURL(url);
            });
        }

        setInterval(fetchData, 100);
      </script>
    </head>
    <body>
      <h1>RC Car Telemetry</h1>
      <div id="speed">Speed: Loading...</div>
      <div id="threshold">Threshold: Loading...</div>
      <div id="status">Status: Loading...</div>
      <div id="displacement">Displacement: Loading...</div>
      <div id="position">Position: Loading...</div>
      <canvas id="map" width="600" height="600"></canvas>
      <button onclick="toggleCapture()">Pause/Resume Capture</button>
      <button onclick="downloadLog()">Download Log</button>
      <select onchange="scale = parseInt(this.value)">
        <option value="5">Scale x5</option>
        <option value="10" selected>Scale x10</option>
        <option value="20">Scale x20</option>
      </select>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", page);
}

// Enviar datos de telemetría como JSON
void handleTelemetryData() {
  String json = "{\"speedKmH\":" + String(speedKmH, 2) + 
                ",\"threshold\":" + String(threshold, 2) + 
                ",\"displacement\":" + String(displacement, 2) + 
                ",\"posX\":" + String(posX, 2) + 
                ",\"posY\":" + String(posY, 2) + "}";
  server.send(200, "application/json", json);
}

// Descargar el registro en formato CSV
void handleDownloadLog() {
  server.send(200, "text/csv", logData);
}

// Alternar captura de datos
void handleToggleCapture() {
  if (server.hasArg("active")) {
    captureActive = server.arg("active") == "true";
    Serial.println("Capture Active: " + String(captureActive));
  }
  server.send(200, "text/plain", "Capture toggled");
}

void setup() {
  Serial.begin(115200);
  setupADXL345();
  calibrateADXL345();

  // Configurar Wi-Fi en modo AP
  WiFi.softAP(ssid, password);
  Serial.println("WiFi iniciado.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.softAPIP());

  // Configurar rutas del servidor web
  server.on("/", handleRoot);
  server.on("/telemetry_data", handleTelemetryData);
  server.on("/download_log", handleDownloadLog);
  server.on("/toggle_capture", handleToggleCapture);
  server.begin();
  Serial.println("Servidor iniciado.");

  lastUpdateTime = millis();
  sessionStartTime = millis();
}

void loop() {
  server.handleClient();
  readADXL345();
  calculateTelemetry();
}
