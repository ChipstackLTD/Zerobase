/*
* Ví dụ về hàm max()
*
* Mô tả: Chương trình này minh họa cách sử dụng hàm max() để tìm giá trị lớn hơn
* giữa hai số. Kết quả sẽ được in ra Serial Monitor.
*/

// Không bao gồm thư viện ZBPrint theo yêu cầu.

// Cú pháp của hàm max(): max(số_thứ_nhất, số_thứ_hai)
// Trả về: Giá trị lớn hơn trong hai tham số.

int x = 200;  // Khai báo và khởi tạo biến x với giá trị 200.
int y = 300;  // Khai báo và khởi tạo biến y với giá trị 300.
int res = 0;  // Khai báo biến res để lưu trữ kết quả.

void setup() {
  Serial1.begin(9600);  // Khởi tạo giao tiếp Serial1 ở tốc độ 9600 baud.
  while (!Serial1) {
    ;  // Đợi cổng Serial1 kết nối. Điều này cần thiết cho các bo mạch có cổng USB gốc.
  }

  // Sử dụng hàm max() để tìm giá trị lớn hơn giữa x và y.
  // Trong trường hợp này, max(200, 300) sẽ trả về 300.
  res = max(x, y);

  Serial1.print("Max value: ");  // In chuỗi "Max value: " ra Serial Monitor.
  Serial1.println(res);          // In giá trị của res ra Serial Monitor (kết quả sẽ là 300).
}

void loop() {
  // Hàm loop để trống vì ví dụ này chỉ cần chạy một lần trong hàm setup().
}