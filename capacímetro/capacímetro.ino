/*
CAPACIMETRO
-----------

DESCRIPCIÓN

- Este programa permite medir la capacidad de condensadores de entre 0,1 uF a más de 10000 uF con un margen de error de 5%.
- Los resultados de las lecturas de los condensadores, se muestran por el puerto serie.
- Los condensadores a medir tienen que ser de al menos 5v.
- Los condensadores a medir tienen que estar completamente descargados o al menos, tienen que estar por debajo de los 5v. De lo contrario, podemos romper el Arduino.

PLATAFORMA

- Este programa a sido probado en un arduino duemilanove.

FUNCIONAMIENTO TEÓRICO DEL PROGRAMA

El siguiente programa mide el tiempo que tarda en cargarse un condensador a través de una resistencia de 3K Ohmios, para determinar la capacidad.
Lo primero que hace el programa es comprobar si el condensador tiene carga eléctrica. Si la tiene, el pin 2 digital se pone en modo 0 lógico y permite que el condensador de descargue completamente.
Cuando el programa detecta que el condensador tiene un voltaje de 0 voltios, pone la salida digital 2 a 1 lógico y alimenta el condensador a través de una resistencia de 3.000 ohmios.
En el mismo momento en el que el programa pone a 1 lógico la salida 2 que permite cargar el condensador, el programa comienza a contar el tiempo.
Cuando la tensión alcanzada en los terminales del condensador alcanza el 63,2% del voltaje de alimentación, el programa para de contar el tiempo y pone la salida 2 a 0 lógico, repitiéndose nuevamente el ciclo de descarga y vuelta a cargar.
El ciclo de carga y descarga se repite cíclicamente y cada vez que se a completado un ciclo, el programa muestra por el puerto serie el resultado de la lectura de capacidad del condensador.

La tensión de alimentación de carga es la de pin digital 2, es decir 5v. 
La tensión de alimentación solo puede llegar al 63,2%, por que hasta esa tensión de la tensión de alimentación, la carga de un condensador es relativamente lineal y es más fácil de calcular la capacidad de un condensador de esta manera.

CONEXIONADO

Se coloca una resistencia de 3000 ohmios en la salida 2 digital al pin 0 analógico y del pin 0 analógico se conectaría el condensador a medir.
El condensador a medir se tendría que conectar del pin 0 analógico a la masa. 
Si se quieren ver los procesos de carga y descarga, solo hay que conectar del pin 2 digital al ánodo de un diodo led y del cátodo del LED a masa, a través de una resistencia de 220 Ohmios.

NOTAS IMPORTANTES
-----------------

- Los condensadores tienen que estar completamente descargados, si están cargados por encima de los 5v, podemos romper el arduino. Sería recomendable poner un diodo zener entre masa y el pin 0 analógico para evitar en lo posible dañar el arduino. La tensión del diodo zener, sería de entre 4 a 5v y de una potencia lo más alta posible.
- El tiempo que tarda en mostrar la lectura dependerá de la capacidad del condensador, es decir que un condensador de 1000 uF tardará unos 10 segundos y un condensador de un 1uF tardará 1000 veces menos.
- Este programa a sido probado y se a comprobado su correcto funcionamiento en la práctica.


<<<< CUIDADO CON LOS CONDENSADORES CARGADOS, UN CONDENSADOR CARGADO CON CIENTOS DE VOLTIOS ES POTENCIALMENTE PELIGROSO, SI VA A MEDIR CONDENSADORES DE VOLTAJES ALTOS, COMPRUEBE CON UN POLÍMETRO QUE ESTÁN DESCARGADOS ANTES DE MANIPULARLOS >>>>
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AUTOR: Mario R.P - mariorp@terra.es - 13-6-2012


*/

long sensorValue; // Variable que contendrá la tensión medida en el pin analógico 0.
long carga=0; // 0 = descargar 1 = cargando - Esta variable es una variable interna del programa y es la que establece si se empieza a cargar o descargar el condensador.
long tiempocomienzocarga; // Esta variable contiene el tiempo en microsegundos en el que se empezó a cargar el condensador.
float tiempofincarga; // Este variable es la que contiene el tiempo en la que se finalizó la carga del condensador.
void setup() {                
 
 pinMode(2, OUTPUT);  // Se establece el pin 2 digital, como pin de salida de datos
 pinMode(A0, INPUT); // Se establece que el pin 0 analógico es un pin de entrada de datos 
 digitalWrite(2, LOW);   // Se pone el pin digital 2 a 0 lógico
 Serial.begin(9600); // se establece a 9600bps la comunicación por el puerto serie
 Serial.println("CAPACIMETRO"); // Muestra por el puerto serie el mensaje CAPACIMETRO
 Serial.println("-----------");  // Muestra por el puerto serie el mensaje ----------------- en otra linea de texto
 Serial.println(" "); // Muestra un espacio en blanco.
}

void loop() {
 
 sensorValue = analogRead(A0); // Se asigna a la variable "sensorValue" la tensión leída por el puerto serie
 
 if(sensorValue==0 && carga==0) // Si la tensión del condensador es igual a 0v y la variable "carga" = 0, comienza la carga del condensador y se inicia el conteo de tiempo
 {
 digitalWrite(2, HIGH); //La salida digital 2 pasa a 1 lógico y comienza a cargar el condensador
 tiempocomienzocarga=micros(); //Se almacena el tiempo en el que se empezó a cargar el condensador en la variable "tiempocomienzocarga" usando la variable de sistema micros, que es una variable de sistema que devuelve el tiempo en microsegundos que lleva el arduino encendido
 carga=1; // la variable carga pasa a estado 1
 }


 if(sensorValue>=647 && carga==1) // si la tensión en el condensador a llegado al 63,2% y variable carga es igual a 1, se detiene la carga y se calcula en base al tiempo, la capacidad del condensador
 {
 tiempofincarga=micros()-tiempocomienzocarga; // La variable "tiempofincarga" es el resultado de restar el tiempo de inicio de carga, con el de fin
 Serial.println(" "); // Se imprime por el puerto serie el valor calculado de la capacidad del condensador
 Serial.print("Capacidad uF:");
 Serial.print((tiempofincarga/3350)-0.04);  // aquí se hace el cálculo definitivo, en base al tiempo de carga.
 digitalWrite(2, LOW); //La salida lógica 2 se pone a 0 lógico y comienza la descarga del condensador.
 carga=0; // la variable carga se pone a 0 lógico.
 }
 
 
}
