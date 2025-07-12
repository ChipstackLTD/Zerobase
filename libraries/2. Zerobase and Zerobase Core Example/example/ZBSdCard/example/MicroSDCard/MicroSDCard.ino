/* Kiểm tra chức năng thẻ SD
 *
 * Mô tả: Chương trình này kiểm tra các chức năng cơ bản của module thẻ SD
 * trên Zerobase, bao gồm khởi tạo, tạo thư mục lồng nhau (deep directories),
 * và ghi/đọc file ở các cấp độ thư mục khác nhau. Đèn LED tích hợp sẽ nhấp nháy
 * để báo hiệu tiến trình và lỗi, giúp dễ dàng theo dõi trạng thái hoạt động.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=116
 */

#include <SPI.h>       // Thư viện giao tiếp SPI chuẩn của Arduino
#include "ZBSdCard.h"  // Thư viện điều khiển thẻ SD tùy chỉnh (Enhanced ZBSdCard)

#define SD_CS_PIN 10         // Định nghĩa chân CS cho thẻ SD là chân số 10
#define LED_PIN LED_BUILTIN  // Sử dụng đèn LED tích hợp trên bo mạch (thường là chân 13)

ZBSdCard sdCard(SD_CS_PIN);  // Tạo đối tượng sdCard từ lớp ZBSdCard với chân CS đã định nghĩa

// Hàm tạo thư mục nếu chưa tồn tại
bool createDirIfNeeded(const char* dirname);
// Hàm kiểm tra nội dung file bằng cách đọc
void verifyFile(const char* filename);
// Hàm nhấp nháy LED báo hiệu tiến trình (mỗi bước 1 lần nhấp nháy)
void blinkProgress(int count);
// Hàm nhấp nháy LED báo lỗi và lặp vô hạn
void blinkError(int count);


void setup() {
  pinMode(LED_PIN, OUTPUT);  // Thiết lập chân LED làm đầu ra
  Serial1.begin(115200);     // Khởi động giao tiếp Serial1 với tốc độ baud 115200
  delay(1000);               // Đợi 1 giây để ổn định

  Serial1.println("===== Deep Directory Test =====");  // In tiêu đề bài kiểm tra ra serial

  // Bước 1: Khởi động thẻ SD
  Serial1.println("1. Initializing SD card");
  if (!sdCard.begin()) {                           // Nếu khởi động thất bại
    Serial1.println("SD initialization failed!");  // In thông báo lỗi
    blinkError(10);                                // Nhấp nháy đèn LED báo lỗi 10 lần rồi dừng
    return;
  }
  Serial1.println("SD initialized successfully");  // Thông báo thành công
  blinkProgress(1);                                // Nhấp nháy đèn LED 1 lần báo tiến trình
  delay(500);

  // Bước 2: Tạo thư mục cấp 1 có tên "TEST"
  Serial1.println("2. Creating TEST directory");
  if (!createDirIfNeeded("TEST")) {  // Nếu không tạo được thì dừng
    return;
  }
  blinkProgress(2);  // Nhấp nháy LED 2 lần
  delay(500);

  // Bước 3: Tạo file ở thư mục gốc
  Serial1.println("3. Creating file in root");
  if (!sdCard.writeFile("ROOT.TXT", "Root level file")) {  // Tạo file ROOT.TXT với nội dung mẫu
    Serial1.println("Failed to create root file");         // Thông báo lỗi nếu không tạo được
    blinkError(3);                                         // Nhấp nháy báo lỗi
    return;
  }
  Serial1.println("Root file created successfully");  // Thông báo thành công
  blinkProgress(3);
  delay(500);

  // Bước 4: Tạo thư mục cấp 2: TEST/SUB
  Serial1.println("4. Creating TEST/SUB directory");
  if (!createDirIfNeeded("TEST/SUB")) {
    return;
  }
  blinkProgress(4);
  delay(500);

  // Bước 5: Tạo file trong thư mục TEST/SUB
  Serial1.println("5. Creating file in TEST/SUB");
  if (!sdCard.writeFile("TEST/SUB/L2.TXT", "Level 2 file")) {
    Serial1.println("Failed to create L2 file");
    blinkError(5);
    return;
  }
  Serial1.println("Level 2 file created successfully");
  blinkProgress(5);
  delay(500);

  // Bước 6: Tạo thư mục cấp 3: TEST/SUB/NEW
  Serial1.println("6. Creating TEST/SUB/NEW directory");
  if (!createDirIfNeeded("TEST/SUB/NEW")) {
    return;
  }
  blinkProgress(6);
  delay(500);

  // Bước 7: Tạo file trong thư mục TEST/SUB/NEW
  Serial1.println("7. Creating file in TEST/SUB/NEW");
  if (!sdCard.writeFile("TEST/SUB/NEW/L3.TXT", "Level 3 file")) {
    Serial1.println("Failed to create L3 file");
    blinkError(7);
    return;
  }
  Serial1.println("Level 3 file created successfully");
  blinkProgress(7);
  delay(500);

  // Bước 8: Kiểm tra đọc lại các file đã tạo
  Serial1.println("8. Verifying files by reading them");
  verifyFile("ROOT.TXT");             // Đọc file ROOT.TXT
  verifyFile("TEST/SUB/L2.TXT");      // Đọc file cấp 2
  verifyFile("TEST/SUB/NEW/L3.TXT");  // Đọc file cấp 3
  blinkProgress(8);

  // Báo hiệu hoàn thành bằng cách nhấp nháy LED nhanh 10 lần
  Serial1.println("===== All Tests Passed =====");
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void loop() {
  // Nhấp nháy LED định kỳ như nhịp tim để thể hiện chương trình đang chạy
  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);
  delay(1950);
}
// Hàm tạo thư mục nếu chưa tồn tại
bool createDirIfNeeded(const char* dirname) {
  if (sdCard.dirExists(dirname)) {  // Nếu thư mục đã tồn tại
    Serial1.print(dirname);
    Serial1.println(" already exists");
    return true;
  }

  Serial1.print("Creating ");
  Serial1.println(dirname);

  if (!sdCard.createDirectory(dirname)) {  // Tạo thư mục, nếu thất bại thì báo lỗi
    Serial1.print("Failed to create ");
    Serial1.println(dirname);
    blinkError(3);
    return false;
  }

  Serial1.print(dirname);
  Serial1.println(" created successfully");
  return true;
}

// Hàm kiểm tra nội dung file bằng cách đọc
void verifyFile(const char* filename) {
  char buffer[64];  // Bộ nhớ đệm chứa dữ liệu đọc
  uint16_t bytesRead;

  Serial1.print("Reading ");
  Serial1.println(filename);

  // Đặt tất cả các byte trong buffer về 0 để đảm bảo chuỗi kết thúc null
  memset(buffer, 0, sizeof(buffer));

  if (sdCard.readFile(filename, buffer, sizeof(buffer) - 1, &bytesRead)) {  // -1 để dành chỗ cho null terminator
    Serial1.print("Content: ");
    Serial1.println(buffer);
    Serial1.print("Bytes read: ");
    Serial1.println(bytesRead);
  } else {
    Serial1.print("Failed to read ");
    Serial1.println(filename);
  }
}

// Hàm nhấp nháy LED báo hiệu tiến trình (mỗi bước 1 lần nhấp nháy)
void blinkProgress(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

// Hàm nhấp nháy LED báo lỗi và lặp vô hạn
void blinkError(int count) {
  while (1) {
    for (int i = 0; i < count; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(200);
    }
    delay(1000);  // Nghỉ giữa mỗi chu kỳ nhấp nháy
  }
}