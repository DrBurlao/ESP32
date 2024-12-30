#include <LiquidCrystal.h>

// Definir los pines conectados al LCD
const int rs = 26;   // Pin RS
const int en = 25;   // Pin Enable
const int d4 = 17;   // Pin D4
const int d5 = 16;   // Pin D5
const int d6 = 27;   // Pin D6
const int d7 = 14;   // Pin D7
const int vo = 32;   // Pin VO (control de contraste, conectado al potenciómetro)

// Crear un objeto LiquidCrystal
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // Inicializar el LCD
  lcd.begin(16, 2);  // Configurar el LCD con 16 columnas y 2 filas
  
  // Configuración inicial de la pantalla LCD
  lcd.setCursor(0, 0);  // Colocar el cursor en la primera columna de la primera fila
  lcd.print("Feliz Navidad");
  lcd.setCursor(0, 1);  // Colocar el cursor en la primera columna de la segunda fila
  lcd.print("ESP32 Test");

  // Ajuste de contraste estático (fijo)
  analogWrite(vo, 128); // Establecer un valor de contraste fijo. Puede ajustarse entre 0 (mínimo contraste) y 255 (máximo contraste)
}

void loop() {
  // No es necesario hacer nada en el loop
}
