/* Đọc giá trị ADC và hiển thị lên UART TTL
 *
 * Mô tả: Chương trình này đọc giá trị điện áp analog từ chân A3 của Zerobase
 * (chân đọc ADC). Giá trị đọc được sẽ là một số nguyên trong khoảng từ 0 đến 1023
 * (đối với ADC 10-bit). Giá trị này sau đó được gửi qua cổng Serial1 (UART)
 * về máy tính hoặc thiết bị khác, hiển thị trên Serial Monitor, giúp bạn
 * theo dõi tín hiệu analog trong thời gian thực.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=90
 */

// Khai báo hằng số adcPin là chân A3 để sử dụng cho việc đọc giá trị analog (ADC)
const int adcPin = A3;  // Chân analog để đọc ADC

// Hàm setup() chỉ chạy một lần khi vi điều khiển khởi động hoặc reset
void setup() {
  // Khởi tạo giao tiếp Serial1 với tốc độ truyền là 9600 baud
  Serial1.begin(9600);
}

// Hàm loop() chạy lặp liên tục sau khi setup() hoàn tất
void loop() {
  // Đọc giá trị điện áp analog tại chân adcPin (A3)
  // Kết quả trả về là một số nguyên từ 0 đến 1023 (đối với độ phân giải 10 bit)
  int adcValue = analogRead(adcPin);

  // Gửi chuỗi "ADC Value: " qua cổng Serial1 để gợi ý tên giá trị đang được gửi
  Serial1.print("ADC Value: ");

  // Gửi giá trị adcValue vừa đọc được, kèm ký tự xuống dòng (println tự động thêm '\n')
  Serial1.println(adcValue);

  // Dừng chương trình trong 500 mili giây trước khi thực hiện vòng lặp tiếp theo
  // Giúp tránh việc gửi dữ liệu quá nhanh gây quá tải hoặc khó đọc trên terminal
  delay(500);
}