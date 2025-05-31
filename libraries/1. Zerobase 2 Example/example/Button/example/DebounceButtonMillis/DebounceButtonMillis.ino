/* Chống dội nút nhấn sử dụng millis()
 *
 * Mô tả: Chương trình này điều khiển một LED bật/tắt mỗi khi nút nhấn được
 * bấm. Nó sử dụng kỹ thuật chống dội (debounce) dựa trên hàm millis() để
 * theo dõi thời gian, cho phép Zerobase thực hiện các tác vụ khác đồng thời
 * mà không bị gián đoạn.
 *
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Code điều khiển LED bằng nút nhấn có debounce
// Button ở chân D3, LED ở chân D2

// Định nghĩa các chân
const int buttonPin = 3;  // Chân D3 cho nút nhấn
const int ledPin = 2;     // Chân D2 cho đèn LED

// Biến trạng thái
int ledState = LOW;         // Trạng thái hiện tại của LED
int buttonState;            // Trạng thái hiện tại của nút nhấn
int lastButtonState = LOW;  // Trạng thái trước đó của nút nhấn

// Biến thời gian cho debounce
unsigned long lastDebounceTime = 0;  // Thời điểm cuối cùng mà nút nhấn thay đổi trạng thái
unsigned long debounceDelay = 50;    // Thời gian debounce, đơn vị mili giây

void setup() {
  // Cấu hình chân INPUT với điện trở kéo lên cho nút nhấn
  pinMode(buttonPin, INPUT_PULLUP);
  // Cấu hình chân OUTPUT cho LED
  pinMode(ledPin, OUTPUT);

  // Thiết lập trạng thái ban đầu cho LED
  digitalWrite(ledPin, ledState);
}

void loop() {
  // Đọc trạng thái của nút nhấn
  int reading = digitalRead(buttonPin);

  // Kiểm tra nếu có sự thay đổi trạng thái, do nhiễu hoặc nhấn nút
  if (reading != lastButtonState) {
    // Đặt lại bộ đếm thời gian debounce
    lastDebounceTime = millis();
  }

  // Nếu đã qua đủ thời gian debounce, kiểm tra nếu trạng thái nút nhấn thật sự thay đổi
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Nếu trạng thái nút đã thay đổi:
    if (reading != buttonState) {
      buttonState = reading;

      // Chỉ chuyển đổi trạng thái LED nếu nút nhấn được nhấn (LOW vì sử dụng INPUT_PULLUP)
      if (buttonState == LOW) {
        ledState = !ledState;  // Đảo trạng thái LED
      }
    }
  }

  // Cập nhật trạng thái LED
  digitalWrite(ledPin, ledState);

  // Lưu trạng thái nút nhấn cho lần lặp tiếp theo
  lastButtonState = reading;
}