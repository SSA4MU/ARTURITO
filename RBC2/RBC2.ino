
// CONTROLES
//   w = avanzar     s = retroceder
//   a = izquierda   d = derecha
//   q / espacio     = parar
//   1 = potencia baja    2 = potencia media    3 = potencia alta



#include <SoftwareSerial.h>
#include "display_wrapper.h" 

// --------------------- BLUETOOTH ---------------------
const int BT_RX_PIN = 11; // conectado al TXD del módulo Bluetooth
const int BT_TX_PIN = 12; // conectado al RXD del módulo Bluetooth
SoftwareSerial bluetooth(BT_RX_PIN, BT_TX_PIN);

// --------------------- MOTORES---------------------
const int IN1 = 6; // Motor A adelante
const int IN2 = 5; // Motor A atrás
const int IN3 = 4; // Motor B adelante
const int IN4 = 3; // Motor B atrás

// ENA/ENB deben ir en pines PWM (~).
const int ENA = 9; 
const int ENB = 10; 

const uint8_t VELOCIDAD_BAJA  = 90;
const uint8_t VELOCIDAD_MEDIA = 138;
const uint8_t VELOCIDAD_ALTA  = 150;

uint8_t velocidad_actual = VELOCIDAD_MEDIA;

char dato;

enum Estado {
  ESTADO_DESPIERTO,
  ESTADO_DORMIDO
};

Estado estado_actual = ESTADO_DORMIDO; // arranca dormido (no se ha movido aún)

void aplicar_velocidad()
{
  analogWrite(ENA, velocidad_actual);
  analogWrite(ENB, velocidad_actual);
}

void avanzar()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  despertar();
}

void retroceder()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  despertar();
}

void girar_izquierda()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  despertar();
}

void girar_derecha()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  despertar();
}

void parar()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  dormir();
}


void manejar_control()
{
  char dato = 0;

  if (bluetooth.available() > 0)
  {
    dato = bluetooth.read();
  }
  else if (Serial.available() > 0)
  {
    dato = Serial.read();
  }
  else
  {
    return; 
  }

  switch (dato)
  {
    case 'w': avanzar(); break;
    case 's': retroceder(); break;
    case 'a': girar_izquierda(); break;
    case 'd': girar_derecha(); break;
    case 'q':
    case 'Q':
    case ' ': parar(); break;
    case '1': velocidad_actual = VELOCIDAD_BAJA;  aplicar_velocidad(); break;
    case '2': velocidad_actual = VELOCIDAD_MEDIA; aplicar_velocidad(); break;
    case '3': velocidad_actual = VELOCIDAD_ALTA;  aplicar_velocidad(); break;
    default: break; // ignorar cualquier otro caracter
  }
}

// --------------------- OJOS (PANTALLA OLED) ---------------------
const int REF_EYE_HEIGHT = 40;
const int REF_EYE_WIDTH = 40;
const int REF_SPACE_BETWEEN_EYE = 10;
const int REF_CORNER_RADIUS = 10;

struct EyeState {
  int height;
  int width;
  int x;
  int y;
};

EyeState left_eye, right_eye;
int corner_radius = REF_CORNER_RADIUS;

unsigned long next_blink_time = 0;
unsigned long next_move_time = 0;

// --- Máquina de estados de la animación ---
enum AnimState {
  ANIM_IDLE,
  ANIM_BLINK_CLOSING,
  ANIM_BLINK_OPENING,
  ANIM_LOOK_MOVING,
  ANIM_LOOK_HOLDING,
  ANIM_LOOK_RETURNING
};

AnimState anim_state = ANIM_IDLE;
unsigned long anim_last_step = 0;
int anim_step = 0;

const int BLINK_STEPS = 3;
const int BLINK_SPEED = 14;
const unsigned long BLINK_FRAME_INTERVAL = 15; // ms entre frames del parpadeo

const int LOOK_AMPLITUDE_X = 6;
const int LOOK_AMPLITUDE_Y = 4;
const unsigned long LOOK_HOLD_TIME = 300; // ms que la mirada se mantiene desviada

int look_dir_x = 0;
int look_dir_y = 0;

int calculate_safe_radius(int r, int w, int h) {
    if (w < 2 * (r + 1)) r = (w / 2) - 1;
    if (h < 2 * (r + 1)) r = (h / 2) - 1;
    return (r < 0) ? 0 : r;
}

void draw_eyes()
{
    if (estado_actual == ESTADO_DORMIDO)
    {
        int w = REF_EYE_WIDTH + 4;
        int h = 6;
        int cy = SCREEN_HEIGHT / 2;
        int left_cx = SCREEN_WIDTH / 2 - REF_EYE_WIDTH / 2 - REF_SPACE_BETWEEN_EYE / 2;
        int right_cx = SCREEN_WIDTH / 2 + REF_EYE_WIDTH / 2 + REF_SPACE_BETWEEN_EYE / 2;
        g_draw_filled_round_rect(left_cx - w / 2, cy - h / 2, w, h, h / 2, G_COLOR_WHITE);
        g_draw_filled_round_rect(right_cx - w / 2, cy - h / 2, w, h, h / 2, G_COLOR_WHITE);
        return;
    }

    int r_left = calculate_safe_radius(corner_radius, left_eye.width, left_eye.height);
    int x_left = int(left_eye.x - left_eye.width / 2);
    int y_left = int(left_eye.y - left_eye.height / 2);
    g_draw_filled_round_rect(x_left, y_left, left_eye.width, left_eye.height, r_left, G_COLOR_WHITE);

    int r_right = calculate_safe_radius(corner_radius, right_eye.width, right_eye.height);
    int x_right = int(right_eye.x - right_eye.width / 2);
    int y_right = int(right_eye.y - right_eye.height / 2);
    g_draw_filled_round_rect(x_right, y_right, right_eye.width, right_eye.height, r_right, G_COLOR_WHITE);
}

void draw_frame() {
    g_clear_display();
    draw_eyes();
    g_update_display();
}

void reset_eyes(bool update = true)
{
  left_eye.height = REF_EYE_HEIGHT;
  left_eye.width = REF_EYE_WIDTH;
  right_eye.height = REF_EYE_HEIGHT;
  right_eye.width = REF_EYE_WIDTH;

  left_eye.x = SCREEN_WIDTH / 2 - REF_EYE_WIDTH / 2 - REF_SPACE_BETWEEN_EYE / 2;
  left_eye.y = SCREEN_HEIGHT / 2;
  right_eye.x = SCREEN_WIDTH / 2 + REF_EYE_WIDTH / 2 + REF_SPACE_BETWEEN_EYE / 2;
  right_eye.y = SCREEN_HEIGHT / 2;

  corner_radius = REF_CORNER_RADIUS;

  if (update) draw_frame();
}


void dormir()
{
  estado_actual = ESTADO_DORMIDO;
  anim_state = ANIM_IDLE;
  draw_frame();
}

// Despierta al robot: vuelve a ojos normales y reprograma el
// parpadeo/mirada para que no ocurran de inmediato.
void despertar()
{
  if (estado_actual == ESTADO_DESPIERTO) return; 
  estado_actual = ESTADO_DESPIERTO;
  reset_eyes();
  next_blink_time = millis() + random(2000, 5000);
  next_move_time  = millis() + random(3000, 4000);
}

// Inicia un parpadeo si no hay otra animación en curso
void start_blink()
{
  if (anim_state != ANIM_IDLE) return; 

  anim_state = ANIM_BLINK_CLOSING;
  anim_step = 0;
  anim_last_step = millis();
}

// Inicia un movimiento de mirada si no hay otra animación en curso
void start_look(int direction_x, int direction_y)
{
  if (anim_state != ANIM_IDLE) return;

  look_dir_x = direction_x;
  look_dir_y = direction_y;
  anim_state = ANIM_LOOK_MOVING;
  anim_last_step = millis();
}

// Debe llamarse en cada vuelta de loop(). Avanza la animación
// un paso cuando corresponde, sin bloquear nunca la ejecución.
void update_animation()
{
  unsigned long now = millis();

  switch (anim_state)
  {
    case ANIM_IDLE:
      // nada que hacer
      break;

    case ANIM_BLINK_CLOSING:
      if (now - anim_last_step >= BLINK_FRAME_INTERVAL)
      {
        anim_last_step = now;
        left_eye.height -= BLINK_SPEED;
        right_eye.height -= BLINK_SPEED;
        int current_h = left_eye.height;
        int mapped_radius = map(current_h, 4, REF_EYE_HEIGHT, 1, REF_CORNER_RADIUS);
        corner_radius = min(mapped_radius, current_h / 2);
        left_eye.width += 3;
        right_eye.width += 3;
        draw_frame();

        anim_step++;
        if (anim_step >= BLINK_STEPS)
        {
          anim_state = ANIM_BLINK_OPENING;
          anim_step = 0;
        }
      }
      break;

    case ANIM_BLINK_OPENING:
      if (now - anim_last_step >= BLINK_FRAME_INTERVAL)
      {
        anim_last_step = now;
        left_eye.height += BLINK_SPEED;
        right_eye.height += BLINK_SPEED;
        int current_h = left_eye.height;
        int mapped_radius = map(current_h, 4, REF_EYE_HEIGHT, 1, REF_CORNER_RADIUS);
        corner_radius = min(mapped_radius, current_h / 2);
        left_eye.width -= 3;
        right_eye.width -= 3;
        draw_frame();

        anim_step++;
        if (anim_step >= BLINK_STEPS)
        {
          reset_eyes();
          anim_state = ANIM_IDLE;
          next_blink_time = millis() + random(2000, 5000);
        }
      }
      break;

    case ANIM_LOOK_MOVING:
      left_eye.x += LOOK_AMPLITUDE_X * look_dir_x;
      right_eye.x += LOOK_AMPLITUDE_X * look_dir_x;
      left_eye.y += LOOK_AMPLITUDE_Y * look_dir_y;
      right_eye.y += LOOK_AMPLITUDE_Y * look_dir_y;
      draw_frame();

      anim_last_step = now;
      anim_state = ANIM_LOOK_HOLDING;
      break;

    case ANIM_LOOK_HOLDING:
      if (now - anim_last_step >= LOOK_HOLD_TIME)
      {
        anim_state = ANIM_LOOK_RETURNING;
      }
      break;

    case ANIM_LOOK_RETURNING:
      left_eye.x -= LOOK_AMPLITUDE_X * look_dir_x;
      right_eye.x -= LOOK_AMPLITUDE_X * look_dir_x;
      left_eye.y -= LOOK_AMPLITUDE_Y * look_dir_y;
      right_eye.y -= LOOK_AMPLITUDE_Y * look_dir_y;
      draw_frame();

      anim_state = ANIM_IDLE;
      next_move_time = millis() + random(3000, 8000);
      break;
  }
}

// --------------------- SETUP / LOOP ---------------------
void setup()
{
  Serial.begin(9600);
  bluetooth.begin(9600); // la mayoría de módulos HC-05/HC-06 vienen así de fábrica

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  g_init_display();
  randomSeed(analogRead(A0));

  aplicar_velocidad(); // fija la velocidad inicial en ENA/ENB
  parar();              // arrancar siempre en estado seguro (detenido y dormido)
}

void loop()
{
  // 1) Motores: se revisa en cada vuelta, nunca se bloquea
  manejar_control();

  // 2) Animación de ojos: máquina de estados, tampoco bloquea
  unsigned long now = millis();

  if (estado_actual == ESTADO_DESPIERTO)
  {
    if (now >= next_blink_time)
    {
      start_blink();
    }

    if (now >= next_move_time)
    {
      int dir_x = random(-2, 2); // -1, 0 o 1
      int dir_y = random(-2, 2);
      if (dir_x != 0 || dir_y != 0)
      {
        start_look(dir_x, dir_y);
      }
      else
      {
        // no hubo movimiento real, reprograma para no reintentar en bucle
        next_move_time = millis() + random(3000, 8000);
      }
    }

    update_animation();
  }
}
