#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Spectrophotometer"; // Wi-Fi SSID
const char* password = "password";       // Wi-Fi Password

WebServer server(80);

// Pin donde está conectado el LDR
const int ldrPin = 35; // Cambia esto según tu configuración

float referenceValue = 0; // Valor de referencia para la absorbancia
float whiteValue = 0; // Valor blanco para la absorbancia
float absorbanceValue = 0; // Valor de absorbancia
float rawValue = 0; // Valor bruto del LDR

bool isUpdating = true; // Variable de control para actualizaciones en vivo

// Estructura para almacenar los valores de medición instantánea
struct Measurement {
  float raw;
  float absorbance;
};

Measurement measurements[100]; // Arreglo para almacenar hasta 100 mediciones
int measurementCount = 0; // Contador de mediciones

void setup() {
  Serial.begin(115200);
  pinMode(ldrPin, INPUT);
  
  // Configurar Wi-Fi
  WiFi.softAP(ssid, password);
  Serial.println("WiFi Started");
  
  server.on("/", handleRoot);
  server.on("/measure", HTTP_GET, handleMeasure);
  server.on("/set_reference", HTTP_POST, handleSetReference);
  server.on("/set_white", HTTP_POST, handleSetWhite);
  server.on("/get_values", HTTP_GET, handleGetValues);
  server.on("/toggle_update", HTTP_GET, handleToggleUpdate);
  
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<html><head><title>Spectrophotometer</title>";
  html += "<style>body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 20px; }";
  html += ".frame { background-color: #fff; border: 1px solid #ccc; border-radius: 5px; padding: 10px; margin-bottom: 10px; position: relative; }";
  html += ".header { background-color: #007BFF; color: white; padding: 5px; cursor: move; }";
  html += ".close-btn { position: absolute; right: 10px; top: 5px; color: white; cursor: pointer; }";
  html += ".hidden { display: none; }";
  html += "</style>";
  html += "<script>let updateActive = true; function updateValues() { if (updateActive) { fetch('/get_values').then(response => response.text()).then(data => document.getElementById('current_values').innerHTML = data); }}";
  html += "setInterval(updateValues, 1000);";
  html += "function toggleUpdate() { updateActive = !updateActive; fetch('/toggle_update'); }";
  html += "function dragElement(elmnt) { var pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0; elmnt.getElementsByClassName('header')[0].onmousedown = dragMouseDown; function dragMouseDown(e) { e = e || window.event; e.preventDefault(); pos3 = e.clientX; pos4 = e.clientY; document.onmouseup = closeDragElement; document.onmousemove = elementDrag; } function elementDrag(e) { e = e || window.event; e.preventDefault(); pos1 = pos3 - e.clientX; pos2 = pos4 - e.clientY; pos3 = e.clientX; pos4 = e.clientY; elmnt.style.top = (elmnt.offsetTop - pos2) + 'px'; elmnt.style.left = (elmnt.offsetLeft - pos1) + 'px'; } function closeDragElement() { document.onmouseup = null; document.onmousemove = null; } }";
  html += "function toggleFrame(id) { var frame = document.getElementById(id); frame.classList.toggle('hidden'); }</script></head><body>";
  html += "<h1>Spectrophotometer Control</h1>";

  // Marco para establecer el valor de referencia
  html += "<div class='frame' id='ref_frame' onmousedown='dragElement(this)'>";
  html += "<div class='header'>Set Reference Value <span class='close-btn' onclick=\"toggleFrame('ref_frame')\">✖</span></div>";
  html += "<form action='/set_reference' method='POST'>";
  html += "Reference Value (V): <input type='number' name='reference' step='any'> V<br>";
  html += "<input type='submit' value='Set Reference'>";
  html += "</form></div>";

  // Marco para establecer el valor blanco
  html += "<div class='frame' id='white_frame' onmousedown='dragElement(this)'>";
  html += "<div class='header'>Set White Value <span class='close-btn' onclick=\"toggleFrame('white_frame')\">✖</span></div>";
  html += "<form action='/set_white' method='POST'>";
  html += "White Value (V): <input type='number' name='white' step='any'> V<br>";
  html += "<input type='submit' value='Set White'>";
  html += "</form></div>";

  // Marco para la medición instantánea
  html += "<div class='frame' id='measure_frame' onmousedown='dragElement(this)'>";
  html += "<div class='header'>Instant Measurement <span class='close-btn' onclick=\"toggleFrame('measure_frame')\">✖</span></div>";
  html += "<button onclick=\"location.href='/measure'\">Take Instant Measurement</button><br><br>";
  html += "<button onclick=\"toggleUpdate()\">Toggle Live View</button><br><br>";
  html += "</div>";

  // Mostrar los valores actuales
  html += "<h2>Current Values:</h2>";
  html += "<div id='current_values'>Loading...</div>";
  
  // Tabla de mediciones
  html += "<h2>Measurement History:</h2><table border='1'><tr><th>Raw Value</th><th>Absorbance</th></tr>";
  for (int i = 0; i < measurementCount; i++) {
    html += "<tr><td>" + String(measurements[i].raw) + "</td><td>" + String(measurements[i].absorbance) + "</td></tr>";
  }
  html += "</table>";

  html += "<br>&copy; Álvaro Merchán Martín"; // Copyright
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleMeasure() {
  rawValue = analogRead(ldrPin); // Leer el valor bruto
  absorbanceValue = calculateAbsorbance(rawValue);
  
  // Almacenar la medición
  if (measurementCount < 100) {
    measurements[measurementCount].raw = rawValue;
    measurements[measurementCount].absorbance = absorbanceValue;
    measurementCount++;
  } else {
    // Si el arreglo está lleno, podrías hacer algo aquí, como sobrescribir o no hacer nada
    Serial.println("Measurement history full, cannot store more measurements.");
  }
  
  Serial.println("Instant Measurement: Raw: " + String(rawValue) + ", Absorbance: " + String(absorbanceValue));
  server.send(200, "text/plain", "Measurement taken!"); // Enviar respuesta sin redirigir
}

void handleSetReference() {
  if (server.hasArg("reference")) {
    referenceValue = server.arg("reference").toFloat();
    Serial.println("Reference value set to: " + String(referenceValue));
    server.send(200, "text/plain", "Reference value set!"); // Enviar respuesta sin redirigir
  } else {
    server.send(400, "text/plain", "Invalid reference value!"); // Respuesta en caso de error
  }
}

void handleSetWhite() {
  if (server.hasArg("white")) {
    whiteValue = server.arg("white").toFloat();
    Serial.println("White value set to: " + String(whiteValue));
    server.send(200, "text/plain", "White value set!"); // Enviar respuesta sin redirigir
  } else {
    server.send(400, "text/plain", "Invalid white value!"); // Respuesta en caso de error
  }
}

void handleGetValues() {
  String values = "Raw Value: " + String(rawValue) + " V<br>";
  values += "Absorbance Value: " + String(absorbanceValue) + "<br>";
  values += "Reference Value: " + String(referenceValue) + " V<br>";
  values += "White Value: " + String(whiteValue) + " V<br>";
  values += "<br>&copy; Álvaro Merchán Martín"; // Copyright
  server.send(200, "text/html", values);
}

void handleToggleUpdate() {
  isUpdating = !isUpdating; // Alternar el estado de actualización en vivo
  server.send(200, "text/plain", "Live update toggled!"); // Enviar respuesta sin redirigir
}

float calculateAbsorbance(float raw) {
  if (raw == 0) return 0; // Evitar división por cero
  return log10(referenceValue / raw); // Cálculo de absorbancia
}
