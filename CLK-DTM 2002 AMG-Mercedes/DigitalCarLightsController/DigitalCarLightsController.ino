#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <WebServer.h>  // Cambié a la librería WebServer estándar

// Pines de las luces
const int frontLights = 19;
const int leftIndicator = 18;
const int rightIndicator = 5;
const int brakeLight = 17;
const int rearLight = 16;

// Pines de los LEDs de decoración
const int decorLedPins[] = {14, 32, 33, 25, 26, 27};

// Pin de la LDR
const int ldrPin = 34;

// Inicializa el MPU6050
MPU6050 mpu;

// Umbrales para activación
const int brakeThreshold = -2000; // Umbral negativo para la luz de freno
const int indicatorThreshold = 2000; // Umbral para los indicadores
const int tiltThreshold = 30000; // Umbral para detectar un vuelco

// Variables para manejar los intermitentes y las luces de vuelco
bool leftIndicatorActive = false;
bool rightIndicatorActive = false;
bool flipLightsActive = false;

// Configuración de Wi-Fi
const char *ssid = "ESP32_Car_Lights"; // Nombre del AP
const char *password = "123456789";    // Contraseña del AP

// Crea un servidor web estándar en el puerto 80
WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Configura los pines como salidas
  pinMode(frontLights, OUTPUT);
  pinMode(leftIndicator, OUTPUT);
  pinMode(rightIndicator, OUTPUT);
  pinMode(brakeLight, OUTPUT);
  pinMode(rearLight, OUTPUT);
  pinMode(ldrPin, INPUT);

  // Configura los pines de los LEDs de decoración
  for (int i = 0; i < 6; i++) {
    pinMode(decorLedPins[i], OUTPUT);
  }

  // Inicializa el MPU6050
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 no conectado correctamente");
    while (1);
  }

  // Inicia el punto de acceso Wi-Fi
  WiFi.softAP(ssid, password);
  Serial.println("Punto de acceso Wi-Fi iniciado");
  Serial.print("IP del AP: ");
  Serial.println(WiFi.softAPIP());

  // Definir las rutas para el servidor web
  server.on("/", HTTP_GET, []() {
    String html = "<html><body><h1>Estado de las luces</h1>";
    html += "<p>Intermitente izquierdo: " + String(leftIndicatorActive ? "Activado" : "Desactivado") + "</p>";
    html += "<p>Intermitente derecho: " + String(rightIndicatorActive ? "Activado" : "Desactivado") + "</p>";
    html += "<p>Luces de vuelco: " + String(flipLightsActive ? "Activadas" : "Desactivadas") + "</p>";
    html += "<p><a href=\"/toggleLeftIndicator\">Alternar Intermitente Izquierdo</a></p>";
    html += "<p><a href=\"/toggleRightIndicator\">Alternar Intermitente Derecho</a></p>";
    html += "<p><a href=\"/toggleFlipLights\">Alternar Luces de Vuelco</a></p>";
    html += "</body></html>";

    server.send(200, "text/html", html);
  });

  // Rutas para controlar las luces desde la página web
  server.on("/toggleLeftIndicator", HTTP_GET, []() {
    leftIndicatorActive = !leftIndicatorActive;
    digitalWrite(leftIndicator, leftIndicatorActive ? HIGH : LOW);
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/toggleRightIndicator", HTTP_GET, []() {
    rightIndicatorActive = !rightIndicatorActive;
    digitalWrite(rightIndicator, rightIndicatorActive ? HIGH : LOW);
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/toggleFlipLights", HTTP_GET, []() {
    flipLightsActive = !flipLightsActive;
    if (flipLightsActive) {
      blinkFlipLights();
    } else {
      digitalWrite(leftIndicator, LOW);
      digitalWrite(rightIndicator, LOW);
    }
    server.sendHeader("Location", "/");
    server.send(303);
  });

  // Inicia el servidor
  server.begin();
}

void loop() {
  // Lee el valor de la LDR
  int ldrValue = analogRead(ldrPin);

  // Determina el umbral de luz baja (ajusta según tu entorno)
  int threshold = 2000; // Este valor puede necesitar ajuste según la luz ambiental

  // Enciende las luces frontales y traseras automáticamente en condiciones de baja luz
  if (ldrValue >= threshold) { // Invertido para encender las luces cuando no hay luz
    fadeLights(frontLights, 255);
    fadeLights(rearLight, 255);
  } else {
    fadeLights(frontLights, 0);
    fadeLights(rearLight, 0);
  }

  // Leer datos del MPU6050
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Detectar vuelco solo en el eje Z
  long tilt = az * az; // Solo considerar la aceleración en el eje Z para el vuelco

  // Simula la luz de freno solo con aceleración negativa y ajusta la intensidad según la fuerza de desaceleración
  int brakeIntensity = map(ax, brakeThreshold, 0, 255, 0); // Mapea la aceleración negativa a intensidad de luz
  brakeIntensity = constrain(brakeIntensity, 0, 255); // Asegúrate de que la intensidad esté dentro del rango
  if (ax < brakeThreshold) { // Solo cuando la aceleración es negativa y supera el umbral
    analogWrite(brakeLight, brakeIntensity);
  } else {
    analogWrite(brakeLight, 0);
  }

  // Verificar y controlar los intermitentes según el giro (basado en el eje Y)
  if (ay < -indicatorThreshold && !leftIndicatorActive) {
    leftIndicatorActive = true;
  } else if (ay > indicatorThreshold && !rightIndicatorActive) {
    rightIndicatorActive = true;
  }

  // Si el giro terminó, apaga el indicador correspondiente
  if (ay > indicatorThreshold && leftIndicatorActive) {
    leftIndicatorActive = false;
  } else if (ay < -indicatorThreshold && rightIndicatorActive) {
    rightIndicatorActive = false;
  }

  // Activa las luces de vuelco si se detecta un vuelco significativo en el eje Z
  if (tilt > tiltThreshold && !flipLightsActive) {
    flipLightsActive = true;
  }

  // Desactiva las luces de vuelco si ya no está volcado
  if (tilt <= tiltThreshold && flipLightsActive) {
    flipLightsActive = false;
  }

  // Parpadeo de los intermitentes
  if (leftIndicatorActive) {
    blinkIndicator(leftIndicator);
  } else {
    digitalWrite(leftIndicator, LOW);
  }

  if (rightIndicatorActive) {
    blinkIndicator(rightIndicator);
  } else {
    digitalWrite(rightIndicator, LOW);
  }

  // Parpadeo de las luces de vuelco
  if (flipLightsActive) {
    blinkFlipLights();
  } else {
    digitalWrite(leftIndicator, LOW);
    digitalWrite(rightIndicator, LOW);
  }

  // Enciende los LEDs de decoración con efecto pulso simultáneamente
  pulseLeds();

  delay(100); // Pausa entre secuencias
}

// Función para encender/apagar las luces gradualmente
void fadeLights(int pin, int value) {
  for (int i = 0; i <= 255; i++) {
    analogWrite(pin, value);
    delay(10); // Ajusta el tiempo según la rapidez del cambio deseado
  }
}

// Función para el parpadeo de los indicadores
void blinkIndicator(int pin) {
  static unsigned long lastMillis = 0;
  static bool state = false;
  unsigned long interval = 500; // Intervalo de parpadeo en milisegundos

  if (millis() - lastMillis >= interval) {
    lastMillis = millis();
    state = !state;
    digitalWrite(pin, state ? HIGH : LOW);
  }
}

// Función para el parpadeo de las luces de vuelco
void blinkFlipLights() {
  static unsigned long lastMillis = 0;
  static bool state = false;
  unsigned long interval = 500; // Intervalo de parpadeo en milisegundos

  if (millis() - lastMillis >= interval) {
    lastMillis = millis();
    state = !state;
    digitalWrite(leftIndicator, state ? HIGH : LOW);
    digitalWrite(rightIndicator, state ? HIGH : LOW);
  }
}

// Función para el efecto pulso de los LEDs de decoración simultáneamente
void pulseLeds() {
  for (int i = 0; i <= 255; i++) {
    for (int j = 0; j < 6; j++) {
      analogWrite(decorLedPins[j], i);
    }
    delay(5);
  }
  for (int i = 255; i >= 0; i--) {
    for (int j = 0; j < 6; j++) {
      analogWrite(decorLedPins[j], i);
    }
    delay(5);
  }
}
