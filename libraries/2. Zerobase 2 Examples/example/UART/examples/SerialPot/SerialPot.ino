/* Đọc Giá Trị Analog (ADC) và Gửi qua Serial
 *
 * Mô tả: Sketch này thực hiện việc đọc giá trị điện áp analog từ một chân ADC
 * được chỉ định (A3) và sau đó in giá trị này ra Serial Monitor.
 * Đây là một ví dụ cơ bản nhưng quan trọng để bắt đầu làm việc với các cảm biến
 * analog và hiểu về cách ADC hoạt động trên vi điều khiển.
 *
 * Link bài viết hướng dẫn chi tiết: [THÊM_LINK_BÀI_VIẾT_CỦA_BẠN_VỀ_ADC_ZEROBASÉ]
 */

#include <ZBPrint.h>  // Thư viện in ra Serial Monitor

// Khai báo hằng số adcPin là chân A3 để sử dụng cho việc đọc giá trị analog (ADC)
const int adcPin = A3;  // Chân analog được chọn để đọc giá trị ADC

// Hàm setup() chỉ chạy một lần khi vi điều khiển khởi động hoặc reset
void setup() {
  // Khởi tạo giao tiếp Serial với tốc độ truyền là 9600 baud.
  // Điều này cho phép vi điều khiển gửi dữ liệu đến máy tính để bạn có thể xem trên Serial Monitor.
  Serial.begin(9600);
}

// Hàm loop() chạy lặp liên tục sau khi setup() hoàn tất
void loop() {
  // Đọc giá trị điện áp analog tại chân adcPin (A3).
  // Hàm analogRead() sẽ chuyển đổi điện áp analog thành một giá trị số nguyên.
  // Với ADC 12-bit (như trên Zerobase 2), kết quả trả về là một số nguyên từ 0 đến 4095.
  int adcValue = analogRead(adcPin);

  // Gửi chuỗi "ADC Value: " qua cổng Serial.
  // Hàm Serial.print() không thêm ký tự xuống dòng, cho phép giá trị số xuất hiện trên cùng một dòng.
  Serial.print("ADC Value: ");

  // Gửi giá trị adcValue vừa đọc được qua cổng Serial.
  // Hàm Serial.println() sẽ tự động thêm ký tự xuống dòng ('\n') sau giá trị,
  // đảm bảo mỗi lần đọc sẽ hiển thị trên một dòng mới trong Serial Monitor.
  Serial.println(adcValue);

  // Dừng chương trình trong 500 mili giây (nửa giây) trước khi thực hiện vòng lặp tiếp theo.
  // Việc này giúp kiểm soát tốc độ đọc và gửi dữ liệu, tránh làm tràn Serial Monitor
  // và giúp người dùng dễ dàng theo dõi các giá trị thay đổi.
  delay(500);
}