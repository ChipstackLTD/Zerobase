/*
* Ví dụ về hàm randomSeed()
*
* Mô tả: Chương trình này minh họa cách sử dụng hàm randomSeed() để khởi tạo bộ tạo số ngẫu nhiên.
* Bằng cách sử dụng giá trị đọc từ chân analog chưa kết nối (tạo ra nhiễu ngẫu nhiên) làm "hạt giống" (seed),
* mỗi lần chạy chương trình sẽ tạo ra một chuỗi số ngẫu nhiên khác nhau.
* Chương trình sau đó sẽ in ra các số ngẫu nhiên trong hai phạm vi khác nhau lên Serial Monitor.
*/

// Không bao gồm thư viện ZBPrint theo yêu cầu.

// Cú pháp: randomSeed(hạt_giống)
// Trả về: Không có gì.

int seed = 1234;  // Biến 'seed' được khai báo nhưng không dùng trực tiếp trong ví dụ này.
long randNumber;  // Biến để lưu trữ số ngẫu nhiên được tạo.

void setup() {
  Serial1.begin(9600);  // Khởi tạo giao tiếp Serial1 ở tốc độ 9600 baud.

  // Nếu chân đầu vào analog 0 không được kết nối, nhiễu analog ngẫu nhiên
  // sẽ khiến lệnh gọi randomSeed() tạo ra các số hạt giống khác nhau mỗi khi chương trình chạy.
  // Điều này sẽ làm cho hàm random() tạo ra các chuỗi số ngẫu nhiên khác nhau.
  randomSeed(analogRead(0));  // Sử dụng giá trị analog đọc được từ chân A0 làm hạt giống.
}

void loop() {
  // Tạo và in một số ngẫu nhiên từ 0 đến 299 (số lớn nhất là 299).
  randNumber = random(300);
  Serial1.print("Randum number from 0 to 299: ");
  Serial1.println(randNumber);

  // Tạo và in một số ngẫu nhiên từ 10 đến 19 (số lớn nhất là 19).
  randNumber = random(10, 20);
  Serial1.print("Randum number from 10 to 20: ");
  Serial1.println(randNumber);

  Serial1.println();  // In một dòng trống để dễ đọc hơn.
  delay(1000);        // Dừng chương trình 1 giây trước khi lặp lại.
}