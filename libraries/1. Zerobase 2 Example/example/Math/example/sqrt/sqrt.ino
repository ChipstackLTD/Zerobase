/*
* Ví dụ về hàm sqrt()
*
* Mô tả: Chương trình này minh họa cách sử dụng hàm sqrt() để tính căn bậc hai của một số.
* Nó sẽ đọc một số từ Serial Monitor, tính căn bậc hai của số đó và in cả số gốc
* và kết quả căn bậc hai ra màn hình.
*/

#include <ZBPrint.h>  // Bao gồm thư viện ZBPrint.

// Cú pháp của hàm sqrt(): sqrt(x)
// Trả về: Căn bậc hai của số. Kiểu dữ liệu: double (số thực độ chính xác kép).

int x = 0;    // Biến để lưu trữ giá trị đọc từ Serial Monitor.
int res = 0;  // Biến để lưu trữ kết quả căn bậc hai. Lưu ý: sqrt() trả về double, nên có thể có mất mát chính xác khi gán vào int.

void setup() {
  Serial.begin(9600);  // Khởi tạo giao tiếp Serial ở tốc độ 9600 baud.
  while (!Serial) {
    ;  // Đợi cho cổng Serial kết nối. Điều này cần thiết cho các bo mạch có cổng USB gốc.
  }
  // Lưu ý: Do cách triển khai của hàm sq() (và tương tự cho sqrt()),
  // tránh sử dụng các hàm khác bên trong dấu ngoặc của nó, điều này có thể dẫn đến kết quả không chính xác.
  // Ví dụ: // int x = sq(Serial.parseInt()); // nên tránh cách này
}

void loop() {
  // Đọc một số nguyên từ Serial Monitor. Hàm parseInt() sẽ đợi cho đến khi nhận được một số.
  x = Serial.parseInt();  // Giữ các phép toán khác bên ngoài hàm sqrt().

  // Chỉ thực hiện tính toán nếu giá trị đọc được khác 0.
  if (x != 0) {
    // Tính căn bậc hai của x. Kết quả là double và sẽ được chuyển đổi thành int khi gán vào 'res'.
    int res = sqrt(x);

    Serial.print("x value: ");  // In ra chuỗi "x value: "
    Serial.println(x);          // In giá trị gốc của x.

    Serial.print("Square root value: ");  // In ra chuỗi "Square root value: "
    Serial.println(res);                  // In giá trị căn bậc hai (phần nguyên).
  }
}