#include <WiFi.h>
#include <WebServer.h>

// Definir los pines para cada LED
#define LED_AZUL_1 5   // Esquina superior izquierda
#define LED_BLANCO_1 13 // Lado superior
#define LED_AZUL_2 12  // Esquina superior derecha
#define LED_BLANCO_2 14 // Lado izquierdo
#define LED_CENTRAL_R 18 // LED Central - Rojo
#define LED_CENTRAL_G 19 // LED Central - Verde
#define LED_CENTRAL_B 23 // LED Central - Azul
#define LED_BLANCO_3 27 // Lado derecho
#define LED_AZUL_3 16  // Esquina inferior izquierda
#define LED_BLANCO_4 17 // Lado inferior
#define LED_AZUL_4 25  // Esquina inferior derecha

// Crear una instancia del servidor web en el puerto 80
WebServer server(80);

unsigned long previousMillis = 0;  // Para el tiempo transcurrido
const long interval = 5000;       // Intervalo de 30 segundos
int currentAnimation = 0;          // Índice de la animación actual
bool autoMode = true;              // Modo automático para cambiar animaciones

// Nombre y contraseña de la red WiFi
const char* ssid = "ESP32-LED-Control";
const char* password = "12345678";

void setup() {
  // Configurar los pines como salidas
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

  // Configurar y lanzar el AP WiFi
  WiFi.softAP(ssid, password);

  // Manejar peticiones web
  server.on("/", handleRoot);  // Página principal
  server.on("/setAnimation", handleSetAnimation);  // Cambiar animación
  server.on("/toggleAutoMode", handleToggleAutoMode);  // Cambiar modo automático/manual

  server.begin();  // Iniciar servidor web
}

void loop() {
  server.handleClient();  // Manejar peticiones de clientes

  unsigned long currentMillis = millis();

  // Cambiar de animación cada 30 segundos en modo automático
  if (autoMode && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    currentAnimation++;
    if (currentAnimation > 4) { // Cambiar entre 5 animaciones (0, 1, 2, 3, 4)
      currentAnimation = 0;
    }
  }

  // Seleccionar la animación actual
  switch (currentAnimation) {
    case 0:
      allBlink();
      break;
    case 1:
      animateCenterLED();
      break;
    case 2:
      runningLight();
      break;
    case 3:
      fadeInOut();
      break;
    case 4:
      chasingLight();
      break;
  }
}

// Animación 1: Todos los LEDs parpadean
void allBlink() {
  digitalWrite(LED_AZUL_1, HIGH);
  digitalWrite(LED_BLANCO_1, HIGH);
  digitalWrite(LED_AZUL_2, HIGH);
  digitalWrite(LED_BLANCO_2, HIGH);
  digitalWrite(LED_CENTRAL_R, HIGH);
  digitalWrite(LED_CENTRAL_G, HIGH);
  digitalWrite(LED_CENTRAL_B, HIGH);
  digitalWrite(LED_BLANCO_3, HIGH);
  digitalWrite(LED_AZUL_3, HIGH);
  digitalWrite(LED_BLANCO_4, HIGH);
  digitalWrite(LED_AZUL_4, HIGH);
  delay(250);

  digitalWrite(LED_AZUL_1, LOW);
  digitalWrite(LED_BLANCO_1, LOW);
  digitalWrite(LED_AZUL_2, LOW);
  digitalWrite(LED_BLANCO_2, LOW);
  digitalWrite(LED_CENTRAL_R, LOW);
  digitalWrite(LED_CENTRAL_G, LOW);
  digitalWrite(LED_CENTRAL_B, LOW);
  digitalWrite(LED_BLANCO_3, LOW);
  digitalWrite(LED_AZUL_3, LOW);
  digitalWrite(LED_BLANCO_4, LOW);
  digitalWrite(LED_AZUL_4, LOW);
  delay(250);
}

// Animación 2: Animar el LED central RGB
void animateCenterLED() {
  static int colorState = 0;

  // Apagar todos los colores del LED central
  digitalWrite(LED_CENTRAL_R, LOW);
  digitalWrite(LED_CENTRAL_G, LOW);
  digitalWrite(LED_CENTRAL_B, LOW);

  // Encender el color correspondiente según el estado actual
  switch (colorState) {
    case 0:
      digitalWrite(LED_CENTRAL_R, HIGH); // Rojo
      break;
    case 1:
      digitalWrite(LED_CENTRAL_G, HIGH); // Verde
      break;
    case 2:
      digitalWrite(LED_CENTRAL_B, HIGH); // Azul
      break;
    case 3:
      digitalWrite(LED_CENTRAL_R, HIGH);
      digitalWrite(LED_CENTRAL_G, HIGH); // Amarillo (Rojo + Verde)
      break;
    case 4:
      digitalWrite(LED_CENTRAL_G, HIGH);
      digitalWrite(LED_CENTRAL_B, HIGH); // Cyan (Verde + Azul)
      break;
    case 5:
      digitalWrite(LED_CENTRAL_R, HIGH);
      digitalWrite(LED_CENTRAL_B, HIGH); // Magenta (Rojo + Azul)
      break;
    case 6:
      digitalWrite(LED_CENTRAL_R, HIGH);
      digitalWrite(LED_CENTRAL_G, HIGH);
      digitalWrite(LED_CENTRAL_B, HIGH); // Blanco (Rojo + Verde + Azul)
      break;
  }

  // Cambiar al siguiente color
  colorState++;
  if (colorState > 6) {
    colorState = 0;
  }

  delay(500);
}

// Animación 3: Efecto de luz en movimiento
void runningLight() {
  digitalWrite(LED_AZUL_1, HIGH);
  delay(100);
  digitalWrite(LED_AZUL_1, LOW);

  digitalWrite(LED_BLANCO_1, HIGH);
  delay(100);
  digitalWrite(LED_BLANCO_1, LOW);

  digitalWrite(LED_AZUL_2, HIGH);
  delay(100);
  digitalWrite(LED_AZUL_2, LOW);

  digitalWrite(LED_BLANCO_3, HIGH);
  delay(100);
  digitalWrite(LED_BLANCO_3, LOW);

  digitalWrite(LED_AZUL_4, HIGH);
  delay(100);
  digitalWrite(LED_AZUL_4, LOW);

  digitalWrite(LED_BLANCO_4, HIGH);
  delay(100);
  digitalWrite(LED_BLANCO_4, LOW);

  digitalWrite(LED_AZUL_3, HIGH);
  delay(100);
  digitalWrite(LED_AZUL_3, LOW);

  digitalWrite(LED_BLANCO_2, HIGH);
  delay(100);
  digitalWrite(LED_BLANCO_2, LOW);
}

// Animación 4: Fade In y Fade Out del LED central
void fadeInOut() {
  for (int brightness = 0; brightness <= 255; brightness += 5) {
    analogWrite(LED_CENTRAL_R, brightness);
    analogWrite(LED_CENTRAL_G, brightness);
    analogWrite(LED_CENTRAL_B, brightness);
    delay(30);
  }
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    analogWrite(LED_CENTRAL_R, brightness);
    analogWrite(LED_CENTRAL_G, brightness);
    analogWrite(LED_CENTRAL_B, brightness);
    delay(30);
  }
}

// Animación 5: Luces persiguiéndose
void chasingLight() {
  digitalWrite(LED_AZUL_1, HIGH);
  delay(100);
  digitalWrite(LED_AZUL_1, LOW);
  digitalWrite(LED_BLANCO_1, HIGH);
  delay(100);
  digitalWrite(LED_BLANCO_1, LOW);
  digitalWrite(LED_AZUL_2, HIGH);
  delay(100);
  digitalWrite(LED_AZUL_2, LOW);
  digitalWrite(LED_BLANCO_3, HIGH);
  delay(100);
  digitalWrite(LED_BLANCO_3, LOW);
  digitalWrite(LED_AZUL_4, HIGH);
  delay(100);
  digitalWrite(LED_AZUL_4, LOW);
  digitalWrite(LED_BLANCO_4, HIGH);
  delay(100);
  digitalWrite(LED_BLANCO_4, LOW);
  digitalWrite(LED_AZUL_3, HIGH);
  delay(100);
  digitalWrite(LED_AZUL_3, LOW);
  digitalWrite(LED_BLANCO_2, HIGH);
  delay(100);
  digitalWrite(LED_BLANCO_2, LOW);
}

// Función para manejar la página principal
void handleRoot() {
  String html = "<html><body><h1>Control de Matriz LED 3x3</h1>";
  html += "<p>Animación actual: " + String(currentAnimation) + "</p>";
  html += "<p><a href='/setAnimation?anim=0'>Animación 1: Todos parpadean</a></p>";
  html += "<p><a href='/setAnimation?anim=1'>Animación 2: LED Central RGB</a></p>";
  html += "<p><a href='/setAnimation?anim=2'>Animación 3: Luz en movimiento</a></p>";
  html += "<p><a href='/setAnimation?anim=3'>Animación 4: Fade In/Out</a></p>";
  html += "<p><a href='/setAnimation?anim=4'>Animación 5: Luces persiguiéndose</a></p>";
  html += "<p><a href='/toggleAutoMode'>Cambiar a " + String(autoMode ? "manual" : "automático") + "</a></p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Función para cambiar la animación actual
void handleSetAnimation() {
  if (server.hasArg("anim")) {
    currentAnimation = server.arg("anim").toInt();
    autoMode = false;  // Cambiar a modo manual
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

// Función para cambiar entre modo automático y manual
void handleToggleAutoMode() {
  autoMode = !autoMode;
  server.sendHeader("Location", "/");
  server.send(303);
}
