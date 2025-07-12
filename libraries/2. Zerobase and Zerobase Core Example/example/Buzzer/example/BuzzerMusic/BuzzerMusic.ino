/*
* Máy phát nhạc với buzzer và điều khiển ngắt
* 
* Mô tả: Chương trình phát giai điệu "Happy Birthday" sử dụng buzzer trên ZeroBase.
* Sử dụng 2 nút nhấn với ngắt ngoài: nút PLAY để bắt đầu phát nhạc và nút STOP để dừng.
* Giai điệu được lưu trong mảng tần số và thời gian, cho phép điều khiển linh hoạt
* việc phát/dừng nhạc bất kỳ lúc nào.
* 
* Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=92
*/

// Định nghĩa chân kết nối với buzzer và nút bấm
const int BUZZER_PIN = 1;   // Chân kết nối với buzzer
const int BUTTON_PLAY = 2;  // Chân kết nối với nút PLAY
const int BUTTON_STOP = 3;  // Chân kết nối với nút STOP

volatile bool isPlaying = false;  // Biến cờ báo hiệu trạng thái phát nhạc

// Mảng chứa các tần số (Hz) của giai điệu
const int melody[] = {
  262, 262, 294, 262, 349, 330,
  262, 262, 294, 262, 392, 349,
  262, 262, 523, 440, 349, 330, 294,
  440, 440, 349, 392, 349
};

// Mảng chứa thời gian phát của từng nốt (ms)
const int durations[] = {
  400, 200, 600, 600, 600, 1200,
  400, 200, 600, 600, 600, 1200,
  400, 200, 600, 600, 600, 600, 1200,
  400, 200, 600, 600, 1200
};

// Xử lý ngắt cho nút PLAY (D2)
void checkSwitch_PLAY(void) {
  EXTI_ClearITPendingBit(EXTI_Line2);  // Xóa cờ ngắt của chân D2
  isPlaying = true;                    // Bật trạng thái phát nhạc
}

// Xử lý ngắt cho nút STOP (D3)
void checkSwitch_STOP(void) {
  EXTI_ClearITPendingBit(EXTI_Line3);  // Xóa cờ ngắt của chân D3
  isPlaying = false;                   // Tắt trạng thái phát nhạc
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);         // Cấu hình chân buzzer là OUTPUT
  pinMode(BUTTON_PLAY, INPUT_PULLUP);  // Cấu hình nút PLAY là INPUT với pull-up nội
  pinMode(BUTTON_STOP, INPUT_PULLUP);  // Cấu hình nút STOP là INPUT với pull-up nội

  // Gán hàm xử lý ngắt cho nút PLAY và STOP, kích hoạt khi có xung xuống (nhấn nút)
  attachInterrupt(BUTTON_PLAY, GPIO_Mode_IPU, checkSwitch_PLAY, EXTI_Mode_Interrupt, EXTI_Trigger_Falling);
  attachInterrupt(BUTTON_STOP, GPIO_Mode_IPU, checkSwitch_STOP, EXTI_Mode_Interrupt, EXTI_Trigger_Falling);
}

void loop() {
  if (isPlaying) {  // Nếu đang trong trạng thái phát nhạc
    for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
      if (!isPlaying) {                 // Kiểm tra nếu nhấn STOP, dừng ngay lập tức
        noTone(BUZZER_PIN);             // Tắt buzzer
        digitalWrite(BUZZER_PIN, LOW);  // Đảm bảo buzzer tắt
        return;                         // Thoát khỏi vòng lặp
      }
      tone(BUZZER_PIN, melody[i], durations[i]);  // Phát nốt nhạc
      delay(durations[i] + 50);                   // Giữ khoảng cách giữa các nốt
    }
    noTone(BUZZER_PIN);             // Dừng phát nhạc sau khi hoàn thành giai điệu
    digitalWrite(BUZZER_PIN, LOW);  // Đảm bảo buzzer tắt
    isPlaying = false;              // Đặt lại trạng thái
  } else {
    noTone(BUZZER_PIN);             // Nếu không phát nhạc, đảm bảo buzzer tắt
    digitalWrite(BUZZER_PIN, LOW);  // Đảm bảo buzzer tắt
  }
}