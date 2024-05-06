#include <Arduino.h>
#include <U8g2lib.h>
 
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);  // High speed I2C


unsigned long tiempoInicioCuentaAtras = millis(); // Almacena el tiempo al iniciar
const long intervaloCuentaAtras = 1000; // Intervalo de un segundo
int cuentaAtras = 122; // Comienza la cuenta atrás desde 120 segundos

int ledPins[] = {8, 9}; // Ajusta los números de pin según tu circuito
int numLeds = 2; // Número de LEDs en el circuito

unsigned long previousMillis = 0; // Variable para almacenar el tiempo anterior
const unsigned long interval = 250; // Intervalo deseado
int ledIndex = 0; // Variable para rastrear el índice del LED

#define BUZZER 7 //Altavoz pin 5 ocupado
unsigned long previousMillisBuzzer = 0;
const unsigned long intervalBuzzer = 500; // Intervalo deseado
bool buzzerState = false; // Estado actual del zumbador

String resultado = ""; //almacena el resultado
unsigned long tiempoUltimaCaptura = 0; // Tiempo de la última captura
bool capturaActiva = false; // Indica si se está capturando señales
unsigned long tiempoEspera = 1000; // Tiempo de espera entre lecturas 

int vecesimpreso = 0;
#define LED 6


void setup(void) {
  u8g2.begin();
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  //Serial.begin(9600);

  capturaActiva = true;

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(BUZZER, OUTPUT);

}

void loop() {
  unsigned long currentMillis = millis(); // Obtiene el tiempo actual
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Actualiza el tiempo anterior
    // Realiza la acción específica (encender o apagar el LED)
    digitalWrite(ledPins[ledIndex], !digitalRead(ledPins[ledIndex]));
    // Avanza al siguiente LED
    ledIndex = (ledIndex + 1) % numLeds;
  }
  if (currentMillis - previousMillisBuzzer >=intervalBuzzer) {
    previousMillisBuzzer = currentMillis; //ACT Tiempo
    // Alterna el estado del zumbador
    buzzerState = !buzzerState;
    if (buzzerState) {
      tone(BUZZER, 1400); // Enciende el zumbador
    } else {
      noTone(BUZZER); // Apaga el zumbador
    }
  }
  if (currentMillis - tiempoInicioCuentaAtras >= intervaloCuentaAtras && cuentaAtras > 0) {
    tiempoInicioCuentaAtras = currentMillis; // Actualiza el tiempo para la próxima cuenta atrás
    cuentaAtras--; // Decrementa la cuenta atrás

    // Prepara el texto para mostrar en la pantalla OLED
    u8g2.clearBuffer();
    char textoCuentaAtras[16];
    sprintf(textoCuentaAtras, "Tiempo: %3d", cuentaAtras);
    u8g2.setFont(u8g2_font_ncenB14_tr); // Establece la fuente
    u8g2.drawStr(0, 15, textoCuentaAtras); // Dibuja el texto de la cuenta atrás

    if (cuentaAtras == 0) {
      // Si la cuenta atrás llega a 0, muestra el mensaje final
      u8g2.clearBuffer();
      u8g2.drawStr(30, 30, "Fin de");
      u8g2.drawStr(26, 45, "tiempo");
      u8g2.sendBuffer();
      while(true); // Bucle infinito para detener el programa
    }
  }
 
  if (capturaActiva) {
    if (currentMillis - tiempoUltimaCaptura >= tiempoEspera) {
      int senal = analogRead(A2);
      digitalWrite(LED, LOW);
      //Serial.println(senal);

      if (senal >= 1020) {//diferentes frecuencias captadas
      resultado += "A";
    } else if (senal >= 990) {
      resultado += "B";
    } else if (senal >= 490) {
      resultado += "C";
    } else if (senal >= 4) {
      resultado += "D";
    }
    String textoCompleto ="Clave: " + resultado;
    //Serial.println(textoCompleto);
    // Muestra el mensaje en la pantalla si no está vacío
    if (resultado.length() > 0) {
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.drawStr(0, 45, textoCompleto.c_str());
    }
    u8g2.sendBuffer(); // Envía el buffer a la pantalla
    if (senal >= 4) {
      digitalWrite(LED, HIGH);
      tiempoUltimaCaptura = currentMillis; // Actualiza el tiempo solo si la señal es mayor o igual a 4
      }
    }
  }

  if (resultado.length() == 4 && vecesimpreso == 0 ) {
    capturaActiva = false;
    vecesimpreso++;
    String textoFinal = "Clave: " + resultado;
    u8g2.clearBuffer();                   // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB14_tr);   // choose a suitable font
    u8g2.drawStr(0, 45, "Analizando"); // write something to the internal memory
    u8g2.sendBuffer();
    noTone(BUZZER); // Apaga el zumbador
    delay(5000); //Tensión
    u8g2.clearBuffer();                   // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB14_tr);   // choose a suitable font
    if(resultado == "CABD") {
      //u8g2.drawStr(0, 45, "Correcto!"); // write something to the internal memory
      u8g2.drawButtonUTF8(62, 40, U8G2_BTN_SHADOW1|U8G2_BTN_HCENTER|U8G2_BTN_BW2, 34,  2,  2, "Correcto!" );
      digitalWrite(LED, LOW);
    } else {
      //u8g2.drawStr(0, 45, "Incorrecto!"); // write something to the internal memory
      u8g2.drawButtonUTF8(62, 40, U8G2_BTN_SHADOW1|U8G2_BTN_HCENTER|U8G2_BTN_BW2, 34,  2,  2, "Incorrecto!" );
    }
    u8g2.sendBuffer();
    while (true); // Bucle infinito
  }
}









