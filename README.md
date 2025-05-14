# Kim's underworld
Un Metroidvania que trata sobre adentrarte en las profundidades del inframundo para reunirte con tu difunto padre.

## Integrantes del grupo:

Lead del proyecto: Xavi Chaparro

Lead de desarrollo: Marc Blánquez

Lead de level design: Arnau Balasch

Leads de arte: Clara Rodríguez y Stephanie Wisnar

Departamento de desarrollo: Adrià Belinchón, Isaac Ramírez, Aniol López, Asier Ulloa

Departamento de level design: Martí Mira

Departamento de arte: David Caldés, Alba Espejo, Irene Fuentes, Arnau Guinart, Mar Hernández, Jan Liria, Yazmin Mohamed, Isaac Oliver, Lourdes Pastor

## ¿Qué es?
Kim's underworld es un juego donde, mediante la escalada, puedes ir avanzando en el mapa, visitando el inframundo y luchar contra las fases del duelo de Kim, nuestra protagonista, que acaba de perder a su padre en un accidente trágico causado por su culpa.

## Vertical slice
En esta versión inicial, nos hemos centrado en las mecánicas principales y nos hemos asegurado de que su funcionamiento es correcto. En esta release hemos incluido los siguientes elementos y mecánicas:
- Lanzamiento de piquetas: Esta mecánica sirve tanto para atacar a los enemigos desde la distancia y para colocar las piquetas en unas paredes concretas, donde se podrán usar a las piquetas como plataforma para progresar en el mapa.
- Ataque cuerpo a cuerpo: Tanto si tienes piquetas como si no tienes, se puede atacar. El daño del ataque cambia si tienes piquetas, si no tienes, se hará una animación diferente.
- Puntos de alma: Estos puntos los sueltan los enemigos y algunas rocas repartidas por el mapa. Estos puntos se acercan al jugador y sirven para comprar mejoras (aún no disponibles) para Kim. Tienen diferentes tamaños, que corresponden a la cantidad de puntos que proporcionan al jugador.
- Rocas de alma: Estas rocas sultan puntos de alma al romperse, se usa el ataque del jugador para romperlas. Al romperse,sueltan puntos de alma variables, es decir, que el número nunca será igual.
- Enemigo simple: Contiene animaciones básicas y un comportamiento que sigue una lógica que funciona siguiendo al jugador. Tiene estados de idle, movimiento, daño y muerte. Al morir,sueltan puntos de alma variables, es decir, que el número nunca será igual.
- Jugador avanzado: Kim en el juego se mueve de forma fluida, con saltos que varían según el tiempo que pulses el botón de salto, coyote time para poder saltar unos segundos tras caer de una plataforma, etc. Hemos usado una State Machine para iterar entre los estados de tanto los enemigos como el jugador.
- Checkpoints: Repartidos por el mapa. Sirven para guardar la posición y reaparecer en esa posición tras morir, además de curar la vida del jugador.
- Mapa del tutorial: En este nivel, se usarán las mecánicas base del juego, se le enseñará al jugador a escalar, atacar enemigos y buscar recolectar el mayor número de puntos de alma posibles.
- Mapa de testeo: Desde la pantalla de inicio, se puede acceder a un nivel de testeo. En este nivel probamos mecánicas y fluidez del juego en un ambiente mucho más simplificado, sin tiles avanzados y un tamaño reducido.

## Controles
### Movimiento
- A: Moverse a la izquierda
- D: Moverse a la derecha
- Barra espaciadora: Saltar (La altura del salto varía según el tiempo que mantengas pulsada la tecla)
- Q: Disparar piqueta horizontalmente
- Q + W: Disparar piqueta verticalmente
- E: Ataque cuerpo a cuerpo
### Debug
- F9: Mostrar/Dejar de ver colisiones
- F10: Activar/Desactivar God mode
- F11: Activar/Desactivar juego en 30 FPS
  