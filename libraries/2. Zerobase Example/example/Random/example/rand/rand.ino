/*
* Ví dụ về hàm random()
*
* Mô tả: Chương trình này minh họa cách tạo số ngẫu nhiên bằng hàm random().
* Nó sẽ in ra hai loại số ngẫu nhiên: một số từ 0 đến 299 và một số từ 10 đến 19 (không bao gồm 20).
* Các số này được in ra Serial Monitor mỗi giây.
*/

// Không bao gồm thư viện ZBPrint theo yêu cầu.

// Cú pháp:
// random(max)        // Tạo số ngẫu nhiên từ 0 đến (max - 1)
// random(min, max)   // Tạo số ngẫu nhiên từ min đến (max - 1)
// Trả về: Một số ngẫu nhiên. Kiểu dữ liệu: long (số nguyên dài).

long randNumber;  // Biến để lưu trữ số ngẫu nhiên được tạo.

void setup() {
  Serial1.begin(9600);  // Khởi tạo giao tiếp Serial1 ở tốc độ 9600 baud.
}

void loop() {
  // Tạo và in một số ngẫu nhiên từ 0 đến 299 (vì max là 300, nên số lớn nhất là 299).
  randNumber = random(300);
  Serial1.print("Randum number from 0 to 299: ");
  Serial1.println(randNumber);

  // Tạo và in một số ngẫu nhiên từ 10 đến 19 (vì min là 10 và max là 20, nên số lớn nhất là 19).
  randNumber = random(10, 20);
  Serial1.print("Randum number from 10 to 20: ");
  Serial1.println(randNumber);

  Serial1.println();  // In một dòng trống để dễ đọc hơn giữa các lần in.
  delay(1000);        // Dừng chương trình 1 giây trước khi lặp lại.
}