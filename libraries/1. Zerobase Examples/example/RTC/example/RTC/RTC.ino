/* Đồng hồ thời gian thực (RTC) DS3231 với hiển thị LCD I2C
 *
 * Mô tả: Chương trình này sử dụng module đồng hồ thời gian thực (RTC) DS3231
 * để lấy thông tin về ngày, tháng, năm, thứ và thời gian (giờ, phút, giây).
 * Các thông tin này sau đó được hiển thị liên tục trên màn hình LCD 16x2
 * thông qua giao tiếp I2C. Thời gian ban đầu được cài đặt cố định trong code
 * và chỉ được thiết lập một lần khi Zerobase khởi động.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=111
 */

// Thư viện I2C chuẩn của Arduino để giao tiếp với các thiết bị sử dụng giao thức I2C
#include <Wire.h>

// Thư viện điều khiển module thời gian thực DS3231
#include <DS3231.h>

// Thư viện điều khiển LCD sử dụng giao tiếp I2C
#include <LiquidCrystal_I2C.h>

// Khởi tạo đối tượng DS3231 để giao tiếp với module thời gian thực
DS3231 myRTC;

// Khởi tạo đối tượng LCD có địa chỉ I2C là 0x27, màn hình LCD có 16 cột và 2 hàng
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Các giá trị thời gian cần cài đặt khi bắt đầu chương trình
const byte setYear = 25;    // Năm 2025 (chỉ 2 chữ số cuối)
const byte setMonth = 5;    // Tháng 5
const byte setDate = 28;    // Ngày 28
const byte setDOW = 4;      // Thứ 4 (1 = Chủ Nhật, 2 = Thứ 2, ..., 7 = Thứ 7)
const byte setHour = 10;    // 10 giờ
const byte setMinute = 48;  // 48 phút
const byte setSecond = 31;  // 31 giây

// Biến để đảm bảo rằng thời gian chỉ được thiết lập một lần
bool timeIsSet = false;

void setup() {
  // Bắt đầu giao tiếp I2C
  Wire.begin();

  // Khởi động LCD với cấu hình 16x2
  lcd.begin(16, 2);

  // Bật đèn nền LCD
  lcd.backlight();

  // Thiết lập chế độ 24h cho DS3231 (false = 24h, true = 12h)
  myRTC.setClockMode(false);

  // Kiểm tra nếu thời gian chưa được cài thì tiến hành cài đặt
  if (!timeIsSet) {
    // Thiết lập từng thành phần của thời gian thực
    myRTC.setYear(setYear);      // Năm
    myRTC.setMonth(setMonth);    // Tháng
    myRTC.setDate(setDate);      // Ngày
    myRTC.setDoW(setDOW);        // Thứ trong tuần
    myRTC.setHour(setHour);      // Giờ
    myRTC.setMinute(setMinute);  // Phút
    myRTC.setSecond(setSecond);  // Giây

    // Đánh dấu thời gian đã được thiết lập
    timeIsSet = true;

    // Hiển thị thông điệp khởi động
    lcd.setCursor(2, 0);       // Cột 2, dòng 0
    lcd.print("Zerobase!!!");  // Dòng chữ đầu
    lcd.setCursor(2, 1);       // Cột 2, dòng 1
    lcd.print("Starting...");  // Dòng chữ thứ hai
    delay(2000);               // Dừng lại 2 giây để người dùng đọc

    lcd.clear();  // Xóa màn hình sau khi hiển thị
  }
}

void loop() {
  // Biến trạng thái cho chế độ 12h/24h và AM/PM, bắt buộc khai báo khi gọi getHour
  bool h12, PM;

  // Biến lưu trạng thái thế kỷ (dùng nếu năm > 2099, không cần thiết ở đây)
  bool century;

  // Đọc thời gian thực từ module DS3231
  byte second = myRTC.getSecond();       // Giây hiện tại
  byte minute = myRTC.getMinute();       // Phút hiện tại
  byte hour = myRTC.getHour(h12, PM);    // Giờ hiện tại
  byte date = myRTC.getDate();           // Ngày hiện tại
  byte month = myRTC.getMonth(century);  // Tháng hiện tại
  byte year = myRTC.getYear();           // Năm hiện tại (2 chữ số cuối)
  byte dow = myRTC.getDoW();             // Thứ trong tuần

  // Mảng chứa tên viết tắt các ngày trong tuần
  const char* daysOfWeek[] = {
    "Err",  // Vị trí 0 không sử dụng (do DOW bắt đầu từ 1)
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };

  // Hiển thị ngày trong tuần lên LCD
  lcd.setCursor(1, 0);         // Đặt con trỏ tại hàng 0, cột 1
  lcd.print(daysOfWeek[dow]);  // In thứ (ví dụ: Mon)
  lcd.print(" ");

  // In ngày với định dạng 2 chữ số
  if (date < 10) lcd.print("0");
  lcd.print(date);
  lcd.print("/");

  // In tháng với định dạng 2 chữ số
  if (month < 10) lcd.print("0");
  lcd.print(month);
  lcd.print("/20");

  // In năm với định dạng 2 chữ số
  if (year < 10) lcd.print("0");
  lcd.print(year);

  // Hiển thị giờ ở dòng thứ 2
  lcd.setCursor(4, 1);  // Đặt con trỏ tại hàng 1, cột 4

  // In giờ với định dạng 2 chữ số
  if (hour < 10) lcd.print("0");
  lcd.print(hour);
  lcd.print(":");

  // In phút
  if (minute < 10) lcd.print("0");
  lcd.print(minute);
  lcd.print(":");

  // In giây
  if (second < 10) lcd.print("0");
  lcd.print(second);

  // Cập nhật màn hình mỗi giây
  delay(1000);
}