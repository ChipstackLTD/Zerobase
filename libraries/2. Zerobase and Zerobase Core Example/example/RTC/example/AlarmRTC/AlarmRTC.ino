
/*
 * Đồng hồ báo thức với RTC DS3231 và LCD I2C
 * 
 * Mô tả: Hệ thống đồng hồ báo thức sử dụng module RTC DS3231, màn hình LCD I2C 16x2,
 * 3 nút nhấn để cài đặt thời gian và báo thức, có còi báo và hiển thị ngày giờ đầy đủ
 * 
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=112
 */

#include <Wire.h>               // Thư viện Wire để giao tiếp I2C
#include <DS3231.h>             // Thư viện DS3231 để điều khiển module đồng hồ thời gian thực
#include <LiquidCrystal_I2C.h>  // Thư viện điều khiển màn hình LCD qua I2C

// Định nghĩa chân cho các nút nhấn
#define MODE_BTN_PIN 2   // Kết nối đến chân hỗ trợ ngắt
#define UP_BTN_PIN 3     // Kết nối đến chân hỗ trợ ngắt
#define DOWN_BTN_PIN 10  // Chân cho nút giảm
#define BUZZER_PIN 11    // Chân kết nối với còi báo (tùy chọn)

// Cấu hình LCD
#define LCD_I2C_ADDR 0x27  // Địa chỉ I2C tiêu chuẩn cho hầu hết các module LCD I2C
#define LCD_COLS 16        // Số cột của màn hình LCD
#define LCD_ROWS 2         // Số hàng của màn hình LCD

// Tạo các đối tượng
DS3231 rtc;                                                               // Đối tượng đồng hồ thời gian thực
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);                  // Đối tượng màn hình LCD
uint8_t BellChar[] = { 0x04, 0x0e, 0x0a, 0x0a, 0x0a, 0x1f, 0x00, 0x04 };  // Ký tự hình chuông tùy chỉnh

// Các chế độ hoạt động
enum Mode {
  DISPLAY_TIME,     // Hiển thị thời gian bình thường
  SET_HOUR,         // Cài đặt giờ
  SET_MINUTE,       // Cài đặt phút
  SET_SECOND,       // Cài đặt giây
  SET_DAY,          // Cài đặt ngày
  SET_MONTH,        // Cài đặt tháng
  SET_YEAR,         // Cài đặt năm
  SET_DOW,          // Cài đặt thứ trong tuần
  SET_ALARM_HOUR,   // Cài đặt giờ báo thức
  SET_ALARM_MINUTE  // Cài đặt phút báo thức
};

// Biến toàn cục
Mode currentMode = DISPLAY_TIME;  // Chế độ hiện tại, bắt đầu với hiển thị thời gian
bool alarmEnabled = false;        // Trạng thái bật/tắt báo thức
bool alarmTriggered = false;      // Trạng thái báo thức đang kích hoạt
bool timeChanged = false;         // Cờ đánh dấu thời gian đã thay đổi

// Biến báo thức
byte alarmHour = 7;    // Giờ báo thức mặc định
byte alarmMinute = 0;  // Phút báo thức mặc định

// Biến chống dội nút nhấn
unsigned long lastModeButtonPress = 0;     // Thời điểm cuối cùng nhấn nút chế độ
unsigned long lastUpButtonPress = 0;       // Thời điểm cuối cùng nhấn nút tăng
unsigned long lastDownButtonPress = 0;     // Thời điểm cuối cùng nhấn nút giảm
const unsigned long DEBOUNCE_DELAY = 200;  // Thời gian trễ chống dội (mili giây)

// Bộ đệm cho các giá trị thời gian trong quá trình cài đặt
byte tempHour, tempMinute, tempSecond;       // Giờ, phút, giây tạm thời
byte tempDay, tempMonth, tempYear, tempDOW;  // Ngày, tháng, năm, thứ tạm thời

// Hằng số hiển thị thời gian
const char* daysOfWeek[] = {
  "Err", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"  // Mảng tên các ngày trong tuần
};

// Khai báo nguyên mẫu hàm
void handleModeButton();                                            // Xử lý nút chế độ
void handleUpButton();                                              // Xử lý nút tăng
void handleDownButton();                                            // Xử lý nút giảm
void updateRTC();                                                   // Cập nhật đồng hồ thời gian thực
void updateDisplay();                                               // Cập nhật hiển thị
void checkAlarm();                                                  // Kiểm tra báo thức
void setAlarm();                                                    // Thiết lập báo thức
void soundAlarm(bool state);                                        // Điều khiển âm thanh báo thức
void displayTimeSettingMode();                                      // Hiển thị chế độ cài đặt thời gian
void displayAlarmSettingMode();                                     // Hiển thị chế độ cài đặt báo thức
void blinkValue(byte col, byte row, byte value, bool leadingZero);  // Hiển thị giá trị nhấp nháy

void setup() {
  // Khởi tạo I2C
  Wire.begin();

  // Khởi tạo RTC
  rtc.setClockMode(false);  // Đặt chế độ 24 giờ

  // Khởi tạo LCD
  lcd.begin(LCD_COLS, LCD_ROWS);  // Khởi tạo màn hình LCD
  lcd.backlight();                // Bật đèn nền màn hình

  // Khởi tạo các nút nhấn với điện trở kéo lên nội
  pinMode(MODE_BTN_PIN, INPUT_PULLUP);
  pinMode(UP_BTN_PIN, INPUT_PULLUP);
  pinMode(DOWN_BTN_PIN, INPUT_PULLUP);

  // Khởi tạo chân còi báo
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  // Tắt còi báo ban đầu

  // Hiển thị thông báo chào mừng
  lcd.setCursor(1, 0);
  lcd.print("Zerobase Alarm");
  lcd.setCursor(2, 1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();
  lcd.createChar(3, BellChar);  // Tạo ký tự chuông tùy chỉnh

  // Thiết lập báo thức mặc định (7:00 sáng)
  setAlarm();
}

void loop() {
  // Kiểm tra nhấn nút
  if (digitalRead(MODE_BTN_PIN) == LOW) {  // Nếu nút chế độ được nhấn
    handleModeButton();
  }

  if (digitalRead(UP_BTN_PIN) == LOW) {  // Nếu nút tăng được nhấn
    handleUpButton();
  }

  if (digitalRead(DOWN_BTN_PIN) == LOW) {  // Nếu nút giảm được nhấn
    handleDownButton();
  }

  // Cập nhật hiển thị dựa trên chế độ hiện tại
  updateDisplay();

  // Kiểm tra nếu báo thức nên kích hoạt (chỉ trong chế độ hiển thị thời gian)
  if (currentMode == DISPLAY_TIME) {
    checkAlarm();
  }

  // Trễ nhỏ để giảm tải CPU
  delay(50);
}

void handleModeButton() {
  // Xử lý chống dội
  unsigned long currentMillis = millis();
  if (currentMillis - lastModeButtonPress < DEBOUNCE_DELAY) {
    return;  // Bỏ qua nếu thời gian từ lần nhấn trước quá ngắn
  }
  lastModeButtonPress = currentMillis;

  // Xử lý chuyển chế độ
  switch (currentMode) {
    case DISPLAY_TIME:
      // Bắt đầu cài đặt thời gian với giờ
      currentMode = SET_HOUR;

      // Đọc thời gian hiện tại vào biến tạm thời
      bool h12, PM, century;
      tempHour = rtc.getHour(h12, PM);
      tempMinute = rtc.getMinute();
      tempSecond = rtc.getSecond();
      tempDay = rtc.getDate();
      tempMonth = rtc.getMonth(century);
      tempYear = rtc.getYear();
      tempDOW = rtc.getDoW();
      break;

    case SET_HOUR:
      currentMode = SET_MINUTE;  // Chuyển sang cài đặt phút
      break;

    case SET_MINUTE:
      currentMode = SET_SECOND;  // Chuyển sang cài đặt giây
      break;

    case SET_SECOND:
      currentMode = SET_DAY;  // Chuyển sang cài đặt ngày
      break;

    case SET_DAY:
      currentMode = SET_MONTH;  // Chuyển sang cài đặt tháng
      break;

    case SET_MONTH:
      currentMode = SET_YEAR;  // Chuyển sang cài đặt năm
      break;

    case SET_YEAR:
      currentMode = SET_DOW;  // Chuyển sang cài đặt thứ
      break;

    case SET_DOW:
      // Lưu các cài đặt thời gian mới
      updateRTC();
      // Chuyển sang cài đặt báo thức
      currentMode = SET_ALARM_HOUR;
      break;

    case SET_ALARM_HOUR:
      currentMode = SET_ALARM_MINUTE;  // Chuyển sang cài đặt phút báo thức
      break;

    case SET_ALARM_MINUTE:
      // Lưu cài đặt báo thức
      setAlarm();
      // Trở về hiển thị bình thường
      currentMode = DISPLAY_TIME;
      break;
  }

  // Xóa màn hình khi chế độ thay đổi
  lcd.clear();
}

void handleUpButton() {
  // Xử lý chống dội
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpButtonPress < DEBOUNCE_DELAY) {
    return;  // Bỏ qua nếu thời gian từ lần nhấn trước quá ngắn
  }
  lastUpButtonPress = currentMillis;

  timeChanged = true;  // Đánh dấu thời gian đã thay đổi

  // Xử lý tăng giá trị dựa trên chế độ hiện tại
  switch (currentMode) {
    case DISPLAY_TIME:
      // Bật/tắt báo thức
      alarmEnabled = !alarmEnabled;
      if (alarmTriggered) {  // Nếu báo thức đang kích hoạt
        alarmTriggered = false;
        soundAlarm(false);  // Tắt âm thanh báo thức
      }
      break;

    case SET_HOUR:
      tempHour = (tempHour + 1) % 24;  // Tăng giờ từ 0-23
      break;

    case SET_MINUTE:
      tempMinute = (tempMinute + 1) % 60;  // Tăng phút từ 0-59
      break;

    case SET_SECOND:
      tempSecond = (tempSecond + 1) % 60;  // Tăng giây từ 0-59
      break;

    case SET_DAY:
      // Ngày từ 1 đến 31, xử lý ràng buộc theo tháng cụ thể
      tempDay = tempDay % 31 + 1;
      break;

    case SET_MONTH:
      // Tháng từ 1 đến 12
      tempMonth = tempMonth % 12 + 1;
      break;

    case SET_YEAR:
      // Năm từ 0 đến 99 (2000-2099)
      tempYear = (tempYear + 1) % 100;
      break;

    case SET_DOW:
      // Thứ từ 1 (Chủ nhật) đến 7 (Thứ bảy)
      tempDOW = tempDOW % 7 + 1;
      break;

    case SET_ALARM_HOUR:
      alarmHour = (alarmHour + 1) % 24;  // Tăng giờ báo thức từ 0-23
      break;

    case SET_ALARM_MINUTE:
      alarmMinute = (alarmMinute + 1) % 60;  // Tăng phút báo thức từ 0-59
      break;
  }

  // Xóa màn hình sau khi thay đổi
  lcd.clear();
}

void handleDownButton() {
  // Xử lý chống dội
  unsigned long currentMillis = millis();
  if (currentMillis - lastDownButtonPress < DEBOUNCE_DELAY) {
    return;  // Bỏ qua nếu thời gian từ lần nhấn trước quá ngắn
  }
  lastDownButtonPress = currentMillis;

  timeChanged = true;  // Đánh dấu thời gian đã thay đổi

  // Xử lý giảm giá trị dựa trên chế độ hiện tại
  switch (currentMode) {
    case DISPLAY_TIME:
      // Tạm dừng/dừng báo thức nếu đang kích hoạt
      if (alarmTriggered) {
        alarmTriggered = false;
        soundAlarm(false);  // Tắt âm thanh báo thức
      }
      break;

    case SET_HOUR:
      tempHour = (tempHour + 23) % 24;  // Giảm giờ với quay vòng
      break;

    case SET_MINUTE:
      tempMinute = (tempMinute + 59) % 60;  // Giảm phút với quay vòng
      break;

    case SET_SECOND:
      tempSecond = (tempSecond + 59) % 60;  // Giảm giây với quay vòng
      break;

    case SET_DAY:
      tempDay = ((tempDay + 29) % 31) + 1;  // Đảm bảo ngày từ 1-31
      break;

    case SET_MONTH:
      tempMonth = ((tempMonth + 10) % 12) + 1;  // Đảm bảo tháng từ 1-12
      break;

    case SET_YEAR:
      tempYear = (tempYear + 99) % 100;  // Giảm năm với quay vòng
      break;

    case SET_DOW:
      tempDOW = ((tempDOW + 5) % 7) + 1;  // Đảm bảo thứ từ 1-7
      break;

    case SET_ALARM_HOUR:
      alarmHour = (alarmHour + 23) % 24;  // Giảm giờ báo thức với quay vòng
      break;

    case SET_ALARM_MINUTE:
      alarmMinute = (alarmMinute + 59) % 60;  // Giảm phút báo thức với quay vòng
      break;
  }

  // Xóa màn hình sau khi thay đổi
  lcd.clear();
}

void updateRTC() {
  // Cập nhật RTC với các giá trị mới
  rtc.setHour(tempHour);      // Cập nhật giờ
  rtc.setMinute(tempMinute);  // Cập nhật phút
  rtc.setSecond(tempSecond);  // Cập nhật giây
  rtc.setDate(tempDay);       // Cập nhật ngày
  rtc.setMonth(tempMonth);    // Cập nhật tháng
  rtc.setYear(tempYear);      // Cập nhật năm
  rtc.setDoW(tempDOW);        // Cập nhật thứ

  // Đặt lại cờ timeChanged
  timeChanged = false;
}

void updateDisplay() {
  // Hiển thị khác nhau dựa trên chế độ hiện tại
  switch (currentMode) {
    case DISPLAY_TIME:
      displayNormalTime();  // Hiển thị thời gian bình thường
      break;

    case SET_HOUR:
    case SET_MINUTE:
    case SET_SECOND:
    case SET_DAY:
    case SET_MONTH:
    case SET_YEAR:
    case SET_DOW:
      displayTimeSettingMode();  // Hiển thị chế độ cài đặt thời gian
      break;

    case SET_ALARM_HOUR:
    case SET_ALARM_MINUTE:
      displayAlarmSettingMode();  // Hiển thị chế độ cài đặt báo thức
      break;
  }
}

void displayNormalTime() {
  // Biến để đọc thời gian
  bool h12, PM, century;

  // Đọc thời gian hiện tại
  byte second = rtc.getSecond();       // Đọc giây
  byte minute = rtc.getMinute();       // Đọc phút
  byte hour = rtc.getHour(h12, PM);    // Đọc giờ
  byte date = rtc.getDate();           // Đọc ngày
  byte month = rtc.getMonth(century);  // Đọc tháng
  byte year = rtc.getYear();           // Đọc năm
  byte dow = rtc.getDoW();             // Đọc thứ

  // Hiển thị ngày và thứ ở hàng trên
  lcd.setCursor(2, 0);
  lcd.print(daysOfWeek[dow]);  // Hiển thị tên thứ
  lcd.print(" ");

  // Định dạng ngày: DD/MM/YY
  if (date < 10) lcd.print("0");  // Thêm số 0 phía trước nếu ngày < 10
  lcd.print(date);
  lcd.print("/");

  if (month < 10) lcd.print("0");  // Thêm số 0 phía trước nếu tháng < 10
  lcd.print(month);
  lcd.print("/");

  if (year < 10) lcd.print("0");  // Thêm số 0 phía trước nếu năm < 10
  lcd.print(year);

  // Hiển thị thời gian ở hàng dưới
  lcd.setCursor(4, 1);

  // Giờ với số 0 phía trước nếu cần
  if (hour < 10) lcd.print("0");
  lcd.print(hour);
  lcd.print(":");

  // Phút với số 0 phía trước nếu cần
  if (minute < 10) lcd.print("0");
  lcd.print(minute);
  lcd.print(":");

  // Giây với số 0 phía trước nếu cần
  if (second < 10) lcd.print("0");
  lcd.print(second);

  // Hiển thị trạng thái báo thức
  lcd.setCursor(0, 1);
  if (alarmEnabled) {
    lcd.write(byte(3));  // Hiển thị biểu tượng chuông nếu báo thức được bật
  } else {
    lcd.print("  ");  // Hiển thị khoảng trắng nếu báo thức tắt
  }
  // Hiển thị chỉ báo báo thức đang kích hoạt
  if (alarmTriggered) {
    // Nhấp nháy chỉ báo báo thức
    if (second % 2 == 0) {  // Nhấp nháy mỗi giây
      lcd.setCursor(0, 1);
      lcd.print("**");  // Hiển thị ** khi báo thức đang kêu
    }
  }
}

void displayTimeSettingMode() {
  // Hàng trên hiển thị đang cài đặt gì

  switch (currentMode) {
    case SET_HOUR:
      lcd.setCursor(4, 0);
      lcd.print("Set Hour");             // Hiển thị "Cài đặt giờ"
      blinkValue(4, 1, tempHour, true);  // Nhấp nháy giá trị giờ
      break;

    case SET_MINUTE:
      lcd.setCursor(3, 0);
      lcd.print("Set Minute");             // Hiển thị "Cài đặt phút"
      blinkValue(7, 1, tempMinute, true);  // Nhấp nháy giá trị phút
      break;

    case SET_SECOND:
      lcd.setCursor(3, 0);
      lcd.print("Set Second");              // Hiển thị "Cài đặt giây"
      blinkValue(10, 1, tempSecond, true);  // Nhấp nháy giá trị giây
      break;

    case SET_DAY:
      lcd.setCursor(4, 0);
      lcd.print("Set Day");  // Hiển thị "Cài đặt ngày"
      lcd.setCursor(4, 1);

      // Hiển thị ngày hiện tại
      if (tempDay < 10) lcd.print("0");
      lcd.print(tempDay);
      lcd.print("/");

      if (tempMonth < 10) lcd.print("0");
      lcd.print(tempMonth);
      lcd.print("/");

      if (tempYear < 10) lcd.print("0");
      lcd.print(tempYear);
      break;

    case SET_MONTH:
      lcd.setCursor(3, 0);
      lcd.print("Set Month");  // Hiển thị "Cài đặt tháng"
      lcd.setCursor(4, 1);

      // Hiển thị ngày hiện tại
      if (tempDay < 10) lcd.print("0");
      lcd.print(tempDay);
      lcd.print("/");

      if (tempMonth < 10) lcd.print("0");
      lcd.print(tempMonth);
      lcd.print("/");

      if (tempYear < 10) lcd.print("0");
      lcd.print(tempYear);
      break;

    case SET_YEAR:
      lcd.setCursor(4, 0);
      lcd.print("Set Year");  // Hiển thị "Cài đặt năm"
      lcd.setCursor(4, 1);

      // Hiển thị ngày hiện tại
      if (tempDay < 10) lcd.print("0");
      lcd.print(tempDay);
      lcd.print("/");

      if (tempMonth < 10) lcd.print("0");
      lcd.print(tempMonth);
      lcd.print("/");

      if (tempYear < 10) lcd.print("0");
      lcd.print(tempYear);
      break;

    case SET_DOW:
      lcd.setCursor(0, 0);
      lcd.print("Set Day of Week");  // Hiển thị "Cài đặt thứ trong tuần"
      lcd.setCursor(6, 1);
      if (daysOfWeek[tempDOW] < 10) lcd.print("0");
      lcd.print(daysOfWeek[tempDOW]);  // Hiển thị tên thứ
      lcd.print("                            ");
      // Hiển thị thứ nhấp nháy
      break;
  }

  // Luôn hiển thị đầy đủ thời gian đang cài đặt ở dưới cùng, trừ cài đặt liên quan đến ngày tháng
  if (currentMode == SET_HOUR || currentMode == SET_MINUTE || currentMode == SET_SECOND) {
    lcd.setCursor(4, 1);

    // Giờ
    if (tempHour < 10) lcd.print("0");
    lcd.print(tempHour);
    lcd.print(":");

    // Phút
    if (tempMinute < 10) lcd.print("0");
    lcd.print(tempMinute);
    lcd.print(":");

    // Giây
    if (tempSecond < 10) lcd.print("0");
    lcd.print(tempSecond);
  }
}

void displayAlarmSettingMode() {
  // Hàng trên hiển thị đang cài đặt gì
  lcd.setCursor(0, 0);

  if (currentMode == SET_ALARM_HOUR) {
    lcd.print("Set Alarm Hour");        // Hiển thị "Cài đặt giờ báo thức"
    blinkValue(4, 1, alarmHour, true);  // Nhấp nháy giá trị giờ báo thức
  } else {
    lcd.print("Set Alarm Minute");        // Hiển thị "Cài đặt phút báo thức"
    blinkValue(7, 1, alarmMinute, true);  // Nhấp nháy giá trị phút báo thức
  }

  // Hàng dưới hiển thị thời gian báo thức
  lcd.setCursor(4, 1);

  // Giờ
  if (alarmHour < 10) lcd.print("0");
  lcd.print(alarmHour);
  lcd.print(":");

  // Phút
  if (alarmMinute < 10) lcd.print("0");
  lcd.print(alarmMinute);

  // Hiển thị biểu tượng chuông
  lcd.setCursor(0, 1);
  lcd.write(byte(3));  // Hiển thị biểu tượng chuông
}

void blinkValue(byte col, byte row, byte value, bool leadingZero) {
  // Làm giá trị nhấp nháy bằng cách thay đổi giữa hiển thị và ẩn
  if ((millis() / 500) % 2 == 0) {  // Mỗi 500ms đổi trạng thái
    lcd.setCursor(col, row);

    // Hiển thị với số 0 phía trước tùy chọn
    if (leadingZero && value < 10) {
      lcd.print("0");
    }
    lcd.print(value);
  } else {
    lcd.setCursor(col, row);

    // Ẩn giá trị bằng cách in khoảng trắng
    if (leadingZero) {
      lcd.print("  ");  // Hai khoảng trắng cho giá trị 2 chữ số
    } else {
      lcd.print(" ");  // Một khoảng trắng cho giá trị 1 chữ số
    }
  }
}

void checkAlarm() {
  // Đọc thời gian hiện tại
  bool h12, PM;
  byte hour = rtc.getHour(h12, PM);
  byte minute = rtc.getMinute();
  byte second = rtc.getSecond();

  // Kiểm tra nếu báo thức nên kích hoạt (khi giây bằng 0)
  if (alarmEnabled && hour == alarmHour && minute == alarmMinute && second == 0 && !alarmTriggered) {
    alarmTriggered = true;
    soundAlarm(true);  // Bật âm thanh báo thức
  }

  // Tắt báo thức sau 1 phút nếu không được dừng thủ công
  if (alarmTriggered && second == 30) {
    alarmTriggered = false;
    soundAlarm(false);  // Tắt âm thanh báo thức
  }
}

void setAlarm() {
  // Thiết lập báo thức chỉ là lưu các biến trong triển khai này
  // Đối với hệ thống phức tạp hơn, điều này sẽ tương tác với báo thức tích hợp của DS3231
}

void soundAlarm(bool state) {
  // Điều khiển còi báo
  digitalWrite(BUZZER_PIN, state ? HIGH : LOW);  // Bật hoặc tắt còi báo theo trạng thái
}