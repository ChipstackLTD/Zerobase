/*
* Ví dụ về hàm constrain()
*
* Mô tả: Chương trình này minh họa cách sử dụng hàm constrain() để giới hạn
* một giá trị số trong một phạm vi xác định. Nếu giá trị nằm ngoài phạm vi,
* nó sẽ được đặt về giới hạn gần nhất.
*/

#include <ZBPrint.h>  // Bao gồm thư viện ZBPrint, thường được sử dụng cho các chức năng in ấn.

// Cú pháp của hàm constrain(): constrain(giá_trị, giới_hạn_dưới, giới_hạn_trên)
// Hàm này trả về:
// - giá_trị: Nếu giá_trị nằm giữa giới_hạn_dưới và giới_hạn_trên
// - giới_hạn_dưới: Nếu giá_trị nhỏ hơn giới_hạn_dưới
// - giới_hạn_trên: Nếu giá_trị lớn hơn giới_hạn_trên

int x = 200;  // Khai báo và khởi tạo biến x với giá trị 200.
int a = 40;   // Khai báo và khởi tạo biến a (giới hạn dưới) với giá trị 40.
int b = 180;  // Khai báo và khởi tạo biến b (giới hạn trên) với giá trị 180.

void setup() {
  Serial.begin(9600);  // Khởi tạo giao tiếp Serial với tốc độ 9600 baud để in dữ liệu ra Serial Monitor.
  while (!Serial) {
    ;  // Đợi cho cổng Serial kết nối. Điều này cần thiết cho các bo mạch có cổng USB gốc (native USB port) để đảm bảo Serial Monitor đã sẵn sàng.
  }

  // Sử dụng hàm constrain() để giới hạn giá trị của x trong khoảng từ a đến b.
  // Ở đây, x ban đầu là 200, mà 200 > 180 (giá trị của b),
  // nên hàm constrain sẽ trả về giá trị của b (180).
  // Vì vậy, x sẽ được gán lại bằng 180.
  x = constrain(x, a, b);

  Serial.print("Constrain value: ");  // In chuỗi "Constrain value: " ra Serial Monitor.
  Serial.println(x);                  // In giá trị đã được constrain của x ra Serial Monitor.
                                      // Kết quả in ra sẽ là "Constrain value: 180".
}

void loop() {
  // Hàm loop để trống vì tất cả các thao tác minh họa chỉ cần chạy một lần trong hàm setup().
}