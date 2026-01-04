
/*
 * Điều khiển nút nhấn nâng cao với debounce và phát hiện đa dạng kiểu nhấn
 * 
 * Mô tả: Chương trình phát hiện và phân biệt các kiểu nhấn nút khác nhau trên ZeroBase:
 * - Single Press: Nhấn một lần nhanh
 * - Double Press: Nhấn hai lần liên tiếp trong khoảng thời gian ngắn
 * - Long Press: Giữ nút trong thời gian dài (≥1 giây)
 * Sử dụng kỹ thuật debounce để loại bỏ nhiễu và millis() để đo thời gian chính xác.
 * 
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=89
 */

// Chân kết nối nút nhấn
const int buttonPin = 2;

// Biến lưu thời điểm thay đổi trạng thái gần nhất (cho debounce)
unsigned long lastDebounceTime = 0;
// Khoảng thời gian debounce (loại bỏ nhiễu khi nhấn nút)
unsigned long debounceDelay = 50;

// Biến lưu thời điểm nhấn và nhả nút
unsigned long buttonDownTime = 0;
unsigned long buttonUpTime = 0;

// Thời điểm lần nhấn gần nhất để kiểm tra double press
unsigned long lastButtonPress = 0;

// Trạng thái hiện tại của nút
bool buttonState = HIGH;
// Trạng thái trước đó của nút
bool lastButtonState = HIGH;

// Cờ để kiểm tra đã xử lý nhấn giữ chưa
bool longPressDetected = false;
// Cờ để chờ xem có nhấn lần thứ 2 không (double click)
bool waitingForDouble = false;

// Thời gian cần giữ để tính là long press (ms)
const unsigned long longPressTime = 1000;
// Khoảng thời gian tối đa giữa 2 lần nhấn để tính là double press (ms)
const unsigned long doubleClickGap = 400;

void setup() {
  // Cấu hình chân nút nhấn là INPUT_PULLUP (nút nối GND)
  pinMode(buttonPin, INPUT_PULLUP);

  // Khởi tạo Serial1 với tốc độ 9600 baud
  Serial1.begin(9600);
}

void loop() {
  // Đọc trạng thái nút hiện tại (LOW khi nhấn, HIGH khi nhả)
  bool reading = digitalRead(buttonPin);

  // Nếu trạng thái hiện tại khác trạng thái trước đó → khả năng bị nhiễu hoặc nhấn thật
  if (reading != lastButtonState) {
    // Lưu lại thời điểm thay đổi để debounce
    lastDebounceTime = millis();
  }

  // Nếu đã qua thời gian debounce thì xác nhận đây là thay đổi hợp lệ
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Nếu trạng thái thay đổi thật sự
    if (reading != buttonState) {
      // Cập nhật trạng thái mới
      buttonState = reading;

      // Nếu nút vừa được nhấn xuống
      if (buttonState == LOW) {
        buttonDownTime = millis();  // Lưu thời điểm nhấn
        longPressDetected = false;  // Reset cờ long press

      } else {                                                        // Nếu nút vừa được nhả ra
        buttonUpTime = millis();                                      // Lưu thời điểm nhả
        unsigned long pressDuration = buttonUpTime - buttonDownTime;  // Tính thời gian giữ

        // Nếu giữ đủ lâu → long press
        if (pressDuration >= longPressTime) {
          Serial1.println("Long Press");
          longPressDetected = true;
        } else {
          // Nếu đang chờ nhấn lần 2 → kiểm tra khoảng cách giữa 2 lần nhấn
          if (waitingForDouble) {
            if (millis() - lastButtonPress <= doubleClickGap) {
              Serial1.println("Double Press");
              waitingForDouble = false;
            }
          } else {
            // Nếu là lần nhấn đầu → chờ xem có lần 2 không
            waitingForDouble = true;
            lastButtonPress = millis();
          }
        }
      }
    }
  }

  // Nếu sau một thời gian mà không có lần nhấn thứ 2 → xác nhận là single press
  if (waitingForDouble && (millis() - lastButtonPress > doubleClickGap)) {
    Serial1.println("Single Press");
    waitingForDouble = false;
  }

  // Cập nhật trạng thái trước đó cho vòng lặp sau
  lastButtonState = reading;
}