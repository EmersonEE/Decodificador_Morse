#include <Arduino.h>
#include "Morse.h"

const int BOTON_PIN = D0; // Pin de entrada del botón
int estadoBoton;
unsigned long tiempoInicial = 0;
unsigned long tiempoSoltado = 0;
unsigned long tiempoEntrePulsaciones = 0;
unsigned long tiempoPulsado = 0;
char Lectura[10] = {}; // Buffer para la secuencia Morse de una letra
int punto = 0;
char letra;
String palabra = ""; // Buffer para la palabra completa
String frase = "";

const char *codigoMorse[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."};
const char letras[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void decodificarMorse()
{
  for (int i = 0; i < 26; i++)
  {
    if (strcmp(Lectura, codigoMorse[i]) == 0)
    {
      letra = letras[i];
      Serial.print("Letra: ");
      Serial.println(letra);
      return;
    }
  }
  letra = ' ';
  Serial.println("Espacio En Blanco Entre Palabras");
}

void setup()
{
  pinMode(BOTON_PIN, INPUT);
  Serial.begin(9600);
}

void loop()
{
  estadoBoton = digitalRead(BOTON_PIN);

  if (estadoBoton == HIGH)
  {
    // Botón pulsado
    if (tiempoInicial == 0)
    {
      tiempoInicial = millis(); // Guarda el tiempo inicial
      if (tiempoSoltado != 0)
      {
        tiempoEntrePulsaciones = tiempoInicial - tiempoSoltado;
        Serial.print("Tiempo entre pulsaciones (ms): ");
        Serial.println(tiempoEntrePulsaciones);
        if (tiempoEntrePulsaciones > 250)
        {
          decodificarMorse(); // Decodifica la secuencia actual a letra
          palabra += letra;   // Agrega la letra a la palabra
          punto = 0;          // Reinicia el buffer de la letra
          Lectura[0] = '\0';
          if (tiempoEntrePulsaciones > 1490)
          {
            Serial.print("Palabra: ");
            Serial.println(palabra);
            frase += palabra + " "; // Agrega la palabra a la frase con espacio
            palabra = "";           // Reinicia el buffer de la palabra
          }
          if (tiempoEntrePulsaciones > 3000)
          {
            // Reinicia la frase completa después de un largo tiempo de inactividad
            frase = "";
          }
        }
      }
    }
  }
  else
  {
    // Botón soltado
    if (tiempoInicial != 0)
    {
      tiempoPulsado = millis() - tiempoInicial; // Calcula el tiempo pulsado
      Serial.print("Tiempo pulsado (ms): ");
      Serial.println(tiempoPulsado);
      if (tiempoPulsado > 200 && tiempoPulsado < 400)
      {
        Lectura[punto] = '.';
        punto++;
        Lectura[punto] = '\0';
      }
      else if (tiempoPulsado > 700 && tiempoPulsado < 900)
      {
        Lectura[punto] = '-';
        punto++;
        Lectura[punto] = '\0';
      }
      tiempoInicial = 0;        // Reinicia el tiempo inicial
      tiempoSoltado = millis(); // Guarda el tiempo en que se soltó el botón
      Serial.print("Frase completa: ");
      Serial.println(frase); // Imprime la frase completa
      Serial.print("Secuencia actual: ");
      Serial.println(Lectura);
    }
  }

  // Decodifica la última secuencia si el botón no se pulsa después de un intervalo específico
  if (millis() - tiempoSoltado > 1000 && Lectura[0] != '\0')
  {
    decodificarMorse();
    palabra += letra;
    punto = 0;
    Lectura[0] = '\0';
    Serial.print("Palabra (después de pausa): " + String(frase));
    Serial.println(palabra);
  }
}
