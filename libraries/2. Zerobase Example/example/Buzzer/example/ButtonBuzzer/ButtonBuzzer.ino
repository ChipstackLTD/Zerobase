/*
 * Đàn piano mini với buzzer và nút nhấn
 * 
 * Mô tả: Chương trình tạo ra một cây đàn piano mini sử dụng 6 nút nhấn và buzzer trên ZeroBase.
 * Mỗi nút nhấn tương ứng với một nốt nhạc cơ bản (Đô, Rê, Mi, Pha, Son, La). Khi nhấn nút,
 * buzzer sẽ phát ra âm thanh với tần số tương ứng của nốt nhạc đó.
 * 
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=91
 */

// Định nghĩa các chân kết nối với nút nhấn
const int BUTTON_PIN_14 = 14;  // Nút nhấn phát âm Đô (C)
const int BUTTON_PIN_15 = 15;  // Nút nhấn phát âm Rê (D)
const int BUTTON_PIN_16 = 16;  // Nút nhấn phát âm Mi (E)
const int BUTTON_PIN_17 = 17;  // Nút nhấn phát âm Pha (F)
const int BUTTON_PIN_18 = 18;  // Nút nhấn phát âm Son (G)
const int BUTTON_PIN_19 = 19;  // Nút nhấn phát âm La (A)

// Định nghĩa chân kết nối với buzzer
const int BUZZER_PIN = 1;  // Buzzer sẽ phát ra âm thanh khi nhấn nút

// Định nghĩa các tần số tương ứng với các nốt nhạc
const int TONE_C = 262;  // Tần số của nốt Đô (C)
const int TONE_D = 294;  // Tần số của nốt Rê (D)
const int TONE_E = 330;  // Tần số của nốt Mi (E)
const int TONE_F = 349;  // Tần số của nốt Pha (F)
const int TONE_G = 392;  // Tần số của nốt Son (G)
const int TONE_A = 440;  // Tần số của nốt La (A)

void setup() {
  // Cấu hình các chân nút nhấn là INPUT_PULLUP để tránh trạng thái lơ lửng
  pinMode(BUTTON_PIN_14, INPUT_PULLUP);
  pinMode(BUTTON_PIN_15, INPUT_PULLUP);
  pinMode(BUTTON_PIN_16, INPUT_PULLUP);
  pinMode(BUTTON_PIN_17, INPUT_PULLUP);
  pinMode(BUTTON_PIN_18, INPUT_PULLUP);
  pinMode(BUTTON_PIN_19, INPUT_PULLUP);

  // Cấu hình chân buzzer là OUTPUT
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // Kiểm tra lần lượt các nút nhấn, nếu nút nào được nhấn, phát ra âm thanh tương ứng
  if (digitalRead(BUTTON_PIN_14) == LOW) {
    tone(BUZZER_PIN, TONE_C);  // Phát nốt Đô (C)
  } else if (digitalRead(BUTTON_PIN_15) == LOW) {
    tone(BUZZER_PIN, TONE_D);  // Phát nốt Rê (D)
  } else if (digitalRead(BUTTON_PIN_16) == LOW) {
    tone(BUZZER_PIN, TONE_E);  // Phát nốt Mi (E)
  } else if (digitalRead(BUTTON_PIN_17) == LOW) {
    tone(BUZZER_PIN, TONE_F);  // Phát nốt Pha (F)
  } else if (digitalRead(BUTTON_PIN_18) == LOW) {
    tone(BUZZER_PIN, TONE_G);  // Phát nốt Son (G)
  } else if (digitalRead(BUTTON_PIN_19) == LOW) {
    tone(BUZZER_PIN, TONE_A);  // Phát nốt La (A)
  } else {
    // Nếu không có nút nào được nhấn, dừng phát âm thanh
    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);  // Đảm bảo buzzer không bị kích hoạt ngoài ý muốn
  }
}