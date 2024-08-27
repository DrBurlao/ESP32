#include <WiFi.h>
#include <WebServer.h>

// Definir los pines para cada LED
#define LED_AZUL_1 14   // Esquina superior izquierda
#define LED_BLANCO_1 27 // Lado superior
#define LED_AZUL_2 16  // Esquina superior derecha
#define LED_BLANCO_2 17 // Lado izquierdo
#define LED_CENTRAL_R 22 // LED Central - Rojo
#define LED_CENTRAL_G 12 // LED Central - Verde
#define LED_CENTRAL_B 13 // LED Central - Azul
#define LED_BLANCO_3 0 // Lado derecho
#define LED_AZUL_3 26  // Esquina inferior izquierda
#define LED_BLANCO_4 1 // Lado inferior
#define LED_AZUL_4 3  // Esquina inferior derecha

// Crear una instancia del servidor web en el puerto 80
WebServer server(80);

unsigned long previousMillis = 0;  // Para el tiempo transcurrido
const long interval = 30000;       // Intervalo de 30 segundos
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
    if (currentAnimation > 19) { // Cambiar entre 20 animaciones (0 a 19)
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
    case 5:
      randomBlink();
      break;
    case 6:
      waveEffect();
      break;
    case 7:
      spiralEffect();
      break;
    case 8:
      colorPulse();
      break;
    case 9:
      bounceEffect();
      break;
    case 10:
      breathingLight();
      break;
    case 11:
      diagonalWave();
      break;
    case 12:
      randomColorChange();
      break;
    case 13:
      flashCorners();
      break;
    case 14:
      alternatingLines();
      break;
    case 15:
      circleEffect();
      break;
    case 16:
      expandingSquare();
      break;
    case 17:
      randomWalk();
      break;
    case 18:
      crossPattern();
      break;
    case 19:
      starPattern();
      break;
  }
}

// Animación 1: Todos los LEDs parpadean
void allBlink() {
  setAllLEDs(HIGH);
  delay(250);
  setAllLEDs(LOW);
  delay(250);
}

// Animación 2: Animar el LED central RGB
void animateCenterLED() {
  static int colorState = 0;
  setAllCentralLEDs(LOW);

  switch (colorState) {
    case 0: digitalWrite(LED_CENTRAL_R, HIGH); break;
    case 1: digitalWrite(LED_CENTRAL_G, HIGH); break;
    case 2: digitalWrite(LED_CENTRAL_B, HIGH); break;
    case 3: digitalWrite(LED_CENTRAL_R, HIGH); digitalWrite(LED_CENTRAL_G, HIGH); break;
    case 4: digitalWrite(LED_CENTRAL_G, HIGH); digitalWrite(LED_CENTRAL_B, HIGH); break;
    case 5: digitalWrite(LED_CENTRAL_R, HIGH); digitalWrite(LED_CENTRAL_B, HIGH); break;
    case 6: setAllCentralLEDs(HIGH); break;
  }

  colorState = (colorState + 1) % 7;
  delay(500);
}

// Animación 3: Efecto de luz en movimiento
void runningLight() {
  int order[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_3, LED_AZUL_4, LED_BLANCO_4, LED_AZUL_3, LED_BLANCO_2};
  for (int i = 0; i < 8; i++) {
    digitalWrite(order[i], HIGH);
    delay(100);
    digitalWrite(order[i], LOW);
  }
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
  int order[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_3, LED_AZUL_4, LED_BLANCO_4, LED_AZUL_3, LED_BLANCO_2};
  for (int i = 0; i < 8; i++) {
    digitalWrite(order[i], HIGH);
    delay(100);
    digitalWrite(order[i], LOW);
  }
}

// Animación 6: LEDs parpadeando de forma aleatoria
void randomBlink() {
  int leds[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_2, LED_CENTRAL_R, LED_CENTRAL_G, LED_CENTRAL_B, LED_BLANCO_3, LED_AZUL_3, LED_BLANCO_4, LED_AZUL_4};
  for (int i = 0; i < 10; i++) {
    int index = random(11);
    digitalWrite(leds[index], HIGH);
    delay(200);
    digitalWrite(leds[index], LOW);
  }
}

// Animación 7: Efecto de onda
void waveEffect() {
  int wave[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_3, LED_AZUL_4, LED_BLANCO_4, LED_AZUL_3, LED_BLANCO_2};
  for (int i = 0; i < 8; i++) {
    digitalWrite(wave[i], HIGH);
    delay(150);
    digitalWrite(wave[i], LOW);
  }
}

// Animación 8: Efecto de espiral
void spiralEffect() {
  int spiral[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_3, LED_AZUL_4, LED_BLANCO_4, LED_AZUL_3, LED_BLANCO_2};
  for (int i = 0; i < 8; i++) {
    digitalWrite(spiral[i], HIGH);
    delay(100);
    digitalWrite(spiral[i], LOW);
  }
}

// Animación 9: Pulsos de color en el LED central
void colorPulse() {
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

// Animación 10: Efecto de rebote
void bounceEffect() {
  int bounce[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_3, LED_AZUL_4, LED_BLANCO_4, LED_AZUL_3, LED_BLANCO_2};
  for (int i = 0; i < 8; i++) {
    digitalWrite(bounce[i], HIGH);
    delay(100);
    digitalWrite(bounce[i], LOW);
  }
  for (int i = 7; i >= 0; i--) {
    digitalWrite(bounce[i], HIGH);
    delay(100);
    digitalWrite(bounce[i], LOW);
  }
}

// Animación 11: Respiración suave de luces
void breathingLight() {
  for (int brightness = 0; brightness <= 255; brightness += 5) {
    analogWrite(LED_AZUL_1, brightness);
    analogWrite(LED_BLANCO_1, brightness);
    analogWrite(LED_AZUL_2, brightness);
    analogWrite(LED_BLANCO_3, brightness);
    analogWrite(LED_AZUL_4, brightness);
    analogWrite(LED_BLANCO_4, brightness);
    analogWrite(LED_AZUL_3, brightness);
    analogWrite(LED_BLANCO_2, brightness);
    delay(50);
  }
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    analogWrite(LED_AZUL_1, brightness);
    analogWrite(LED_BLANCO_1, brightness);
    analogWrite(LED_AZUL_2, brightness);
    analogWrite(LED_BLANCO_3, brightness);
    analogWrite(LED_AZUL_4, brightness);
    analogWrite(LED_BLANCO_4, brightness);
    analogWrite(LED_AZUL_3, brightness);
    analogWrite(LED_BLANCO_2, brightness);
    delay(50);
  }
}

// Animación 12: Onda diagonal
void diagonalWave() {
  int diagonal[] = {LED_AZUL_1, LED_BLANCO_2, LED_AZUL_3, LED_BLANCO_4, LED_AZUL_4, LED_BLANCO_3, LED_AZUL_2, LED_BLANCO_1};
  for (int i = 0; i < 8; i++) {
    digitalWrite(diagonal[i], HIGH);
    delay(100);
    digitalWrite(diagonal[i], LOW);
  }
}

// Animación 13: Cambio de color aleatorio
void randomColorChange() {
  int leds[] = {LED_CENTRAL_R, LED_CENTRAL_G, LED_CENTRAL_B};
  for (int i = 0; i < 3; i++) {
    digitalWrite(leds[i], HIGH);
    delay(random(100, 500));
    digitalWrite(leds[i], LOW);
  }
}

// Animación 14: Parpadeo de las esquinas
void flashCorners() {
  int corners[] = {LED_AZUL_1, LED_AZUL_2, LED_AZUL_3, LED_AZUL_4};
  for (int i = 0; i < 4; i++) {
    digitalWrite(corners[i], HIGH);
    delay(150);
    digitalWrite(corners[i], LOW);
  }
}

// Animación 15: Líneas alternas
void alternatingLines() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_BLANCO_1, HIGH);
    digitalWrite(LED_BLANCO_2, LOW);
    digitalWrite(LED_BLANCO_3, HIGH);
    digitalWrite(LED_BLANCO_4, LOW);
    delay(150);
    digitalWrite(LED_BLANCO_1, LOW);
    digitalWrite(LED_BLANCO_2, HIGH);
    digitalWrite(LED_BLANCO_3, LOW);
    digitalWrite(LED_BLANCO_4, HIGH);
    delay(150);
  }
}

// Animación 16: Efecto de círculo
void circleEffect() {
  int circle[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_3, LED_AZUL_4, LED_BLANCO_4, LED_AZUL_3, LED_BLANCO_2};
  for (int i = 0; i < 8; i++) {
    digitalWrite(circle[i], HIGH);
    delay(100);
    digitalWrite(circle[i], LOW);
  }
}

// Animación 17: Cuadro en expansión
void expandingSquare() {
  digitalWrite(LED_BLANCO_2, HIGH);
  digitalWrite(LED_BLANCO_4, HIGH);
  delay(150);
  digitalWrite(LED_BLANCO_1, HIGH);
  digitalWrite(LED_BLANCO_3, HIGH);
  delay(150);
  digitalWrite(LED_AZUL_1, HIGH);
  digitalWrite(LED_AZUL_2, HIGH);
  digitalWrite(LED_AZUL_3, HIGH);
  digitalWrite(LED_AZUL_4, HIGH);
  delay(150);
  setAllLEDs(LOW);
}

// Animación 18: Caminata aleatoria
void randomWalk() {
  int leds[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_2, LED_BLANCO_3, LED_AZUL_3, LED_BLANCO_4, LED_AZUL_4};
  int previous = -1;
  for (int i = 0; i < 8; i++) {
    int index;
    do {
      index = random(8);
    } while (index == previous);
    digitalWrite(leds[index], HIGH);
    delay(150);
    digitalWrite(leds[index], LOW);
    previous = index;
  }
}

// Animación 19: Patrón de cruz
void crossPattern() {
  digitalWrite(LED_BLANCO_1, HIGH);
  digitalWrite(LED_BLANCO_4, HIGH);
  delay(150);
  digitalWrite(LED_BLANCO_2, HIGH);
  digitalWrite(LED_BLANCO_3, HIGH);
  delay(150);
  digitalWrite(LED_CENTRAL_R, HIGH);
  digitalWrite(LED_CENTRAL_G, HIGH);
  digitalWrite(LED_CENTRAL_B, HIGH);
  delay(150);
  setAllLEDs(LOW);
}

// Animación 20: Patrón de estrella
void starPattern() {
  int star[] = {LED_AZUL_1, LED_AZUL_2, LED_AZUL_3, LED_AZUL_4, LED_CENTRAL_R, LED_CENTRAL_G, LED_CENTRAL_B};
  for (int i = 0; i < 7; i++) {
    digitalWrite(star[i], HIGH);
    delay(100);
    digitalWrite(star[i], LOW);
  }
}

// Función para configurar todos los LEDs
void setAllLEDs(int state) {
  digitalWrite(LED_AZUL_1, state);
  digitalWrite(LED_BLANCO_1, state);
  digitalWrite(LED_AZUL_2, state);
  digitalWrite(LED_BLANCO_2, state);
  digitalWrite(LED_CENTRAL_R, state);
  digitalWrite(LED_CENTRAL_G, state);
  digitalWrite(LED_CENTRAL_B, state);
  digitalWrite(LED_BLANCO_3, state);
  digitalWrite(LED_AZUL_3, state);
  digitalWrite(LED_BLANCO_4, state);
  digitalWrite(LED_AZUL_4, state);
}

// Función para configurar todos los LEDs centrales
void setAllCentralLEDs(int state) {
  digitalWrite(LED_CENTRAL_R, state);
  digitalWrite(LED_CENTRAL_G, state);
  digitalWrite(LED_CENTRAL_B, state);
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
  html += "<p><a href='/setAnimation?anim=5'>Animación 6: Parpadeo aleatorio</a></p>";
  html += "<p><a href='/setAnimation?anim=6'>Animación 7: Efecto de onda</a></p>";
  html += "<p><a href='/setAnimation?anim=7'>Animación 8: Efecto de espiral</a></p>";
  html += "<p><a href='/setAnimation?anim=8'>Animación 9: Pulso de color</a></p>";
  html += "<p><a href='/setAnimation?anim=9'>Animación 10: Efecto de rebote</a></p>";
  html += "<p><a href='/setAnimation?anim=10'>Animación 11: Respiración</a></p>";
  html += "<p><a href='/setAnimation?anim=11'>Animación 12: Onda diagonal</a></p>";
  html += "<p><a href='/setAnimation?anim=12'>Animación 13: Cambio de color</a></p>";
  html += "<p><a href='/setAnimation?anim=13'>Animación 14: Parpadeo esquinas</a></p>";
  html += "<p><a href='/setAnimation?anim=14'>Animación 15: Líneas alternas</a></p>";
  html += "<p><a href='/setAnimation?anim=15'>Animación 16: Efecto de círculo</a></p>";
  html += "<p><a href='/setAnimation?anim=16'>Animación 17: Cuadro en expansión</a></p>";
  html += "<p><a href='/setAnimation?anim=17'>Animación 18: Caminata aleatoria</a></p>";
  html += "<p><a href='/setAnimation?anim=18'>Animación 19: Patrón de cruz</a></p>";
  html += "<p><a href='/setAnimation?anim=19'>Animación 20: Patrón de estrella</a></p>";
  html += "<p><a href='/toggleAutoMode'>Modo: " + String(autoMode ? "Automático" : "Manual") + "</a></p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Función para manejar el cambio de animación
void handleSetAnimation() {
  String anim = server.arg("anim");
  currentAnimation = anim.toInt();
  autoMode = false;  // Desactivar el modo automático cuando se selecciona manualmente
  server.sendHeader("Location", "/");
  server.send(303);
}

// Función para alternar entre modo automático y manual
void handleToggleAutoMode() {
  autoMode = !autoMode;
  server.sendHeader("Location", "/");
  server.send(303);
}
