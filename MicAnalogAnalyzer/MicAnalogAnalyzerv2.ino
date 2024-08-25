#include <math.h>

const int microfonoPin = 39; // GPIO39 (entrada analógica)
const float alpha = 0.3;     // Factor de suavizado para el filtro IIR
const int numeroMuestras = 1000;
const unsigned long tiempoCalentamiento = 10000; // 10 segundos de calentamiento

// Variables para la medición actual
int valorMax = 0;
int valorMin = 4095;
long sumador = 0;
float sumaCuadrados = 0;
float filteredValue = 0.0;  // Variable para almacenar el valor filtrado

// Variables globales
int valorMaxGlobal = 0;
int valorMinGlobal = 4095;
long sumadorGlobal = 0;
long totalMuestras = 0;
float sumaCuadradosGlobal = 0;
bool inicializado = false;
unsigned long tiempoInicio;

void setup() {
  Serial.begin(115200); // Inicia la comunicación serial para ver resultados
  tiempoInicio = millis(); // Guarda el tiempo inicial
}

void loop() {
  if (!inicializado) {
    // Fase de calentamiento
    if (millis() - tiempoInicio < tiempoCalentamiento) {
      Serial.println("Calentamiento...");
      delay(1000); // Espera 1 segundo antes de leer el siguiente valor
      return;     // No realiza mediciones globales durante el calentamiento
    }
    inicializado = true;
    Serial.println("Inicio de medición.");
  }

  // Medición actual
  valorMax = 0;
  valorMin = 4095;
  sumador = 0;
  sumaCuadrados = 0;

  for (int i = 0; i < numeroMuestras; i++) {
    int lectura = analogRead(microfonoPin); // Lee el valor del micrófono

    // Aplicar el filtro pasabajos IIR
    filteredValue = alpha * lectura + (1.0 - alpha) * filteredValue;

    int valorFiltrado = round(filteredValue);  // Redondear el valor filtrado para usarlo en cálculos

    if (valorFiltrado > valorMax) {
      valorMax = valorFiltrado;
    }

    if (valorFiltrado < valorMin) {
      valorMin = valorFiltrado;
    }

    sumador += valorFiltrado;
    sumaCuadrados += valorFiltrado * valorFiltrado;

    delay(1); // Ajustar según sea necesario
  }

  int valorPromedio = sumador / numeroMuestras;
  float varianza = (sumaCuadrados / numeroMuestras) - (valorPromedio * valorPromedio);
  float desviacionEstandar = sqrt(varianza);

  // Actualización de estadísticas globales
  if (valorMax > valorMaxGlobal) {
    valorMaxGlobal = valorMax;
  }

  if (valorMin < valorMinGlobal) {
    valorMinGlobal = valorMin;
  }

  sumadorGlobal += sumador;
  sumaCuadradosGlobal += sumaCuadrados;
  totalMuestras += numeroMuestras;

  float promedioGlobal = (float)sumadorGlobal / totalMuestras;
  float varianzaGlobal = (sumaCuadradosGlobal / totalMuestras) - (promedioGlobal * promedioGlobal);
  float desviacionEstandarGlobal = sqrt(varianzaGlobal);

  Serial.println("------- Información del Micrófono -------");
  Serial.println("---- Medición Actual ----");
  Serial.print("Valor máximo: ");
  Serial.println(valorMax);
  Serial.print("Valor mínimo: ");
  Serial.println(valorMin);
  Serial.print("Valor promedio: ");
  Serial.println(valorPromedio);
  Serial.print("Desviación estándar: ");
  Serial.println(desviacionEstandar);
  Serial.print("Valor pico a pico: ");
  Serial.println(valorMax - valorMin);

  Serial.println("---- Estadísticas Globales ----");
  Serial.print("Valor máximo global: ");
  Serial.println(valorMaxGlobal);
  Serial.print("Valor mínimo global: ");
  Serial.println(valorMinGlobal);
  Serial.print("Valor promedio global: ");
  Serial.println(promedioGlobal);
  Serial.print("Desviación estándar global: ");
  Serial.println(desviacionEstandarGlobal);
  Serial.print("Valor pico a pico global: ");
  Serial.println(valorMaxGlobal - valorMinGlobal);

  delay(2000); // Ajusta el tiempo de espera según sea necesario
}
