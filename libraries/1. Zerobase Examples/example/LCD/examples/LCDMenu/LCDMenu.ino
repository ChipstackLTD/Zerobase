/* Giao diện Menu LCD với nút nhấn
 *
 * Mô tả: Chương trình này tạo một hệ thống menu đơn giản trên màn hình
 * LCD 16x2, cho phép người dùng điều hướng qua các mục bằng các nút
 * "Lên" và "Xuống", và chọn một mục bằng nút "Chọn".
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=108
 */

#include <LiquidCrystal_I2C.h>  // Thư viện điều khiển LCD dùng I2C

// Khởi tạo đối tượng LCD 16x2 với địa chỉ I2C là 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Khai báo các chân nút bấm
const int btnDown = 2;     // Nút "Xuống" nối với chân D2
const int btnUp = 3;       // Nút "Lên" nối với chân D3
const int btnSelect = 10;  // Nút "Chọn" nối với chân D10

// Danh sách các mục trong menu
const char* menuItems[] = {
  "1. Start",    // Mục 1
  "2. Setting",  // Mục 2
  "3. About",    // Mục 3
  "4. Exit"      // Mục 4
};

// Tính số lượng mục menu (tổng số phần tử trong mảng)
const int menuLength = sizeof(menuItems) / sizeof(menuItems[0]);

int currentIndex = 0;   // Vị trí hiện tại trong menu (bắt đầu từ 0)
bool selected = false;  // Biến trạng thái khi chọn mục (chưa dùng)

void setup() {
  lcd.init();       // Khởi động LCD
  lcd.backlight();  // Bật đèn nền LCD

  // Cấu hình các chân nút bấm ở chế độ INPUT_PULLUP
  pinMode(btnUp, INPUT_PULLUP);      // Nút "Lên" kéo lên nội bộ
  pinMode(btnDown, INPUT_PULLUP);    // Nút "Xuống" kéo lên nội bộ
  pinMode(btnSelect, INPUT_PULLUP);  // Nút "Chọn" kéo lên nội bộ

  // Hiển thị thông điệp khởi động lên LCD
  lcd.setCursor(2, 0);       // Di chuyển con trỏ đến cột 2, dòng 0
  lcd.print("Zerobase!!!");  // In ra "Zerobase!!!"
  lcd.setCursor(2, 1);       // Di chuyển con trỏ đến cột 2, dòng 1
  lcd.print("Starting...");  // In ra "Starting..."
  delay(2000);               // Tạm dừng 2 giây

  showMenu();  // Hiển thị menu lần đầu
}

void loop() {
  // Xử lý khi nhấn nút "Lên"
  if (!digitalRead(btnUp)) {                                      // Nếu nút "Lên" được nhấn (LOW)
    currentIndex = (currentIndex - 1 + menuLength) % menuLength;  // Lùi một mục, quay vòng nếu < 0
    showMenu();                                                   // Cập nhật hiển thị menu
    delay(200);                                                   // Chống dội nút
  }

  // Xử lý khi nhấn nút "Xuống"
  if (!digitalRead(btnDown)) {                       // Nếu nút "Xuống" được nhấn
    currentIndex = (currentIndex + 1) % menuLength;  // Tăng một mục, quay vòng nếu vượt quá
    showMenu();                                      // Cập nhật hiển thị menu
    delay(200);                                      // Chống dội nút
  }

  // Xử lý khi nhấn nút "Chọn"
  if (!digitalRead(btnSelect)) {         // Nếu nút "Chọn" được nhấn
    lcd.clear();                         // Xóa màn hình
    lcd.setCursor(0, 0);                 // Di chuyển con trỏ đến đầu dòng
    lcd.print("Selected:");              // In ra "Selected:"
    lcd.setCursor(0, 1);                 // Di chuyển xuống dòng 2
    lcd.print(menuItems[currentIndex]);  // In mục hiện tại được chọn
    delay(1000);                         // Đợi 1 giây
    showMenu();                          // Quay lại menu
  }
}

// Hàm hiển thị menu
void showMenu() {
  lcd.clear();                         // Xóa màn hình LCD
  lcd.setCursor(0, 0);                 // Con trỏ đầu dòng đầu tiên
  lcd.print("> ");                     // In dấu ">" chỉ mục được chọn
  lcd.print(menuItems[currentIndex]);  // In mục hiện tại

  // Hiển thị mục tiếp theo (nếu có) ở dòng thứ hai
  int nextIndex = (currentIndex + 1) % menuLength;  // Tính mục tiếp theo theo kiểu vòng tròn
  lcd.setCursor(0, 1);                              // Con trỏ đầu dòng thứ hai
  lcd.print("   ");                                 // In khoảng trắng để căn chỉnh
  lcd.print(menuItems[nextIndex]);                  // In mục tiếp theo
}