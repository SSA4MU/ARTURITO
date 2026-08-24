# 🤖 Bluetooth Robot with Animated Eyes

[🇬🇧 English](#-english) · [🇪🇸 Español](#-español)

---

## 🇬🇧 English

Bluetooth-controlled robot car (Arduino Uno + L298N + HC-05), with an OLED screen simulating a pair of animated eyes: it blinks, looks around randomly, and "falls asleep" when the robot isn't in use. Includes a web control page with **press-and-hold to move** logic.

<br>

### ✨ Features

- Movement control (forward / backward / left / right) over classic Bluetooth (HC-05).
- 3 fixed power levels (low / medium / high), capped to avoid damaging motors rated for ~6.5V while running off a 12V supply.
- Animated eyes on a 128x64 SSD1306 OLED screen: random blinking and look-around movement, fully **non-blocking** (based on `millis()`, not `delay()`), so the animation never interrupts motor control.
- **Sleep/awake** state: while asleep, the robot ignores every command except the one that wakes it up, and shows closed eyes. It starts asleep by default for safety.
- Boot splash screen (fixed logo for 5 seconds) on power-up.
- Web control page (`control_robot.html`) with **press-and-hold to move** buttons, a power selector, and a sleep/wake toggle — connects via the Web Serial API to an HC-05 already paired with the PC.

<br>

### 🧰 Hardware needed

| Component | Notes |
|---|---|
| Arduino Uno or Nano | |
| L298N motor driver | |
| 2x DC motors | rated for ~6.5V nominal |
| HC-05 (or HC-06) Bluetooth module | Classic Bluetooth, not BLE |
| 128x64 SSD1306 OLED screen (I2C) | |
| Motor power supply | 12V (power is capped in software via PWM) |

<br>

### 🔌 Wiring

#### Motors (L298N)

| L298N | Arduino |
|---|---|
| IN1 | Pin 6 |
| IN2 | Pin 5 |
| IN3 | Pin 4 |
| IN4 | Pin 3 |
| ENA | Pin 9 (PWM) |
| ENB | Pin 10 (PWM) |

#### Bluetooth module (HC-05)

| HC-05 | Arduino |
|---|---|
| VCC | 5V |
| GND | GND |
| TXD | Pin 11 *(ideally through a 5V→3.3V voltage divider)* |
| RXD | Pin 12 |

> `SoftwareSerial` is used on pins 11/12 on purpose, to keep the USB Serial port (pins 0/1) free for debugging over the Serial Monitor while Bluetooth is active.

#### OLED screen (I2C)

| OLED | Arduino |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

<br>

### 📁 Project files

```
robot_completo/
├── robot_completo.ino    # Main sketch (motors + eyes + Bluetooth)
├── display_wrapper.h     # Display abstraction layer (Adafruit_SSD1306 / U8g2)
├── config.h              # Display library selection and screen dimensions
└── control_robot.html    # Web control page (Web Serial API)
```

**To upload the sketch:** create a folder named exactly `robot_completo` and place the 3 `.ino`/`.h` files inside (the folder name must match the main `.ino` file's name, as required by the Arduino IDE).

<br>

### 📚 Required libraries

Install from the Arduino IDE Library Manager:

- `Adafruit SSD1306`
- `Adafruit GFX Library`
- `SoftwareSerial` (included by default)

If you'd rather use `U8g2` instead of Adafruit, switch the corresponding definition in `config.h`.

<br>

### 🎮 Commands (Bluetooth/Serial protocol)

| Command | Action |
|---|---|
| `w` | Move forward (while held) |
| `s` | Move backward (while held) |
| `a` | Turn left (while held) |
| `d` | Turn right (while held) |
| `q` / space | Stop motors |
| `1` | Low power (~4.2V) |
| `2` | Medium power (~6.5V, recommended) |
| `3` | High power (~7.8V, use in short bursts only) |
| `z` | Toggle sleep/awake |

> While the robot is **asleep**, it ignores every command except `z`. That's the only way to wake it up.

<br>

### 🌐 Web control page

`control_robot.html` uses the **Web Serial API** (not Web Bluetooth), because the HC-05 is classic Bluetooth, not BLE — browsers can only talk to it through an operating-system serial port, not by connecting to it directly over Bluetooth.

**Steps to use it:**

1. Pair the HC-05 from your PC's Bluetooth settings (typical PIN: `1234` or `0000`). This creates a serial port (COM on Windows, `/dev/tty.*` on Mac).
2. Open `control_robot.html` in **Chrome or Edge** (Web Serial doesn't work in Safari or Firefox).
3. Tap "Conectar robot" and pick the port that corresponds to the HC-05.
4. Press and hold the arrow buttons to move; releasing sends the stop command automatically.
5. Use the power buttons and the 😴/⏸️ toggle to put the robot to sleep or wake it up.

<br>

### ⚠️ Safety notes

- Power is software-limited (via PWM) to a maximum of ~7.8V from a 12V supply, to protect motors rated for ~6.5V nominal. If you're using different motors, adjust `VELOCIDAD_BAJA`, `VELOCIDAD_MEDIA`, and `VELOCIDAD_ALTA` in `robot_completo.ino`.
- The robot starts **asleep and stopped** by default, and `dormir()` always stops the motors before showing closed eyes — it should never end up moving without control.

<br>

### 🛠️ Possible future improvements

- Visual confirmation on the web page of the robot's actual state (currently the sleep/awake state is assumed client-side, not read back from the Arduino).
- Support for a BLE module (HM-10) as an alternative, to allow control from an iPhone (Web Bluetooth doesn't work with a classic HC-05).
- Native Android app using classic Bluetooth (compatible with HC-05), e.g. built with MIT App Inventor.

<br><br>

---

## 🇪🇸 Español

Carrito robot controlado por Bluetooth (Arduino Uno + L298N + HC-05), con una pantalla OLED que simula un par de ojos animados: parpadea, mira alrededor al azar, y se "duerme" cuando el robot no se está usando. Incluye una página web de control con lógica de **mantener presionado para moverse**.

<br>

### ✨ Características

- Control de movimiento (adelante / atrás / izquierda / derecha) por Bluetooth clásico (HC-05).
- 3 niveles de potencia fijos (bajo / medio / alto), acotados para no dañar motores pensados para ~6.5V con una fuente de 12V.
- Ojos animados en una pantalla OLED SSD1306 128x64: parpadeo y movimiento de mirada aleatorios, totalmente **no bloqueantes** (basados en `millis()`, no en `delay()`), para que la animación nunca interrumpa el control de los motores.
- Estado **dormido/despierto**: mientras duerme, el robot ignora cualquier comando (excepto el que lo despierta) y muestra los ojos cerrados. Arranca dormido por seguridad.
- Pantalla de carga (logo fijo 5 segundos) al encender.
- Página web de control (`control_robot.html`) con botones de **mantener presionado para moverse**, selector de potencia, e interruptor de dormir/despertar — conecta por Web Serial API a un HC-05 ya emparejado con la PC.

<br>

### 🧰 Hardware necesario

| Componente | Notas |
|---|---|
| Arduino Uno o Nano | |
| Driver de motores L298N | |
| 2x motores DC | pensados para ~6.5V nominales |
| Módulo Bluetooth HC-05 (o HC-06) | Bluetooth clásico, no BLE |
| Pantalla OLED SSD1306 128x64 (I2C) | |
| Fuente de alimentación para motores | 12V (la potencia se limita por software vía PWM) |

<br>

### 🔌 Conexiones

#### Motores (L298N)

| L298N | Arduino |
|---|---|
| IN1 | Pin 6 |
| IN2 | Pin 5 |
| IN3 | Pin 4 |
| IN4 | Pin 3 |
| ENA | Pin 9 (PWM) |
| ENB | Pin 10 (PWM) |

#### Módulo Bluetooth (HC-05)

| HC-05 | Arduino |
|---|---|
| VCC | 5V |
| GND | GND |
| TXD | Pin 11 *(idealmente con divisor de voltaje 5V→3.3V)* |
| RXD | Pin 12 |

> Se usa `SoftwareSerial` en los pines 11/12 a propósito, para dejar libre el puerto Serial por USB (pines 0/1) y poder depurar por el Monitor Serial mientras el Bluetooth funciona.

#### Pantalla OLED (I2C)

| OLED | Arduino |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

<br>

### 📁 Archivos del proyecto

```
robot_completo/
├── robot_completo.ino    # Sketch principal (motores + ojos + Bluetooth)
├── display_wrapper.h     # Capa de abstracción de la pantalla (Adafruit_SSD1306 / U8g2)
├── config.h              # Selección de librería de pantalla y dimensiones
└── control_robot.html    # Página web de control (Web Serial API)
```

**Para subir el sketch:** crea una carpeta llamada exactamente `robot_completo` y coloca ahí los 3 archivos `.ino`/`.h` (el nombre de la carpeta debe coincidir con el del `.ino` principal, como exige el IDE de Arduino).

<br>

### 📚 Librerías requeridas

Instalar desde el Gestor de Librerías del IDE de Arduino:

- `Adafruit SSD1306`
- `Adafruit GFX Library`
- `SoftwareSerial` (incluida por defecto)

Si prefieres usar `U8g2` en vez de Adafruit, cambia la definición correspondiente en `config.h`.

<br>

### 🎮 Comandos (protocolo Bluetooth/Serial)

| Comando | Acción |
|---|---|
| `w` | Avanzar (mientras se mantiene) |
| `s` | Retroceder (mientras se mantiene) |
| `a` | Girar izquierda (mientras se mantiene) |
| `d` | Girar derecha (mientras se mantiene) |
| `q` / espacio | Detener motores |
| `1` | Potencia baja (~4.2V) |
| `2` | Potencia media (~6.5V, recomendada) |
| `3` | Potencia alta (~7.8V, úsala en tramos cortos) |
| `z` | Alternar dormido/despierto |

> Mientras el robot está **dormido**, ignora todos los comandos excepto `z`. Es la única forma de despertarlo.

<br>

### 🌐 Página web de control

`control_robot.html` usa la **Web Serial API** (no Web Bluetooth) porque el HC-05 es Bluetooth clásico, no BLE — los navegadores solo pueden hablarle a través de un puerto serial del sistema operativo, no conectándose directo por Bluetooth.

**Pasos para usarla:**

1. Empareja el HC-05 desde los ajustes Bluetooth de tu PC (PIN típico: `1234` o `0000`). Esto crea un puerto serial (COM en Windows, `/dev/tty.*` en Mac).
2. Abre `control_robot.html` en **Chrome o Edge** (Web Serial no funciona en Safari ni Firefox).
3. Toca "Conectar robot" y elige el puerto correspondiente al HC-05.
4. Mantén presionadas las flechas para moverte; al soltar, el robot se detiene solo.
5. Usa los botones de potencia y el interruptor 😴/⏸️ para dormir o despertar el robot.

<br>

### ⚠️ Notas de seguridad

- La potencia está limitada por software (PWM) a un máximo de ~7.8V con una fuente de 12V, para proteger motores de ~6.5V nominales. Si usas motores distintos, ajusta `VELOCIDAD_BAJA`, `VELOCIDAD_MEDIA` y `VELOCIDAD_ALTA` en `robot_completo.ino`.
- El robot arranca **dormido y detenido** por defecto, y `dormir()` siempre para los motores antes de mostrar los ojos cerrados — no debería quedar nunca en movimiento sin control.

<br>

### 🛠️ Posibles mejoras futuras

- Confirmación visual en la página web del estado real del robot (actualmente el estado dormido/despierto se asume en el cliente, no se lee de vuelta del Arduino).
- Soporte para un módulo BLE (HM-10) como alternativa, para poder controlar desde iPhone (Web Bluetooth no funciona con HC-05 clásico).
- App nativa para Android usando Bluetooth clásico (compatible con HC-05), por ejemplo con MIT App Inventor.
