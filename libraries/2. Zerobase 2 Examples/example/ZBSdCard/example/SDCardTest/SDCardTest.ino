/**
 * Kiểm tra đầy đủ API ZBSdCard
 * 
 * Sketch này kiểm tra tất cả các chức năng có sẵn trong thư viện ZBSdCard.
 * Nó cung cấp tổng quan đầy đủ về khả năng của thư viện,
 * kiểm tra có hệ thống từng tính năng và báo cáo kết quả.
 * 
 * Các chức năng được kiểm tra:
 * - begin() - Khởi tạo thẻ SD
 * - createFile() - Tạo tập tin trong thư mục gốc
 * - readFile() - Đọc nội dung tập tin
 * - appendFile() - Thêm dữ liệu vào tập tin hiện có
 * - deleteFile() - Xóa tập tin
 * - createDirectory() - Tạo thư mục
 * - listDirectory() - Liệt kê nội dung thư mục
 * - createDirectoryPath() - Tạo cấu trúc thư mục lồng nhau
 * - createFileInPath() - Tạo tập tin trong thư mục con
 *
 * Link bài viết hướng dẫn chi tiết: https://elearning.chipstack.vn/mod/page/view.php?id=165
 */

#include <SPI.h>       // Bao gồm thư viện SPI cho giao tiếp với thẻ SD
#include <ZBSdCard.h>  // Bao gồm thư viện ZBSdCard
#include <ZBPrint.h>   // Bao gồm thư viện ZBPrint để in thông tin

// Tạo một đối tượng của lớp ZBSdCard
ZBSdCard sdCard;  // Khởi tạo đối tượng sdCard để làm việc với thẻ SD

// Định nghĩa chân chip select cho thiết lập của bạn
const int SD_CS_PIN = 10;  // Thay đổi nếu cần thiết - Chân kết nối CS với thẻ SD

// Buffer cho các thao tác tập tin
char readBuffer[256];  // Khai báo buffer để đọc dữ liệu từ tập tin

// Bộ đếm kiểm tra và theo dõi trạng thái
int testsRun = 0;     // Đếm số lượng kiểm tra đã chạy
int testsPassed = 0;  // Đếm số lượng kiểm tra đã vượt qua

// Hàm để in tiêu đề kiểm tra
void beginTest(const char* testName) {                         // Hàm bắt đầu một bài kiểm tra mới với tên được cung cấp
  testsRun++;                                                  // Tăng bộ đếm bài kiểm tra
  Serial.println();                                            // In xuống dòng
  Serial.print("TEST ");                                       // In chữ "TEST"
  Serial.print(testsRun);                                      // In số thứ tự bài kiểm tra
  Serial.print(": ");                                          // In dấu hai chấm và khoảng trắng
  Serial.println(testName);                                    // In tên bài kiểm tra
  Serial.println("----------------------------------------");  // In đường phân cách
}

// Hàm để báo cáo kết quả kiểm tra
void endTest(bool success) {           // Hàm kết thúc bài kiểm tra với kết quả thành công hay thất bại
  if (success) {                       // Nếu bài kiểm tra thành công
    testsPassed++;                     // Tăng bộ đếm bài kiểm tra vượt qua
    Serial.println("RESULT: PASS ✓");  // In thông báo kiểm tra vượt qua
  } else {                             // Nếu bài kiểm tra thất bại
    Serial.println("RESULT: FAIL ✗");  // In thông báo kiểm tra thất bại
  }
  Serial.println("----------------------------------------");  // In đường phân cách
}

// Kiểm tra 1: Các thao tác tập tin cơ bản trong thư mục gốc
void testBasicFileOperations() {       // Hàm kiểm tra các thao tác tập tin cơ bản
  beginTest("Basic File Operations");  // Bắt đầu bài kiểm tra thao tác tập tin cơ bản
  bool success = true;                 // Khởi tạo biến theo dõi trạng thái thành công

  // Tạo một tập tin kiểm tra
  Serial.println("1.1: Creating file 'test1.txt'");                        // In thông báo đang tạo tập tin
  if (!sdCard.createFile("test1.txt", "This is test file #1 content.")) {  // Tạo tập tin với nội dung
    Serial.println("Failed to create file!");                              // In thông báo nếu tạo tập tin thất bại
    success = false;                                                       // Đánh dấu kiểm tra là thất bại
  }

  // Đọc tập tin
  Serial.println("1.2: Reading file 'test1.txt'");                                    // In thông báo đang đọc tập tin
  memset(readBuffer, 0, sizeof(readBuffer));                                          // Xóa bộ đệm đọc
  uint32_t bytesRead = sdCard.readFile("test1.txt", readBuffer, sizeof(readBuffer));  // Đọc tập tin vào buffer

  if (bytesRead > 0) {           // Nếu đọc được dữ liệu
    Serial.print("Read ");       // In "Read"
    Serial.print(bytesRead);     // In số byte đã đọc
    Serial.println(" bytes:");   // In "bytes:"
    Serial.println(readBuffer);  // In nội dung đã đọc được

    // Xác minh nội dung
    if (strcmp(readBuffer, "This is test file #1 content.") != 0) {  // So sánh nội dung đọc được với nội dung mong đợi
      Serial.println("File content doesn't match!");                 // In thông báo nếu nội dung không khớp
      success = false;                                               // Đánh dấu kiểm tra là thất bại
    }
  } else {                                   // Nếu không đọc được dữ liệu
    Serial.println("Failed to read file!");  // In thông báo thất bại
    success = false;                         // Đánh dấu kiểm tra là thất bại
  }

  // Thêm vào tập tin
  Serial.println("1.3: Appending to file 'test1.txt'");                  // In thông báo đang thêm nội dung vào tập tin
  if (!sdCard.appendFile("test1.txt", "\nThis is appended content.")) {  // Thêm nội dung vào tập tin
    Serial.println("Failed to append to file!");                         // In thông báo nếu thêm thất bại
    success = false;                                                     // Đánh dấu kiểm tra là thất bại
  }

  // Đọc lại tập tin để xác minh việc thêm
  Serial.println("1.4: Reading file after append");                          // In thông báo đang đọc tập tin sau khi thêm
  memset(readBuffer, 0, sizeof(readBuffer));                                 // Xóa bộ đệm đọc
  bytesRead = sdCard.readFile("test1.txt", readBuffer, sizeof(readBuffer));  // Đọc lại tập tin

  if (bytesRead > 0) {           // Nếu đọc được dữ liệu
    Serial.print("Read ");       // In "Read"
    Serial.print(bytesRead);     // In số byte đã đọc
    Serial.println(" bytes:");   // In "bytes:"
    Serial.println(readBuffer);  // In nội dung đã đọc được

    // Xác minh nội dung bao gồm dữ liệu đã thêm
    if (strstr(readBuffer, "This is appended content.") == NULL) {  // Tìm chuỗi đã thêm trong nội dung
      Serial.println("Appended content not found!");                // In thông báo nếu không tìm thấy
      success = false;                                              // Đánh dấu kiểm tra là thất bại
    }
  } else {                                                // Nếu không đọc được dữ liệu
    Serial.println("Failed to read file after append!");  // In thông báo thất bại
    success = false;                                      // Đánh dấu kiểm tra là thất bại
  }

  // Liệt kê thư mục gốc để xác minh việc tạo tập tin
  Serial.println("1.5: Listing root directory");  // In thông báo đang liệt kê thư mục gốc
  if (!sdCard.listDirectory("/")) {               // Liệt kê nội dung thư mục gốc
    Serial.println("Failed to list directory!");  // In thông báo nếu liệt kê thất bại
    success = false;                              // Đánh dấu kiểm tra là thất bại
  }

  endTest(success);  // Kết thúc bài kiểm tra với kết quả
}

// Kiểm tra 2: Tạo và liệt kê thư mục
void testDirectoryOperations() {      // Hàm kiểm tra các thao tác thư mục
  beginTest("Directory Operations");  // Bắt đầu bài kiểm tra thao tác thư mục
  bool success = true;                // Khởi tạo biến theo dõi trạng thái thành công

  // Tạo một thư mục
  Serial.println("2.1: Creating directory 'testdir'");  // In thông báo đang tạo thư mục
  if (!sdCard.createDirectory("testdir")) {             // Tạo thư mục "testdir"
    Serial.println("Failed to create directory!");      // In thông báo nếu tạo thất bại
    success = false;                                    // Đánh dấu kiểm tra là thất bại
  }

  // Liệt kê thư mục gốc để xác minh việc tạo thư mục
  Serial.println("2.2: Listing root directory to verify directory creation");  // In thông báo xác minh tạo thư mục
  if (!sdCard.listDirectory("/")) {                                            // Liệt kê nội dung thư mục gốc
    Serial.println("Failed to list directory!");                               // In thông báo nếu liệt kê thất bại
    success = false;                                                           // Đánh dấu kiểm tra là thất bại
  }

  // Tạo một tập tin trong thư mục
  Serial.println("2.3: Creating file in directory");                                         // In thông báo đang tạo tập tin trong thư mục
  if (!sdCard.createFileInPath("/testdir/test2.txt", "This is test file #2 in testdir.")) {  // Tạo tập tin trong thư mục con
    Serial.println("Failed to create file in directory!");                                   // In thông báo nếu tạo thất bại
    success = false;                                                                         // Đánh dấu kiểm tra là thất bại
  }

  // Liệt kê thư mục để xác minh việc tạo tập tin
  Serial.println("2.4: Listing directory content");       // In thông báo đang liệt kê nội dung thư mục
  if (!sdCard.listDirectory("/testdir")) {                // Liệt kê nội dung thư mục "testdir"
    Serial.println("Failed to list directory content!");  // In thông báo nếu liệt kê thất bại
    success = false;                                      // Đánh dấu kiểm tra là thất bại
  }

  // Đọc tập tin trong thư mục
  Serial.println("2.5: Reading file from directory");                                          // In thông báo đang đọc tập tin từ thư mục
  memset(readBuffer, 0, sizeof(readBuffer));                                                   // Xóa bộ đệm đọc
  uint32_t bytesRead = sdCard.readFile("/testdir/test2.txt", readBuffer, sizeof(readBuffer));  // Đọc tập tin từ thư mục con

  if (bytesRead > 0) {           // Nếu đọc được dữ liệu
    Serial.print("Read ");       // In "Read"
    Serial.print(bytesRead);     // In số byte đã đọc
    Serial.println(" bytes:");   // In "bytes:"
    Serial.println(readBuffer);  // In nội dung đã đọc được

    // Xác minh nội dung
    if (strcmp(readBuffer, "This is test file #2 in testdir.") != 0) {  // So sánh nội dung đọc được với nội dung mong đợi
      Serial.println("File content doesn't match!");                    // In thông báo nếu nội dung không khớp
      success = false;                                                  // Đánh dấu kiểm tra là thất bại
    }
  } else {                                                  // Nếu không đọc được dữ liệu
    Serial.println("Failed to read file from directory!");  // In thông báo thất bại
    success = false;                                        // Đánh dấu kiểm tra là thất bại
  }

  endTest(success);  // Kết thúc bài kiểm tra với kết quả
}

// Kiểm tra 3: Thư mục lồng nhau và thao tác tập tin
void testNestedDirectories() {      // Hàm kiểm tra thư mục lồng nhau
  beginTest("Nested Directories");  // Bắt đầu bài kiểm tra thư mục lồng nhau
  bool success = true;              // Khởi tạo biến theo dõi trạng thái thành công

  // Tạo một cấu trúc thư mục lồng nhau
  Serial.println("3.1: Creating nested directories '/data/logs/app'");  // In thông báo đang tạo thư mục lồng nhau
  if (!sdCard.createDirectoryPath("/data/logs/app")) {                  // Tạo cấu trúc thư mục lồng nhau
    Serial.println("Failed to create nested directories!");             // In thông báo nếu tạo thất bại
    success = false;                                                    // Đánh dấu kiểm tra là thất bại
  }

  // Liệt kê thư mục gốc
  Serial.println("3.2: Listing root directory");       // In thông báo đang liệt kê thư mục gốc
  if (!sdCard.listDirectory("/")) {                    // Liệt kê nội dung thư mục gốc
    Serial.println("Failed to list root directory!");  // In thông báo nếu liệt kê thất bại
    success = false;                                   // Đánh dấu kiểm tra là thất bại
  }

  // Liệt kê thư mục cấp đầu tiên
  Serial.println("3.3: Listing /data directory");       // In thông báo đang liệt kê thư mục /data
  if (!sdCard.listDirectory("/data")) {                 // Liệt kê nội dung thư mục "/data"
    Serial.println("Failed to list /data directory!");  // In thông báo nếu liệt kê thất bại
    success = false;                                    // Đánh dấu kiểm tra là thất bại
  }

  // Liệt kê thư mục cấp thứ hai
  Serial.println("3.4: Listing /data/logs directory");       // In thông báo đang liệt kê thư mục /data/logs
  if (!sdCard.listDirectory("/data/logs")) {                 // Liệt kê nội dung thư mục "/data/logs"
    Serial.println("Failed to list /data/logs directory!");  // In thông báo nếu liệt kê thất bại
    success = false;                                         // Đánh dấu kiểm tra là thất bại
  }

  // Tạo một tập tin trong thư mục lồng nhau
  Serial.println("3.5: Creating file in nested directory");                                        // In thông báo đang tạo tập tin trong thư mục lồng nhau
  if (!sdCard.createFileInPath("/data/logs/app/log.txt", "Log entry from deeply nested file.")) {  // Tạo tập tin trong thư mục lồng sâu
    Serial.println("Failed to create file in nested directory!");                                  // In thông báo nếu tạo thất bại
    success = false;                                                                               // Đánh dấu kiểm tra là thất bại
  }

  // Liệt kê nội dung thư mục lồng nhau
  Serial.println("3.6: Listing nested directory content");  // In thông báo đang liệt kê nội dung thư mục lồng nhau
  if (!sdCard.listDirectory("/data/logs/app")) {            // Liệt kê nội dung thư mục lồng nhau
    Serial.println("Failed to list nested directory!");     // In thông báo nếu liệt kê thất bại
    success = false;                                        // Đánh dấu kiểm tra là thất bại
  }

  // Đọc tập tin từ thư mục lồng nhau
  Serial.println("3.7: Reading file from nested directory");                                       // In thông báo đang đọc tập tin từ thư mục lồng nhau
  memset(readBuffer, 0, sizeof(readBuffer));                                                       // Xóa bộ đệm đọc
  uint32_t bytesRead = sdCard.readFile("/data/logs/app/log.txt", readBuffer, sizeof(readBuffer));  // Đọc tập tin từ thư mục lồng sâu

  if (bytesRead > 0) {           // Nếu đọc được dữ liệu
    Serial.print("Read ");       // In "Read"
    Serial.print(bytesRead);     // In số byte đã đọc
    Serial.println(" bytes:");   // In "bytes:"
    Serial.println(readBuffer);  // In nội dung đã đọc được

    // Xác minh nội dung
    if (strcmp(readBuffer, "Log entry from deeply nested file.") != 0) {  // So sánh nội dung đọc được với nội dung mong đợi
      Serial.println("File content doesn't match!");                      // In thông báo nếu nội dung không khớp
      success = false;                                                    // Đánh dấu kiểm tra là thất bại
    }
  } else {                                                         // Nếu không đọc được dữ liệu
    Serial.println("Failed to read file from nested directory!");  // In thông báo thất bại
    success = false;                                               // Đánh dấu kiểm tra là thất bại
  }

  endTest(success);  // Kết thúc bài kiểm tra với kết quả
}

// Kiểm tra 4: Xóa tập tin
void testFileDeletion() {      // Hàm kiểm tra xóa tập tin
  beginTest("File Deletion");  // Bắt đầu bài kiểm tra xóa tập tin
  bool success = true;         // Khởi tạo biến theo dõi trạng thái thành công

  // Tạo một tập tin tạm thời để xóa
  Serial.println("4.1: Creating temporary file 'temp.txt'");           // In thông báo đang tạo tập tin tạm thời
  if (!sdCard.createFile("temp.txt", "This file will be deleted.")) {  // Tạo tập tin tạm thời
    Serial.println("Failed to create temporary file!");                // In thông báo nếu tạo thất bại
    success = false;                                                   // Đánh dấu kiểm tra là thất bại
  }

  // Liệt kê thư mục để xác minh việc tạo tập tin
  Serial.println("4.2: Listing directory to verify file creation");  // In thông báo xác minh tạo tập tin
  if (!sdCard.listDirectory("/")) {                                  // Liệt kê nội dung thư mục gốc
    Serial.println("Failed to list directory!");                     // In thông báo nếu liệt kê thất bại
    success = false;                                                 // Đánh dấu kiểm tra là thất bại
  }

  // Xóa tập tin
  Serial.println("4.3: Deleting file 'temp.txt'");  // In thông báo đang xóa tập tin
  if (!sdCard.deleteFile("temp.txt")) {             // Xóa tập tin tạm thời
    Serial.println("Failed to delete file!");       // In thông báo nếu xóa thất bại
    success = false;                                // Đánh dấu kiểm tra là thất bại
  }

  // Liệt kê thư mục để xác minh việc xóa tập tin
  Serial.println("4.4: Listing directory to verify file deletion");  // In thông báo xác minh xóa tập tin
  if (!sdCard.listDirectory("/")) {                                  // Liệt kê nội dung thư mục gốc
    Serial.println("Failed to list directory!");                     // In thông báo nếu liệt kê thất bại
    success = false;                                                 // Đánh dấu kiểm tra là thất bại
  }

  // Thử đọc tập tin đã xóa (nên thất bại)
  Serial.println("4.5: Attempting to read deleted file (should fail)");              // In thông báo đang thử đọc tập tin đã xóa
  memset(readBuffer, 0, sizeof(readBuffer));                                         // Xóa bộ đệm đọc
  uint32_t bytesRead = sdCard.readFile("temp.txt", readBuffer, sizeof(readBuffer));  // Thử đọc tập tin đã xóa

  if (bytesRead > 0) {                                    // Nếu đọc được dữ liệu (không nên xảy ra)
    Serial.println("File still exists after deletion!");  // In thông báo tập tin vẫn tồn tại
    success = false;                                      // Đánh dấu kiểm tra là thất bại
  } else {                                                // Nếu không đọc được dữ liệu (điều này là đúng)
    Serial.println("File successfully deleted.");         // In thông báo xóa thành công
  }

  endTest(success);  // Kết thúc bài kiểm tra với kết quả
}

// Kiểm tra 5: Xử lý đường dẫn - Gốc và đường dẫn tuyệt đối
void testPathHandling() {      // Hàm kiểm tra xử lý đường dẫn
  beginTest("Path Handling");  // Bắt đầu bài kiểm tra xử lý đường dẫn
  bool success = true;         // Khởi tạo biến theo dõi trạng thái thành công

  // Kiểm tra các định dạng đường dẫn khác nhau
  Serial.println("5.1: Testing root path '/'");                 // In thông báo kiểm tra đường dẫn gốc
  if (!sdCard.listDirectory("/")) {                             // Liệt kê thư mục gốc với "/"
    Serial.println("Failed to list root directory with '/'!");  // In thông báo nếu liệt kê thất bại
    success = false;                                            // Đánh dấu kiểm tra là thất bại
  }

  Serial.println("5.2: Testing empty path ''");                        // In thông báo kiểm tra đường dẫn rỗng
  if (!sdCard.listDirectory("")) {                                     // Liệt kê thư mục gốc với đường dẫn rỗng
    Serial.println("Failed to list root directory with empty path!");  // In thông báo nếu liệt kê thất bại
    success = false;                                                   // Đánh dấu kiểm tra là thất bại
  }

  Serial.println("5.3: Testing absolute path with trailing slash '/data/'");  // In thông báo kiểm tra đường dẫn tuyệt đối
  if (!sdCard.listDirectory("/data/")) {                                      // Liệt kê thư mục với dấu gạch chéo cuối
    Serial.println("Failed to list directory with trailing slash!");          // In thông báo nếu liệt kê thất bại
    success = false;                                                          // Đánh dấu kiểm tra là thất bại
  }

  Serial.println("5.4: Creating file with absolute path");                           // In thông báo tạo tập tin với đường dẫn tuyệt đối
  if (!sdCard.createFileInPath("/absolute_path.txt", "File with absolute path.")) {  // Tạo tập tin với đường dẫn tuyệt đối
    Serial.println("Failed to create file with absolute path!");                     // In thông báo nếu tạo thất bại
    success = false;                                                                 // Đánh dấu kiểm tra là thất bại
  }

  Serial.println("5.5: Reading file with absolute path");                                      // In thông báo đọc tập tin với đường dẫn tuyệt đối
  memset(readBuffer, 0, sizeof(readBuffer));                                                   // Xóa bộ đệm đọc
  uint32_t bytesRead = sdCard.readFile("/absolute_path.txt", readBuffer, sizeof(readBuffer));  // Đọc tập tin với đường dẫn tuyệt đối

  if (bytesRead > 0) {           // Nếu đọc được dữ liệu
    Serial.print("Read ");       // In "Read"
    Serial.print(bytesRead);     // In số byte đã đọc
    Serial.println(" bytes:");   // In "bytes:"
    Serial.println(readBuffer);  // In nội dung đã đọc được

    // Xác minh nội dung
    if (strcmp(readBuffer, "File with absolute path.") != 0) {  // So sánh nội dung đọc được với nội dung mong đợi
      Serial.println("File content doesn't match!");            // In thông báo nếu nội dung không khớp
      success = false;                                          // Đánh dấu kiểm tra là thất bại
    }
  } else {                                                      // Nếu không đọc được dữ liệu
    Serial.println("Failed to read file with absolute path!");  // In thông báo thất bại
    success = false;                                            // Đánh dấu kiểm tra là thất bại
  }

  endTest(success);  // Kết thúc bài kiểm tra với kết quả
}

void setup() {
  // Khởi tạo Serial để debug
  Serial.begin(115200);  // Thiết lập giao tiếp Serial ở tốc độ 115200 baud
  delay(3000);           // Đợi 3 giây để Serial khởi tạo

  Serial.println("ZBSdCard Complete API Test");      // In tiêu đề chương trình kiểm tra
  Serial.println("==============================");  // In đường phân cách

  // Khởi tạo thẻ SD
  Serial.println("Initializing SD card...");         // In thông báo khởi tạo thẻ SD
  if (!sdCard.begin(SD_CS_PIN)) {                    // Khởi tạo thẻ SD với chân CS đã định nghĩa
    Serial.print("Initialization failed! Error: ");  // In thông báo khởi tạo thất bại
    Serial.println(sdCard.getErrorMessage());        // In thông báo lỗi cụ thể
    while (1)
      ;  // Dừng thực thi
  }
  Serial.println("SD card initialized successfully!\n");  // In thông báo khởi tạo thành công

  // Chạy tất cả các bài kiểm tra
  testBasicFileOperations();  // Kiểm tra thao tác tập tin cơ bản
  testDirectoryOperations();  // Kiểm tra thao tác thư mục
  testNestedDirectories();    // Kiểm tra thư mục lồng nhau
  testFileDeletion();         // Kiểm tra xóa tập tin
  testPathHandling();         // Kiểm tra xử lý đường dẫn

  // Báo cáo kết quả kiểm tra tổng thể
  Serial.println("\n==============================");  // In đường phân cách
  Serial.print("OVERALL RESULTS: ");                   // In tiêu đề kết quả tổng thể
  Serial.print(testsPassed);                           // In số bài kiểm tra đã vượt qua
  Serial.print("/");                                   // In dấu "/"
  Serial.print(testsRun);                              // In tổng số bài kiểm tra
  Serial.print(" tests passed (");                     // In "tests passed ("
  Serial.print((testsPassed * 100) / testsRun);        // In phần trăm thành công
  Serial.println("%)");                                // In ")%"

  if (testsPassed == testsRun) {            // Nếu tất cả các bài kiểm tra đều vượt qua
    Serial.println("All tests PASSED! ✓");  // In thông báo tất cả đều vượt qua
  } else {                                  // Nếu có bài kiểm tra thất bại
    Serial.print(testsRun - testsPassed);   // In số bài kiểm tra thất bại
    Serial.println(" tests FAILED! ✗");     // In thông báo có bài kiểm tra thất bại
  }
  Serial.println("==============================");  // In đường phân cách
}

void loop() {
  // Không có gì để làm trong vòng lặp
  delay(1000);  // Đợi 1 giây
}