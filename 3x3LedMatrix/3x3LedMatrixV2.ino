#include <Arduino.h>

// Definición de pines
#define LED_AZUL_1 5
#define LED_BLANCO_1 13
#define LED_AZUL_2 12
#define LED_BLANCO_2 14
#define LED_CENTRAL_R 18
#define LED_CENTRAL_G 19
#define LED_CENTRAL_B 23
#define LED_BLANCO_3 27
#define LED_AZUL_3 16
#define LED_BLANCO_4 17
#define LED_AZUL_4 25

// Definición de tiempos
#define RETARDO 300
#define FADE_DELAY 10
#define MAX_BRIGHTNESS 255
#define CAMBIO_PATROON_INTERVALO 10000
#define CASCADA_INTERVALO 100
#define RESPIRACION_DELAY 20
#define ESTRELLA_INTERVALO 200
#define LLUVIA_ESTRELLAS_INTERVALO 100
#define MODO_TEMPO 20000
#define DIANA_INTERVALO 150
#define ALARMA_INTERVALO 100
#define OLA_INTERVALO 100
#define NIEBLA_INTERVALO 50
#define GRADUAL_COLOR_INTERVALO 50
#define ARCOIRIS_INTERVALO 30
#define EXPLOSION_COLOR_INTERVALO 50
#define FLAMA_INTERVALO 100
#define NEBULOSA_INTERVALO 100
#define ESPIRAL_INTERVALO 50
#define VORTICE_INTERVALO 100
#define CORAZON_INTERVALO 500
#define RUIDO_COLOR_INTERVALO 20
#define CAMBIO_RAPIDO_COLOR_INTERVALO 30
#define ONDA_COLOR_INTERVALO 50
#define PULSAR_INTERVALO 100
#define RASTRO_ESTRELLA_INTERVALO 150
#define NIEBLA_COLOR_INTERVALO 100
#define MOVIMIENTO_COLOR_INTERVALO 50

unsigned long lastChange = 0;
unsigned long lastCascada = 0;
unsigned long lastRespiracion = 0;
unsigned long lastEstrella = 0;
unsigned long lastLluviaEstrellas = 0;
unsigned long lastModoTempo = 0;
unsigned long lastDiana = 0;
unsigned long lastAlarma = 0;
unsigned long lastOla = 0;
unsigned long lastNiebla = 0;
unsigned long lastGradualColor = 0;
unsigned long lastArcoiris = 0;
unsigned long lastExplosionColor = 0;
unsigned long lastFlama = 0;
unsigned long lastNebulosa = 0;
unsigned long lastEspiral = 0;
unsigned long lastVortice = 0;
unsigned long lastCorazon = 0;
unsigned long lastRuidoColor = 0;
unsigned long lastCambioRapidoColor = 0;
unsigned long lastOndaColor = 0;
unsigned long lastPulsar = 0;
unsigned long lastRastroEstrella = 0;
unsigned long lastNieblaColor = 0;
unsigned long lastMovimientoColor = 0;

int modoPatron = 0;
int cascadaPos = 0;
int estrellaPos = 0;
int dianaPos = 0;
int olaPos = 0;
int nieblaBrillo = 0;
int arcoirisHue = 0;
int explosionColor = 0;
bool estrellaDirec = true;
int espiralPos = 0;
int vorticeHue = 0;
int corazonBrillo = 0;
int ruidoColorBrillo = 0;
int ondaColorHue = 0;
int pulsarBrillo = 0;
int rastroEstrellaPos = 0;
int nieblaColorHue = 0;
int movimientoColorPos = 0;

void setup() {
  // Configuración de pines como salidas
  pinMode(LED_AZUL_1, OUTPUT);
  pinMode(LED_BLANCO_1, OUTPUT);
  pinMode(LED_AZUL_2, OUTPUT);
  pinMode(LED_BLANCO_2, OUTPUT);
  pinMode(LED_BLANCO_3, OUTPUT);
  pinMode(LED_AZUL_3, OUTPUT);
  pinMode(LED_BLANCO_4, OUTPUT);
  pinMode(LED_AZUL_4, OUTPUT);

  // Inicialización de LEDs (todos apagados)
  apagarLeds();

  // Inicialización del PWM para el LED RGB central
  ledcSetup(0, 5000, 8); // Canal 0, 5kHz, 8 bits de resolución
  ledcAttachPin(LED_CENTRAL_R, 0);
  ledcSetup(1, 5000, 8); // Canal 1, 5kHz, 8 bits de resolución
  ledcAttachPin(LED_CENTRAL_G, 1);
  ledcSetup(2, 5000, 8); // Canal 2, 5kHz, 8 bits de resolución
  ledcAttachPin(LED_CENTRAL_B, 2);
}

void loop() {
  unsigned long currentMillis = millis();

  // Cambia de patrón automáticamente basado en el tiempo
  if (currentMillis - lastModoTempo >= MODO_TEMPO) {
    modoPatron = (modoPatron + 1) % 20; // Alterna entre 20 patrones
    lastModoTempo = currentMillis;
  }

  // Ejecuta el patrón seleccionado
  switch (modoPatron) {
    case 0:
      efectoParpadeo();
      break;
    case 1:
      efectoFade();
      break;
    case 2:
      efectoColoresAleatorios();
      break;
    case 3:
      efectoCascada();
      break;
    case 4:
      efectoEstrellaFugaz();
      break;
    case 5:
      efectoLluviaEstrellas();
      break;
    case 6:
      efectoDiana();
      break;
    case 7:
      efectoAlarma();
      break;
    case 8:
      efectoArcoiris();
      break;
    case 9:
      efectoExplosionColor();
      break;
    case 10:
      efectoFlama();
      break;
    case 11:
      efectoNebulosa();
      break;
    case 12:
      efectoEspiral();
      break;
    case 13:
      efectoVortice();
      break;
    case 14:
      efectoCorazon();
      break;
    case 15:
      efectoRuidoColor();
      break;
    case 16:
      efectoCambioRapidoColor();
      break;
    case 17:
      efectoOndaColor();
      break;
    case 18:
      efectoPulsar();
      break;
    case 19:
      efectoRastroEstrella();
      break;
  }

  // Efecto de Niebla Colorida
  unsigned long currentNieblaColor = millis();
  if (currentNieblaColor - lastNieblaColor >= NIEBLA_COLOR_INTERVALO) {
    lastNieblaColor = currentNieblaColor;
    efectoNieblaColor();
  }

  // Efecto de Movimiento de Color
  unsigned long currentMovimientoColor = millis();
  if (currentMovimientoColor - lastMovimientoColor >= MOVIMIENTO_COLOR_INTERVALO) {
    lastMovimientoColor = currentMovimientoColor;
    efectoMovimientoColor();
  }
}

void efectoParpadeo() {
  // Parpadeo sincronizado en todos los LEDs
  encenderLeds();
  delay(RETARDO);
  apagarLeds();
  delay(RETARDO);
}

void efectoFade() {
  // Efecto de fundido en el LED RGB central
  for (int brightness = 0; brightness <= MAX_BRIGHTNESS; brightness += 5) {
    setColor(brightness, 0, 0);  // Rojo
    delay(FADE_DELAY);
  }
  for (int brightness = MAX_BRIGHTNESS; brightness >= 0; brightness -= 5) {
    setColor(brightness, 0, 0);  // Rojo
    delay(FADE_DELAY);
  }
  for (int brightness = 0; brightness <= MAX_BRIGHTNESS; brightness += 5) {
    setColor(0, brightness, 0);  // Verde
    delay(FADE_DELAY);
  }
  for (int brightness = MAX_BRIGHTNESS; brightness >= 0; brightness -= 5) {
    setColor(0, brightness, 0);  // Verde
    delay(FADE_DELAY);
  }
  for (int brightness = 0; brightness <= MAX_BRIGHTNESS; brightness += 5) {
    setColor(0, 0, brightness);  // Azul
    delay(FADE_DELAY);
  }
  for (int brightness = MAX_BRIGHTNESS; brightness >= 0; brightness -= 5) {
    setColor(0, 0, brightness);  // Azul
    delay(FADE_DELAY);
  }
}

void efectoColoresAleatorios() {
  // Colores aleatorios en el LED RGB central
  setColor(random(0, MAX_BRIGHTNESS), random(0, MAX_BRIGHTNESS), random(0, MAX_BRIGHTNESS));
  delay(RETARDO);
}

void efectoCascada() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastCascada >= CASCADA_INTERVALO) {
    lastCascada = currentMillis;

    // Apagar todos los LEDs
    apagarLeds();

    // Crear efecto de cascada
    int leds[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_2, LED_BLANCO_3, LED_AZUL_3, LED_BLANCO_4, LED_AZUL_4};
    digitalWrite(leds[cascadaPos], HIGH);

    // Avanzar posición en la cascada
    cascadaPos = (cascadaPos + 1) % 8;
  }
}

void efectoEstrellaFugaz() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastEstrella >= ESTRELLA_INTERVALO) {
    lastEstrella = currentMillis;

    // Apagar todos los LEDs
    apagarLeds();

    // Crear efecto de estrella fugaz
    int leds[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_2, LED_BLANCO_3, LED_AZUL_3, LED_BLANCO_4, LED_AZUL_4};
    digitalWrite(leds[estrellaPos], HIGH);

    // Mover la estrella fugaz
    estrellaPos = estrellaDirec ? (estrellaPos + 1) % 8 : (estrellaPos - 1 + 8) % 8;

    // Cambiar dirección cuando llegue a los bordes
    if (estrellaPos == 0 || estrellaPos == 7) {
      estrellaDirec = !estrellaDirec;
    }
  }
}

void efectoLluviaEstrellas() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastLluviaEstrellas >= LLUVIA_ESTRELLAS_INTERVALO) {
    lastLluviaEstrellas = currentMillis;

    // Apagar todos los LEDs
    apagarLeds();

    // Crear efecto de lluvia de estrellas
    int numEstrellas = 3; // Número de estrellas en la lluvia
    for (int i = 0; i < numEstrellas; i++) {
      int led = random(0, 8);
      int leds[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_2, LED_BLANCO_3, LED_AZUL_3, LED_BLANCO_4, LED_AZUL_4};
      digitalWrite(leds[led], HIGH);
    }
  }
}

void efectoDiana() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastDiana >= DIANA_INTERVALO) {
    lastDiana = currentMillis;

    // Apagar todos los LEDs
    apagarLeds();

    // Crear efecto de diana
    int leds[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_2, LED_BLANCO_3, LED_AZUL_3, LED_BLANCO_4, LED_AZUL_4};
    int numLeds = sizeof(leds) / sizeof(leds[0]);

    // Encender LEDs en patrón circular
    digitalWrite(leds[dianaPos], HIGH);
    digitalWrite(leds[(dianaPos + 1) % numLeds], HIGH);
    digitalWrite(leds[(dianaPos + 2) % numLeds], HIGH);
    
    // Mover la posición de la diana
    dianaPos = (dianaPos + 1) % numLeds;
  }
}

void efectoAlarma() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastAlarma >= ALARMA_INTERVALO) {
    lastAlarma = currentMillis;

    // Alternar LEDs para crear un efecto de alarma
    static bool alarmaEstado = false;
    alarmaEstado = !alarmaEstado;

    if (alarmaEstado) {
      encenderLeds();
    } else {
      apagarLeds();
    }
  }
}

void efectoOla() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastOla >= OLA_INTERVALO) {
    lastOla = currentMillis;

    // Apagar todos los LEDs
    apagarLeds();

    // Crear efecto de ola
    int leds[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_2, LED_BLANCO_3, LED_AZUL_3, LED_BLANCO_4, LED_AZUL_4};
    int numLeds = sizeof(leds) / sizeof(leds[0]);

    // Encender LEDs en patrón de ola
    digitalWrite(leds[(olaPos + 0) % numLeds], HIGH);
    digitalWrite(leds[(olaPos + 1) % numLeds], HIGH);
    digitalWrite(leds[(olaPos + 2) % numLeds], HIGH);

    // Mover la ola
    olaPos = (olaPos + 1) % numLeds;
  }
}

void efectoNiebla() {
  // Efecto de luz de niebla en el LED RGB Central
  nieblaBrillo = (nieblaBrillo + 5) % (MAX_BRIGHTNESS + 1);
  setColor(nieblaBrillo, nieblaBrillo, nieblaBrillo);
  delay(NIEBLA_INTERVALO);
}

void efectoGradualColor() {
  // Cambio gradual de color en el LED RGB Central
  static int r = 0, g = 0, b = 0;
  r = (r + 1) % (MAX_BRIGHTNESS + 1);
  g = (g + 2) % (MAX_BRIGHTNESS + 1);
  b = (b + 3) % (MAX_BRIGHTNESS + 1);
  setColor(r, g, b);
  delay(GRADUAL_COLOR_INTERVALO);
}

void efectoArcoiris() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastArcoiris >= ARCOIRIS_INTERVALO) {
    lastArcoiris = currentMillis;

    // Generar un color en el espectro del arcoíris
    int r = sin(3.14 * arcoirisHue / 180.0) * MAX_BRIGHTNESS;
    int g = sin(3.14 * (arcoirisHue + 120) / 180.0) * MAX_BRIGHTNESS;
    int b = sin(3.14 * (arcoirisHue + 240) / 180.0) * MAX_BRIGHTNESS;

    setColor(r, g, b);

    arcoirisHue = (arcoirisHue + 5) % 360;  // Avanzar en el espectro del arcoíris
  }
}

void efectoExplosionColor() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastExplosionColor >= EXPLOSION_COLOR_INTERVALO) {
    lastExplosionColor = currentMillis;

    // Cambiar a un color aleatorio brillante
    int r = random(0, MAX_BRIGHTNESS);
    int g = random(0, MAX_BRIGHTNESS);
    int b = random(0, MAX_BRIGHTNESS);
    setColor(r, g, b);
  }
}

void efectoFlama() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastFlama >= FLAMA_INTERVALO) {
    lastFlama = currentMillis;

    // Generar un efecto de flama
    int r = random(150, 255);
    int g = random(50, 150);
    int b = 0;
    setColor(r, g, b);
  }
}

void efectoNebulosa() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastNebulosa >= NEBULOSA_INTERVALO) {
    lastNebulosa = currentMillis;

    // Generar un color difuso
    int r = random(50, 100);
    int g = random(50, 100);
    int b = random(50, 100);
    setColor(r, g, b);
  }
}

void efectoEspiral() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastEspiral >= ESPIRAL_INTERVALO) {
    lastEspiral = currentMillis;

    // Apagar todos los LEDs
    apagarLeds();

    // Crear efecto de espiral
    int leds[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_2, LED_BLANCO_3, LED_AZUL_3, LED_BLANCO_4, LED_AZUL_4};
    int numLeds = sizeof(leds) / sizeof(leds[0]);

    // Encender LEDs en patrón de espiral
    digitalWrite(leds[espiralPos], HIGH);
    espiralPos = (espiralPos + 1) % numLeds;
  }
}

void efectoVortice() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastVortice >= VORTICE_INTERVALO) {
    lastVortice = currentMillis;

    // Apagar todos los LEDs
    apagarLeds();

    // Crear efecto de vórtice
    int numLeds = 8;
    int leds[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_2, LED_BLANCO_3, LED_AZUL_3, LED_BLANCO_4, LED_AZUL_4};
    int index = (vorticeHue / 45) % numLeds;
    digitalWrite(leds[index], HIGH);
    digitalWrite(leds[(index + 1) % numLeds], HIGH);

    vorticeHue = (vorticeHue + 10) % 360;
  }
}

void efectoCorazon() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastCorazon >= CORAZON_INTERVALO) {
    lastCorazon = currentMillis;

    // Apagar todos los LEDs
    apagarLeds();

    // Crear efecto de corazón (latido)
    corazonBrillo = (corazonBrillo + 5) % (MAX_BRIGHTNESS + 1);
    setColor(corazonBrillo, 0, 0); // Rojo intenso
  }
}

void efectoRuidoColor() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastRuidoColor >= RUIDO_COLOR_INTERVALO) {
    lastRuidoColor = currentMillis;

    // Generar un color aleatorio
    int r = random(0, MAX_BRIGHTNESS);
    int g = random(0, MAX_BRIGHTNESS);
    int b = random(0, MAX_BRIGHTNESS);
    setColor(r, g, b);
  }
}

void efectoCambioRapidoColor() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastCambioRapidoColor >= CAMBIO_RAPIDO_COLOR_INTERVALO) {
    lastCambioRapidoColor = currentMillis;

    // Cambiar a un color aleatorio brillante
    int r = random(0, MAX_BRIGHTNESS);
    int g = random(0, MAX_BRIGHTNESS);
    int b = random(0, MAX_BRIGHTNESS);
    setColor(r, g, b);
  }
}

void efectoOndaColor() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastOndaColor >= ONDA_COLOR_INTERVALO) {
    lastOndaColor = currentMillis;

    // Generar un color en forma de onda
    int r = sin(3.14 * ondaColorHue / 180.0) * MAX_BRIGHTNESS;
    int g = sin(3.14 * (ondaColorHue + 120) / 180.0) * MAX_BRIGHTNESS;
    int b = sin(3.14 * (ondaColorHue + 240) / 180.0) * MAX_BRIGHTNESS;

    setColor(r, g, b);

    ondaColorHue = (ondaColorHue + 10) % 360;  // Avanzar en el espectro de onda de color
  }
}

void efectoPulsar() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastPulsar >= PULSAR_INTERVALO) {
    lastPulsar = currentMillis;

    // Efecto de pulso en el LED RGB central
    pulsarBrillo = (pulsarBrillo + 5) % (MAX_BRIGHTNESS + 1);
    setColor(pulsarBrillo, pulsarBrillo / 2, pulsarBrillo / 2); // Rojo con brillo pulsante
  }
}

void efectoRastroEstrella() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastRastroEstrella >= RASTRO_ESTRELLA_INTERVALO) {
    lastRastroEstrella = currentMillis;

    // Apagar todos los LEDs
    apagarLeds();

    // Crear efecto de rastro de estrella
    int leds[] = {LED_AZUL_1, LED_BLANCO_1, LED_AZUL_2, LED_BLANCO_2, LED_BLANCO_3, LED_AZUL_3, LED_BLANCO_4, LED_AZUL_4};
    int numLeds = sizeof(leds) / sizeof(leds[0]);

    // Encender LEDs en patrón de rastro
    for (int i = 0; i <= rastroEstrellaPos; i++) {
      digitalWrite(leds[i % numLeds], HIGH);
    }

    // Mover la posición del rastro
    rastroEstrellaPos = (rastroEstrellaPos + 1) % numLeds;
  }
}

void efectoNieblaColor() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastNieblaColor >= NIEBLA_COLOR_INTERVALO) {
    lastNieblaColor = currentMillis;

    // Generar un color de niebla
    int r = random(100, 150);
    int g = random(100, 150);
    int b = random(100, 150);
    setColor(r, g, b);
  }
}

void efectoMovimientoColor() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastMovimientoColor >= MOVIMIENTO_COLOR_INTERVALO) {
    lastMovimientoColor = currentMillis;

    // Crear efecto de movimiento de color en el LED RGB Central
    int r = random(0, MAX_BRIGHTNESS);
    int g = random(0, MAX_BRIGHTNESS);
    int b = random(0, MAX_BRIGHTNESS);
    setColor(r, g, b);
  }
}

void setColor(int red, int green, int blue) {
  ledcWrite(0, red);
  ledcWrite(1, green);
  ledcWrite(2, blue);
}

void apagarLeds() {
  // Apagar todos los LEDs
  digitalWrite(LED_AZUL_1, LOW);
  digitalWrite(LED_BLANCO_1, LOW);
  digitalWrite(LED_AZUL_2, LOW);
  digitalWrite(LED_BLANCO_2, LOW);
  digitalWrite(LED_BLANCO_3, LOW);
  digitalWrite(LED_AZUL_3, LOW);
  digitalWrite(LED_BLANCO_4, LOW);
  digitalWrite(LED_AZUL_4, LOW);
}

void encenderLeds() {
  // Encender todos los LEDs
  digitalWrite(LED_AZUL_1, HIGH);
  digitalWrite(LED_BLANCO_1, HIGH);
  digitalWrite(LED_AZUL_2, HIGH);
  digitalWrite(LED_BLANCO_2, HIGH);
  digitalWrite(LED_BLANCO_3, HIGH);
  digitalWrite(LED_AZUL_3, HIGH);
  digitalWrite(LED_BLANCO_4, HIGH);
  digitalWrite(LED_AZUL_4, HIGH);
}
