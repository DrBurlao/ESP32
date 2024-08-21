#define ZUMBADOR_PIN1 12 // Primer zumbador
#define ZUMBADOR_PIN2 13 // Segundo zumbador
#define LED_VERDE_PIN 17 // Pin del nuevo LED verde

void setup() {
  pinMode(ZUMBADOR_PIN1, OUTPUT);
  pinMode(ZUMBADOR_PIN2, OUTPUT);
  pinMode(LED_VERDE_PIN, OUTPUT);

  // Sonido de pulsación de teclado numérico al inicio (más graves y lentos)
  for (int i = 0; i < 3; i++) {
    tone(ZUMBADOR_PIN1, 500, 200);
    digitalWrite(LED_VERDE_PIN, HIGH); // Enciende el LED verde
    delay(250);
    noTone(ZUMBADOR_PIN1);
    digitalWrite(LED_VERDE_PIN, LOW); // Apaga el LED verde
    delay(150);
  }
}

void loop() {
  // Duración total del ciclo en milisegundos
  int duracion_total = 30000; // 30 segundos
  
  // Tiempo inicial
  unsigned long tiempo_inicial = millis();

  // Sonido de confirmación de activación (más graves y lentos)
  tone(ZUMBADOR_PIN1, 300, 600);
  digitalWrite(LED_VERDE_PIN, HIGH); // Enciende el LED verde
  delay(700);
  noTone(ZUMBADOR_PIN1);
  digitalWrite(LED_VERDE_PIN, LOW); // Apaga el LED verde
  delay(500);

  // Sonido de la bomba de Counter-Strike
  int duracion_corto = 200; // Duración inicial del pulso corto (en milisegundos)
  int duracion_largo = 400; // Duración inicial del pulso largo (en milisegundos)
  int decremento = 5; // Cantidad de decremento en cada iteración
  int sonido_explosion = 1000; // Duración del sonido de la explosión

  // Reproducir el sonido en los dos zumbadores alternativamente
  while (millis() - tiempo_inicial < duracion_total) {
    // Pulsos cortos en el primer zumbador (graves)
    digitalWrite(ZUMBADOR_PIN1, HIGH);
    delay(duracion_corto);
    digitalWrite(ZUMBADOR_PIN1, LOW);
    delay(duracion_corto);

    // Pulsos largos en el segundo zumbador (agudos)
    digitalWrite(ZUMBADOR_PIN2, HIGH);
    delay(duracion_largo);
    digitalWrite(ZUMBADOR_PIN2, LOW);
    delay(duracion_largo);

    // Reducir la duración de los pulsos cortos y largos
    duracion_corto -= decremento;
    duracion_largo -= decremento;

    // Asegurar que la duración no sea menor que 50 ms
    duracion_corto = max(duracion_corto, 50);
    duracion_largo = max(duracion_largo, 50);

    // Incrementar el decremento al llegar al 75% del tiempo total
    if (millis() - tiempo_inicial > duracion_total * 0.75) {
      decremento = 15;
    }
  }

  // Simular la explosión al final
  digitalWrite(ZUMBADOR_PIN1, HIGH);
  digitalWrite(ZUMBADOR_PIN2, HIGH);
  delay(sonido_explosion);
  digitalWrite(ZUMBADOR_PIN1, LOW);
  digitalWrite(ZUMBADOR_PIN2, LOW);

  // Sonido de confirmación de explosión
  for (int i = 0; i < 2; i++) {
    tone(ZUMBADOR_PIN1, 1500, 150);
    delay(200);
    noTone(ZUMBADOR_PIN1);
    delay(100);
  }

  delay(2000); // Pausa antes de iniciar un nuevo ciclo
}
