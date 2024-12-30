#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>

// Definir el pin GPIO del láser
const int laserPin = 13;  // Cambia al pin que tengas asignado para el láser

// Credenciales de Wi-Fi
const char* ssid = "ESP32-AP";
const char* password = "123456789";

// Configurar servidor web asincrónico
AsyncWebServer server(80);

// Variables de estado
int laserIntensity = 0;  // Inicialmente apagado
bool laserState = false; // Estado del láser (encendido o apagado)

// Iniciar el punto de acceso Wi-Fi
void setup() {
  Serial.begin(115200);
  
  // Iniciar EEPROM y cargar el valor guardado de la intensidad
  EEPROM.begin(512);
  laserIntensity = EEPROM.read(0);  // Leer la intensidad guardada en la memoria EEPROM
  laserState = (laserIntensity > 0);  // Si la intensidad es mayor que 0, el láser está encendido
  pinMode(laserPin, OUTPUT);
  analogWrite(laserPin, laserIntensity);  // Inicializar con la intensidad guardada

  // Configurar el punto de acceso Wi-Fi
  WiFi.softAP(ssid, password);

  Serial.println("Punto de acceso iniciado.");
  Serial.print("IP del AP: ");
  Serial.println(WiFi.softAPIP());

  // Configurar las rutas y respuestas del servidor web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body><h1>Control del Láser</h1>";
    html += "<p><b>Estado del láser:</b> " + String(laserState ? "Encendido" : "Apagado") + "</p>";
    html += "<p><b>Intensidad del láser:</b> " + String(laserIntensity) + "</p>";
    
    // Mostrar los controles de intensidad
    html += "<p><b>Ajustar intensidad:</b></p>";
    html += "<form action=\"/setIntensity\" method=\"GET\">";
    html += "<input type=\"range\" name=\"intensity\" min=\"0\" max=\"255\" value=\"" + String(laserIntensity) + "\" />";
    html += "<input type=\"submit\" value=\"Ajustar\" />";
    html += "</form>";

    // Botones para incrementar o decrementar la intensidad
    html += "<p><a href=\"/increaseIntensity\">Incrementar Intensidad</a></p>";
    html += "<p><a href=\"/decreaseIntensity\">Disminuir Intensidad</a></p>";

    // Botón para encender o apagar el láser
    html += "<p><a href=\"/toggleLaser\">Encender/Apagar Láser</a></p>";

    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Ruta para ajustar la intensidad del láser
  server.on("/setIntensity", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("intensity")) {
      laserIntensity = request->getParam("intensity")->value().toInt();
      laserIntensity = constrain(laserIntensity, 0, 255);  // Asegurar que la intensidad esté en el rango permitido
      analogWrite(laserPin, laserIntensity);  // Ajustar la intensidad del láser

      // Guardar la configuración en EEPROM
      EEPROM.write(0, laserIntensity);
      EEPROM.commit();
    }
    request->redirect("/");  // Redirige al control principal
  });

  // Ruta para incrementar la intensidad
  server.on("/increaseIntensity", HTTP_GET, [](AsyncWebServerRequest *request){
    laserIntensity = constrain(laserIntensity + 10, 0, 255);
    analogWrite(laserPin, laserIntensity);  // Ajustar la intensidad del láser

    // Guardar la configuración en EEPROM
    EEPROM.write(0, laserIntensity);
    EEPROM.commit();
    request->redirect("/");  // Redirige al control principal
  });

  // Ruta para disminuir la intensidad
  server.on("/decreaseIntensity", HTTP_GET, [](AsyncWebServerRequest *request){
    laserIntensity = constrain(laserIntensity - 10, 0, 255);
    analogWrite(laserPin, laserIntensity);  // Ajustar la intensidad del láser

    // Guardar la configuración en EEPROM
    EEPROM.write(0, laserIntensity);
    EEPROM.commit();
    request->redirect("/");  // Redirige al control principal
  });

  // Ruta para encender o apagar el láser
  server.on("/toggleLaser", HTTP_GET, [](AsyncWebServerRequest *request){
    if (laserState) {
      laserState = false;
      laserIntensity = 0;  // Apagar el láser
    } else {
      laserState = true;
      laserIntensity = 255;  // Encender el láser al máximo
    }
    analogWrite(laserPin, laserIntensity);  // Cambia la intensidad del láser

    // Guardar el estado y la intensidad en EEPROM
    EEPROM.write(0, laserIntensity);
    EEPROM.commit();
    request->redirect("/");  // Redirige al control principal
  });

  // Iniciar el servidor
  server.begin();
}

void loop() {
  // No es necesario código en el loop, el servidor web maneja las solicitudes en segundo plano
}
