/*
* Ví dụ về hàm abs()
*
* Mô tả: Chương trình này minh họa cách sử dụng hàm abs() để lấy giá trị tuyệt đối
* của một số nguyên. Nó sẽ in ra màn hình Serial Monitor giá trị tuyệt đối của cả
* số dương và số âm.
*/

// Không bao gồm thư viện ZBPrint theo yêu cầu.

void setup() {
  Serial1.begin(9600);  // Khởi tạo giao tiếp Serial1 ở tốc độ 9600 baud.
  while (!Serial1) {
    ;  // Đợi cho cổng Serial1 kết nối. Điều này cần thiết cho các cổng USB gốc.
  }

  // Khởi tạo một biến số nguyên x với giá trị dương.
  int x = 42;
  // In ra thông báo và giá trị tuyệt đối của x.
  Serial1.print("The absolute value of ");
  Serial1.print(x);
  Serial1.print(" is ");
  Serial1.println(abs(x));  // Sử dụng hàm abs() để lấy giá trị tuyệt đối.

  // Gán lại x với một giá trị âm.
  x = -42;
  // In ra thông báo và giá trị tuyệt đối của x.
  Serial1.print("The absolute value of ");
  Serial1.print(x);
  Serial1.print(" is ");
  Serial1.println(abs(x));  // Sử dụng hàm abs() để lấy giá trị tuyệt đối.
}

void loop() {
  // Hàm loop để trống vì tất cả các thao tác được thực hiện trong hàm setup().
}