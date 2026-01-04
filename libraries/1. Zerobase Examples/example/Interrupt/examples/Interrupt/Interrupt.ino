/*
* Ví dụ về ngắt ngoại vi
* * Mô tả: Chương trình này minh họa việc sử dụng ngắt ngoại vi để điều khiển một đèn LED
* thông qua một nút nhấn. Khi nút nhấn được nhấn (cạnh xuống), trạng thái của đèn LED sẽ
* được chuyển đổi. Chương trình có tích hợp chức năng chống dội phím (debounce) để đảm bảo
* rằng mỗi lần nhấn nút chỉ kích hoạt một sự kiện.
* */

int btn = 14; // Chân kết nối nút nhấn
int led = 3;  // Chân kết nối đèn LED

// Biến trạng thái để chuyển đổi Bật/Tắt đèn LED.
// Sử dụng 'volatile' vì biến này được thay đổi bên trong hàm ngắt.
volatile bool toggleState = false; 

// Biến lưu trữ thời điểm cuối cùng nút nhấn được xử lý để chống dội phím.
// Sử dụng 'volatile' vì biến này được thay đổi bên trong hàm ngắt.
volatile unsigned long lastDebounceTime = 0; 
const unsigned long debounceDelay = 100;   // Thời gian chờ chống dội phím (100 ms)

// Hàm kiểm tra trạng thái nút nhấn và chuyển đổi đèn LED
void checkSwitch() {
  // Ghi lại thời gian hiện tại
  unsigned long currentTime = millis();

  // Kiểm tra xem đã đủ thời gian trôi qua kể từ lần ngắt cuối cùng chưa
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    // Kiểm tra trạng thái nút nhấn (LOW khi nút được nhấn với INPUT_PULLUP)
    if (digitalRead(btn) == LOW) {
      toggleState = !toggleState;       // Đảo ngược trạng thái
      digitalWrite(led, toggleState);   // Đặt trạng thái cho đèn LED
    }

    // Cập nhật thời gian chống dội phím cuối cùng
    lastDebounceTime = currentTime;
  }
}

// Hàm setup chạy một lần khi bạn nhấn reset hoặc cấp nguồn cho board
void setup() {
  // Cấu hình chân đèn LED là OUTPUT
  pinMode(led, OUTPUT);
  // Gắn ngắt vào chân nút nhấn (btn). 
  // Khi chân btn chuyển từ HIGH xuống LOW (cạnh xuống), hàm checkSwitch sẽ được gọi.
  // GPIO_Mode_IPU: Chế độ input pull-up cho chân GPIO.
  // EXTI_Mode_Interrupt: Chế độ ngắt ngoài.
  // EXTI_Trigger_Falling: Kích hoạt ngắt khi có cạnh xuống.
  attachInterrupt(btn, GPIO_Mode_IPU, checkSwitch, EXTI_Mode_Interrupt, EXTI_Trigger_Falling);
}

// Hàm loop chạy lặp đi lặp lại mãi mãi
void loop() {
  // Chương trình chính không làm gì cả, mọi hoạt động được xử lý bằng ngắt.
  delay(5000); // Tạm dừng 5 giây để giảm tải CPU, không bắt buộc nhưng thường thấy.
}