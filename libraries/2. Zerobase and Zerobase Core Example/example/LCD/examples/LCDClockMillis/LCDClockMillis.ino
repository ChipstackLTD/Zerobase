/* Đồng hồ sử dụng millis()
 *
 * Mô tả: Chương trình này tạo một đồng hồ kỹ thuật số đơn giản hiển thị
 * giờ, phút, giây trên màn hình LCD I2C. Người dùng có thể điều chỉnh
 * thời gian thông qua 3 nút nhấn (Mode, Up, Down). Thời gian được cập nhật
 * bằng hàm millis() để không chặn các tác vụ khác của Zerobase.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=109
 */

// Thư viện điều khiển LCD sử dụng I2C
#include <LiquidCrystal_I2C.h>

// Khởi tạo đối tượng LCD với địa chỉ I2C là 0x27, LCD 16 cột 2 dòng
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Định nghĩa chân nút nhấn
const int btnMode = 2;
const int btnUp = 3;
const int btnDown = 10;

// Biến lưu trữ thời gian
int hours = 0, minutes = 0, seconds = 0;

// Biến lưu thời điểm trước đó để đo khoảng thời gian trôi qua
unsigned long previousMillis = 0;
// Khoảng thời gian chờ giữa mỗi lần tăng giây (1000ms = 1 giây)
const long interval = 1000;

// Chế độ chỉnh giờ
enum EditMode {
  NONE,        // Không chỉnh (chế độ chạy bình thường)
  SET_HOUR,    // Chỉnh giờ
  SET_MINUTE,  // Chỉnh phút
  SET_SECOND   // Chỉnh giây
};
EditMode mode = NONE;  // Khởi đầu ở chế độ bình thường

// Trạng thái trước đó của các nút nhấn (dùng để phát hiện cạnh xuống)
bool lastBtnModeState = HIGH;
bool lastBtnUpState = HIGH;
bool lastBtnDownState = HIGH;

void setup() {
  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();  // Bật đèn nền

  // Thiết lập các chân nút nhấn là INPUT_PULLUP (mức HIGH mặc định)
  pinMode(btnMode, INPUT_PULLUP);
  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);

  // Hiển thị thông điệp khởi động
  lcd.setCursor(2, 0);       // Cột 2, dòng 0
  lcd.print("Zerobase!!!");  // Dòng chữ đầu
  lcd.setCursor(2, 1);       // Cột 2, dòng 1
  lcd.print("Starting...");  // Dòng chữ thứ hai
  delay(2000);               // Dừng lại 2 giây để người dùng đọc

  lcd.clear();  // Xóa màn hình sau khi hiển thị
}

void loop() {
  unsigned long currentMillis = millis();  // Lấy thời gian hiện tại

  // Nếu không ở chế độ chỉnh và đã trôi qua 1 giây
  if (mode == NONE && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Cập nhật thời điểm
    seconds++;                       // Tăng giây

    // Cập nhật phút và giờ nếu cần
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) hours = 0;
      }
    }
  }

  // Xử lý nút nhấn
  handleButtons();

  // Hiển thị thời gian lên LCD
  displayTime();
}

// Hàm xử lý nút nhấn
void handleButtons() {
  // Đọc trạng thái hiện tại của các nút
  bool btnModeState = digitalRead(btnMode);
  bool btnUpState = digitalRead(btnUp);
  bool btnDownState = digitalRead(btnDown);

  // Nếu nút Mode vừa được nhấn (cạnh xuống)
  if (lastBtnModeState == HIGH && btnModeState == LOW) {
    // Chuyển sang chế độ tiếp theo (vòng tròn)
    mode = (EditMode)((mode + 1) % 4);
  }

  // Nếu nút Up vừa được nhấn
  if (btnUpState == LOW && lastBtnUpState == HIGH) {
    if (mode == SET_HOUR) hours = (hours + 1) % 24;
    else if (mode == SET_MINUTE) minutes = (minutes + 1) % 60;
    else if (mode == SET_SECOND) seconds = (seconds + 1) % 60;
  }

  // Nếu nút Down vừa được nhấn
  if (btnDownState == LOW && lastBtnDownState == HIGH) {
    if (mode == SET_HOUR) hours = (hours + 23) % 24;             // Giảm giờ
    else if (mode == SET_MINUTE) minutes = (minutes + 59) % 60;  // Giảm phút
    else if (mode == SET_SECOND) seconds = (seconds + 59) % 60;  // Giảm giây
  }

  // Cập nhật trạng thái trước đó cho lần lặp sau
  lastBtnModeState = btnModeState;
  lastBtnUpState = btnUpState;
  lastBtnDownState = btnDownState;
}

// Hàm hiển thị thời gian và thông tin chỉnh sửa lên LCD
void displayTime() {
  switch (mode) {
    case SET_HOUR:
      lcd.setCursor(0, 0);
      lcd.print("   ");
      lcd.setCursor(3, 0);
      lcd.print("Edit: Hour          ");
      break;
    case SET_MINUTE:
      lcd.setCursor(0, 0);
      lcd.print("  ");
      lcd.setCursor(2, 0);
      lcd.print("Edit: Minute        ");
      break;
    case SET_SECOND:
      lcd.setCursor(0, 0);
      lcd.print("  ");
      lcd.setCursor(2, 0);
      lcd.print("Edit: Second        ");
      break;
    case NONE:
      lcd.setCursor(0, 0);
      lcd.print(" Zerobase clock          ");
      break;
  }

  // Hiển thị thời gian dưới dòng thứ hai
  lcd.setCursor(4, 1);
  printDigits(hours);
  lcd.print(":");
  printDigits(minutes);
  lcd.print(":");
  printDigits(seconds);
}

// Hàm in số có định dạng 2 chữ số (ví dụ: 01, 09, 12,...)
void printDigits(int digits) {
  if (digits < 10)
    lcd.print("0");  // Thêm số 0 phía trước nếu nhỏ hơn 10
  lcd.print(digits);
}