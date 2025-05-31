/**
 * Trình xem tập tin SD Card với màn hình OLED SSD1306
 * 
 * Sketch này cho phép duyệt và xem các tập tin trên thẻ SD
 * sử dụng màn hình OLED SSD1306.
 * Tính năng:
 * - Duyệt tập tin với điều hướng thư mục
 * - Xem tập tin văn bản
 * - Cuộn cho nội dung tập tin dài
 *
 * Link bài viết hướng dẫn chi tiết: https://elearning.chipstack.vn/mod/page/view.php?id=165
 */
#include <ZBPrint.h>           // Bao gồm thư viện ZBPrint
#include <SPI.h>               // Bao gồm thư viện SPI cho giao tiếp
#include <ZBSdCard.h>          // Bao gồm thư viện ZBSdCard để làm việc với thẻ SD
#include <Wire.h>              // Bao gồm thư viện Wire cho giao tiếp I2C
#include <Adafruit_GFX.h>      // Bao gồm thư viện đồ họa Adafruit GFX
#include <Adafruit_SSD1306.h>  // Bao gồm thư viện cho màn hình OLED SSD1306

// Cấu hình OLED
#define SCREEN_WIDTH 128                                                   // Định nghĩa chiều rộng màn hình là 128 pixel
#define SCREEN_HEIGHT 64                                                   // Định nghĩa chiều cao màn hình là 64 pixel
#define OLED_RESET -1                                                      // Định nghĩa chân reset màn hình (-1 nghĩa là dùng chân reset mặc định)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  // Khởi tạo đối tượng màn hình

// Cấu hình thẻ SD
#define SD_CS_PIN 10  // Định nghĩa chân Chip Select cho thẻ SD là chân 10
ZBSdCard sdCard;      // Khởi tạo đối tượng thẻ SD

// Chân nút bấm - Cần khớp với kết nối vật lý của bạn
#define BTN_UP 2      // Nút lên kết nối với chân 2
#define BTN_DOWN 3    // Nút xuống kết nối với chân 3
#define BTN_SELECT 4  // Nút chọn kết nối với chân 4
#define BTN_BACK 5    // Nút quay lại kết nối với chân 5

// Biến cho trình xem tập tin
#define MAX_FILES 32            // Số lượng tập tin tối đa có thể hiển thị
#define MAX_FILENAME_LENGTH 20  // Độ dài tối đa cho tên tập tin
#define MAX_PATH_LENGTH 100     // Độ dài tối đa cho đường dẫn
#define LINES_PER_SCREEN 6      // Số dòng hiển thị trên mỗi màn hình
#define MAX_FILE_BUFFER 1024    // Kích thước bộ đệm tối đa cho đọc tập tin

// Bộ đệm cho thao tác tập tin
char fileBuffer[MAX_FILE_BUFFER];  // Bộ đệm dùng để lưu nội dung tập tin

// Đường dẫn thư mục hiện tại
char currentPath[MAX_PATH_LENGTH] = "/";  // Bắt đầu ở thư mục gốc

// Mảng để lưu tên tập tin
char fileList[MAX_FILES][MAX_FILENAME_LENGTH];  // Mảng 2 chiều để lưu tên các tập tin và thư mục
bool isDirectory[MAX_FILES];                    // Cờ để đánh dấu nếu mục là thư mục
int fileCount = 0;                              // Số lượng tập tin tìm thấy
int currentFile = 0;                            // Vị trí tập tin được chọn hiện tại
int scrollPosition = 0;                         // Vị trí cuộn hiện tại
int viewMode = 0;                               // Chế độ xem: 0 = danh sách tập tin, 1 = nội dung tập tin

// Khai báo các hàm
void scanFiles(const char* path);                              // Quét tập tin trong thư mục
void displayFileList();                                        // Hiển thị danh sách tập tin
void displayFileContent(const char* filename);                 // Hiển thị nội dung tập tin
void handleButtons();                                          // Xử lý các nút bấm
bool readButton(int pin);                                      // Đọc trạng thái nút bấm với chống dội
void navigateToFolder(const char* folderName);                 // Di chuyển vào thư mục con
void navigateUp();                                             // Di chuyển lên thư mục cha
void constructFullPath(char* fullPath, const char* filename);  // Tạo đường dẫn đầy đủ

void setup() {
  // Khởi tạo cổng nối tiếp cho debug
  Serial.begin(115200);  // Khởi tạo giao tiếp Serial với tốc độ 115200 baud
  delay(1000);           // Đợi 1 giây để Serial ổn định

  Serial.println("SD Card File Viewer");  // In thông báo khởi động

  // Khởi tạo các nút bấm với điện trở kéo lên
  pinMode(BTN_UP, INPUT_PULLUP);      // Cấu hình nút lên với điện trở kéo lên
  pinMode(BTN_DOWN, INPUT_PULLUP);    // Cấu hình nút xuống với điện trở kéo lên
  pinMode(BTN_SELECT, INPUT_PULLUP);  // Cấu hình nút chọn với điện trở kéo lên
  pinMode(BTN_BACK, INPUT_PULLUP);    // Cấu hình nút quay lại với điện trở kéo lên

  // Khởi tạo màn hình OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Khởi tạo màn hình với địa chỉ I2C 0x3C
    Serial.println(F("SSD1306 allocation failed"));  // In thông báo lỗi nếu khởi tạo thất bại
    for (;;)
      ;  // Không tiếp tục, lặp vô hạn
  }

  // Khởi tạo thẻ SD
  display.clearDisplay();                      // Xóa màn hình
  display.setTextSize(1);                      // Đặt kích thước văn bản là 1
  display.setTextColor(SSD1306_WHITE);         // Đặt màu văn bản là trắng
  display.setCursor(0, 0);                     // Đặt con trỏ tại vị trí (0,0)
  display.println("Initializing SD Card...");  // Hiển thị thông báo đang khởi tạo thẻ SD
  display.display();                           // Cập nhật hiển thị

  if (!sdCard.begin(SD_CS_PIN)) {               // Khởi tạo thẻ SD với chân CS đã định nghĩa
    display.clearDisplay();                     // Xóa màn hình
    display.setCursor(0, 0);                    // Đặt con trỏ về vị trí (0,0)
    display.println("SD Card init failed!");    // Hiển thị thông báo lỗi khởi tạo
    display.println("Error: ");                 // Chuẩn bị hiển thị thông báo lỗi
    display.println(sdCard.getErrorMessage());  // Hiển thị thông báo lỗi cụ thể
    display.display();                          // Cập nhật hiển thị
    for (;;)
      ;  // Không tiếp tục, lặp vô hạn
  }

  sdCard.createFile("test2.txt", "This is test file #2 content.");                   // Tạo tập tin test2.txt với nội dung cho thử nghiệm
  sdCard.createFile("test3.txt", "This is test file #3 content.");                   // Tạo tập tin test3.txt với nội dung cho thử nghiệm
  sdCard.createDirectory("NEWDIR");                                                  // Tạo thư mục NEWDIR cho thử nghiệm
  sdCard.createFileInPath("/NEWDIR/test2.txt", "This is test file #2 in testdir.");  // Tạo tập tin trong thư mục NEWDIR

  // Hiển thị thông báo trong khi đang quét tập tin
  display.clearDisplay();                 // Xóa màn hình
  display.setCursor(0, 0);                // Đặt con trỏ tại vị trí (0,0)
  display.println("Reading SD Card...");  // Hiển thị thông báo đang đọc thẻ SD
  display.println("Please wait...");      // Hiển thị thông báo đợi
  display.display();                      // Cập nhật hiển thị

  // Quét tập tin trên thẻ SD (bắt đầu từ thư mục gốc)
  scanFiles("/");  // Quét các tập tin trong thư mục gốc

  // Hiển thị danh sách tập tin
  displayFileList();  // Hiển thị danh sách tập tin đã quét
}

void loop() {
  // In trạng thái hiện tại của các nút bấm để debug
  Serial.print("Buttons: ");              // In tiêu đề thông tin nút bấm
  Serial.print(digitalRead(BTN_UP));      // In trạng thái nút lên
  Serial.print(digitalRead(BTN_DOWN));    // In trạng thái nút xuống
  Serial.print(digitalRead(BTN_SELECT));  // In trạng thái nút chọn
  Serial.println(digitalRead(BTN_BACK));  // In trạng thái nút quay lại

  // Xử lý các nút bấm
  handleButtons();  // Xử lý phản hồi cho các nút bấm
  delay(100);       // Đợi nhỏ 100ms để chống dội nút
}

// Quét tập tin trong thư mục sử dụng phương thức mới
void scanFiles(const char* path) {
  fileCount = 0;       // Đặt lại số lượng tập tin về 0
  currentFile = 0;     // Đặt lại vị trí tập tin hiện tại về 0
  scrollPosition = 0;  // Đặt lại vị trí cuộn về 0

  // Lưu đường dẫn hiện tại
  strncpy(currentPath, path, MAX_PATH_LENGTH - 1);  // Sao chép đường dẫn đến biến currentPath
  currentPath[MAX_PATH_LENGTH - 1] = '\0';          // Đảm bảo kết thúc chuỗi

  // Hiển thị thông báo đang quét
  display.clearDisplay();                    // Xóa màn hình
  display.setCursor(0, 0);                   // Đặt con trỏ tại vị trí (0,0)
  display.println("Scanning directory...");  // Hiển thị thông báo đang quét thư mục
  display.display();                         // Cập nhật hiển thị

  // Không còn thêm mục "lên thư mục" nữa
  // Nút quay lại sẽ xử lý chức năng này thay thế

  // Sử dụng hàm mới để lấy các mục trong thư mục
  int foundEntries = 0;  // Biến đếm số mục tìm thấy
  if (!sdCard.listDirectoryEntries(path, &fileList[fileCount], &isDirectory[fileCount],
                                   MAX_FILES - fileCount, &foundEntries)) {  // Liệt kê mục trong thư mục
    display.setCursor(0, 24);                                                // Đặt con trỏ tại vị trí (0,24)
    display.println("Failed to list directory!");                            // Hiển thị thông báo lỗi
    display.display();                                                       // Cập nhật hiển thị
    delay(2000);                                                             // Đợi 2 giây để xem thông báo
    return;                                                                  // Thoát khỏi hàm
  }

  fileCount += foundEntries;  // Cập nhật tổng số tập tin tìm thấy

  Serial.print("Scanned directory: ");  // In thông tin về thư mục đã quét
  Serial.println(path);                 // In đường dẫn thư mục
  Serial.print("Found ");               // In thông tin về số lượng tìm thấy
  Serial.print(fileCount);              // In số lượng tập tin/thư mục
  Serial.println(" files/dirs.");       // In phần còn lại của thông báo
}

// Hiển thị danh sách tập tin trên màn hình OLED
void displayFileList() {
  display.clearDisplay();   // Xóa màn hình
  display.setCursor(0, 0);  // Đặt con trỏ tại vị trí (0,0)

  // Luôn hiển thị "File Browser" làm tiêu đề
  display.println("File Browser");                             // Hiển thị tiêu đề trình duyệt tập tin
  display.drawLine(0, 9, SCREEN_WIDTH - 1, 9, SSD1306_WHITE);  // Vẽ đường phân cách sau tiêu đề

  int startIdx = max(0, min(scrollPosition, fileCount - LINES_PER_SCREEN));  // Tính vị trí bắt đầu hiển thị
  int endIdx = min(startIdx + LINES_PER_SCREEN, fileCount);                  // Tính vị trí kết thúc hiển thị

  if (fileCount == 0) {                  // Nếu không có tập tin nào
    display.setCursor(0, 16);            // Đặt con trỏ tại vị trí (0,16)
    display.println("No files found!");  // Hiển thị thông báo không tìm thấy tập tin
  } else {
    for (int i = startIdx; i < endIdx; i++) {  // Lặp qua các tập tin trong phạm vi hiển thị
      // Làm nổi bật tập tin được chọn
      if (i == currentFile) {                                                          // Nếu đây là tập tin hiện tại
        display.fillRect(0, 11 + (i - startIdx) * 9, SCREEN_WIDTH, 9, SSD1306_WHITE);  // Vẽ hình chữ nhật nền trắng
        display.setTextColor(SSD1306_BLACK);                                           // Đặt màu chữ là đen (ngược với nền)
      } else {
        display.setTextColor(SSD1306_WHITE);  // Đặt màu chữ là trắng
      }

      display.setCursor(2, 12 + (i - startIdx) * 9);  // Đặt vị trí con trỏ

      // Hiển thị chỉ báo thư mục cho các thư mục
      if (isDirectory[i]) {          // Nếu đây là thư mục
        display.print("[");          // In dấu [ trước tên thư mục
        display.print(fileList[i]);  // In tên thư mục
        display.print("]");          // In dấu ] sau tên thư mục
      }
      // Tập tin thông thường
      else {
        display.println(fileList[i]);  // In tên tập tin
      }

      display.setTextColor(SSD1306_WHITE);  // Đặt lại màu chữ là trắng
    }

    // Vẽ thanh cuộn nếu cần
    if (fileCount > LINES_PER_SCREEN) {                                                             // Nếu có nhiều tập tin hơn số dòng hiển thị
      int barHeight = max(5, (LINES_PER_SCREEN * SCREEN_HEIGHT) / fileCount);                       // Tính chiều cao thanh cuộn
      int barPosition = (startIdx * (SCREEN_HEIGHT - barHeight)) / (fileCount - LINES_PER_SCREEN);  // Tính vị trí thanh cuộn
      display.drawRect(SCREEN_WIDTH - 3, 10, 3, SCREEN_HEIGHT - 10, SSD1306_WHITE);                 // Vẽ viền thanh cuộn
      display.fillRect(SCREEN_WIDTH - 3, 10 + barPosition, 3, barHeight, SSD1306_WHITE);            // Vẽ thanh cuộn
    }
  }

  display.display();  // Cập nhật hiển thị
}

// Hiển thị nội dung tập tin trên màn hình OLED
void displayFileContent(const char* filename) {
  display.clearDisplay();  // Xóa màn hình

  // Tạo đường dẫn đầy đủ đến tập tin
  char fullPath[MAX_PATH_LENGTH + MAX_FILENAME_LENGTH];  // Khai báo biến lưu đường dẫn đầy đủ
  constructFullPath(fullPath, filename);                 // Tạo đường dẫn đầy đủ

  // Hiển thị tên tập tin ở trên cùng
  display.setCursor(0, 0);  // Đặt con trỏ tại vị trí (0,0)
  // Hiển thị chỉ tên tập tin, không phải đường dẫn đầy đủ
  display.println(filename);                                   // In tên tập tin
  display.drawLine(0, 9, SCREEN_WIDTH - 1, 9, SSD1306_WHITE);  // Vẽ đường phân cách sau tiêu đề

  // Đọc nội dung tập tin
  memset(fileBuffer, 0, sizeof(fileBuffer));                                           // Xóa bộ đệm
  uint32_t bytesRead = sdCard.readFile(fullPath, fileBuffer, sizeof(fileBuffer) - 1);  // Đọc nội dung tập tin

  if (bytesRead == 0) {                     // Nếu không đọc được nội dung
    display.setCursor(0, 16);               // Đặt con trỏ tại vị trí (0,16)
    display.println("Error reading file");  // Hiển thị thông báo lỗi đọc tập tin
    display.println("or file is empty.");   // Hoặc tập tin rỗng
  } else {
    // Hiển thị nội dung với ngắt dòng
    int lineHeight = 8;                              // Chiều cao mỗi dòng (pixel)
    int x = 0, y = 11;                               // Vị trí bắt đầu hiển thị
    int contentLength = strlen(fileBuffer);          // Độ dài nội dung
    int charWidth = 6;                               // Chiều rộng xấp xỉ của một ký tự
    int maxCharsPerLine = SCREEN_WIDTH / charWidth;  // Số ký tự tối đa trên một dòng

    // Áp dụng cuộn cho chế độ xem nội dung
    int startPos = min(scrollPosition * maxCharsPerLine, contentLength);  // Tính vị trí bắt đầu cuộn

    // Hiển thị nội dung từ vị trí cuộn
    int pos = startPos;                                 // Vị trí hiện tại trong nội dung
    while (pos < contentLength && y < SCREEN_HEIGHT) {  // Lặp qua các ký tự trong nội dung
      if (fileBuffer[pos] == '\n') {                    // Nếu gặp ký tự xuống dòng
        // Xử lý ký tự xuống dòng
        x = 0;                               // Đặt lại vị trí x về đầu dòng
        y += lineHeight;                     // Tăng vị trí y lên một dòng
      } else if (fileBuffer[pos] == '\r') {  // Nếu gặp ký tự trả về đầu dòng
        // Bỏ qua ký tự trả về đầu dòng
      } else {
        // Hiển thị ký tự
        display.setCursor(x, y);         // Đặt con trỏ tại vị trí hiện tại
        display.write(fileBuffer[pos]);  // In ký tự
        x += charWidth;                  // Tăng vị trí x cho ký tự tiếp theo

        // Ngắt dòng nếu cần
        if (x >= SCREEN_WIDTH) {  // Nếu vượt quá chiều rộng màn hình
          x = 0;                  // Đặt lại vị trí x về đầu dòng
          y += lineHeight;        // Tăng vị trí y lên một dòng
        }
      }

      pos++;  // Tiến đến ký tự tiếp theo

      // Nếu đã lấp đầy màn hình, dừng lại
      if (y > SCREEN_HEIGHT - lineHeight)  // Nếu vượt quá chiều cao màn hình
        break;                             // Thoát khỏi vòng lặp
    }

    // Vẽ thanh cuộn nếu cần
    int totalLines = (contentLength + maxCharsPerLine - 1) / maxCharsPerLine;                     // Tính tổng số dòng
    if (totalLines > (SCREEN_HEIGHT - 11) / lineHeight) {                                         // Nếu có nhiều dòng hơn có thể hiển thị
      int visibleLines = (SCREEN_HEIGHT - 11) / lineHeight;                                       // Số dòng có thể hiển thị
      int barHeight = max(5, (visibleLines * (SCREEN_HEIGHT - 11)) / totalLines);                 // Chiều cao thanh cuộn
      int maxScrollPosition = totalLines - visibleLines;                                          // Vị trí cuộn tối đa
      int barPosition = (scrollPosition * (SCREEN_HEIGHT - 11 - barHeight)) / maxScrollPosition;  // Vị trí thanh cuộn
      display.drawRect(SCREEN_WIDTH - 3, 10, 3, SCREEN_HEIGHT - 10, SSD1306_WHITE);               // Vẽ viền thanh cuộn
      display.fillRect(SCREEN_WIDTH - 3, 10 + barPosition, 3, barHeight, SSD1306_WHITE);          // Vẽ thanh cuộn
    }
  }

  display.display();  // Cập nhật hiển thị
}

// Xử lý các nút bấm
void handleButtons() {
  // Nút UP (lên)
  if (digitalRead(BTN_UP) == LOW) {         // Nếu nút UP được nhấn (mức thấp)
    delay(50);                              // Đợi 50ms để chống dội nút
    if (digitalRead(BTN_UP) == LOW) {       // Kiểm tra lại nếu nút vẫn được nhấn
      Serial.println("UP button pressed");  // In thông báo nút UP được nhấn
      if (viewMode == 0) {                  // Nếu đang ở chế độ danh sách tập tin
        // Trong chế độ danh sách tập tin
        if (currentFile > 0) {               // Nếu không phải tập tin đầu tiên
          currentFile--;                     // Di chuyển lên một tập tin
          if (currentFile < scrollPosition)  // Nếu cần cuộn lên
            scrollPosition = currentFile;    // Cập nhật vị trí cuộn
          displayFileList();                 // Cập nhật hiển thị danh sách
        }
      } else {
        // Trong chế độ xem tập tin
        if (scrollPosition > 0) {                     // Nếu không phải đang ở đầu tập tin
          scrollPosition--;                           // Cuộn lên một dòng
          displayFileContent(fileList[currentFile]);  // Cập nhật hiển thị nội dung
        }
      }

      // Đợi cho đến khi nút được nhả ra
      while (digitalRead(BTN_UP) == LOW) {  // Trong khi nút vẫn được nhấn
        delay(10);                          // Đợi 10ms
      }
    }
  }

  // Nút DOWN (xuống)
  if (digitalRead(BTN_DOWN) == LOW) {         // Nếu nút DOWN được nhấn (mức thấp)
    delay(50);                                // Đợi 50ms để chống dội nút
    if (digitalRead(BTN_DOWN) == LOW) {       // Kiểm tra lại nếu nút vẫn được nhấn
      Serial.println("DOWN button pressed");  // In thông báo nút DOWN được nhấn
      if (viewMode == 0) {                    // Nếu đang ở chế độ danh sách tập tin
        // Trong chế độ danh sách tập tin
        if (currentFile < fileCount - 1) {                        // Nếu không phải tập tin cuối cùng
          currentFile++;                                          // Di chuyển xuống một tập tin
          if (currentFile >= scrollPosition + LINES_PER_SCREEN)   // Nếu cần cuộn xuống
            scrollPosition = currentFile - LINES_PER_SCREEN + 1;  // Cập nhật vị trí cuộn
          displayFileList();                                      // Cập nhật hiển thị danh sách
        }
      } else {
        // Trong chế độ xem tập tin
        scrollPosition++;                           // Cuộn xuống một dòng
        displayFileContent(fileList[currentFile]);  // Cập nhật hiển thị nội dung
      }

      // Đợi cho đến khi nút được nhả ra
      while (digitalRead(BTN_DOWN) == LOW) {  // Trong khi nút vẫn được nhấn
        delay(10);                            // Đợi 10ms
      }
    }
  }

  // Nút SELECT (chọn)
  if (digitalRead(BTN_SELECT) == LOW) {         // Nếu nút SELECT được nhấn (mức thấp)
    delay(50);                                  // Đợi 50ms để chống dội nút
    if (digitalRead(BTN_SELECT) == LOW) {       // Kiểm tra lại nếu nút vẫn được nhấn
      Serial.println("SELECT button pressed");  // In thông báo nút SELECT được nhấn

      if (viewMode == 0 && fileCount > 0) {  // Nếu đang ở chế độ danh sách và có tập tin
        // Nếu mục được chọn là thư mục
        if (isDirectory[currentFile]) {                    // Nếu đây là thư mục
          if (strcmp(fileList[currentFile], "..") == 0) {  // Nếu là mục ".." (lên một cấp)
            // Đi lên một thư mục
            navigateUp();  // Điều hướng lên thư mục cha
          } else {
            // Điều hướng vào thư mục được chọn
            navigateToFolder(fileList[currentFile]);  // Vào thư mục được chọn
          }
        } else {
          // Chuyển sang chế độ xem tập tin cho tập tin
          viewMode = 1;                               // Đặt chế độ xem là xem nội dung tập tin
          scrollPosition = 0;                         // Đặt lại vị trí cuộn về đầu
          displayFileContent(fileList[currentFile]);  // Hiển thị nội dung tập tin
        }
      }

      // Đợi cho đến khi nút được nhả ra
      while (digitalRead(BTN_SELECT) == LOW) {  // Trong khi nút vẫn được nhấn
        delay(10);                              // Đợi 10ms
      }
    }
  }

  // Nút BACK (quay lại)
  if (digitalRead(BTN_BACK) == LOW) {         // Nếu nút BACK được nhấn (mức thấp)
    delay(50);                                // Đợi 50ms để chống dội nút
    if (digitalRead(BTN_BACK) == LOW) {       // Kiểm tra lại nếu nút vẫn được nhấn
      Serial.println("BACK button pressed");  // In thông báo nút BACK được nhấn
      if (viewMode == 1) {                    // Nếu đang ở chế độ xem tập tin
        // Quay lại chế độ danh sách tập tin từ chế độ xem tập tin
        viewMode = 0;                                                 // Đặt chế độ xem là danh sách tập tin
        scrollPosition = max(0, currentFile - LINES_PER_SCREEN + 1);  // Cập nhật vị trí cuộn
        displayFileList();                                            // Hiển thị danh sách tập tin
      } else if (viewMode == 0 && strcmp(currentPath, "/") != 0) {    // Nếu đang ở chế độ danh sách và không phải thư mục gốc
        // Đi lên một thư mục từ trình duyệt tập tin (nếu không ở thư mục gốc)
        navigateUp();  // Điều hướng lên thư mục cha
      }

      // Đợi cho đến khi nút được nhả ra
      while (digitalRead(BTN_BACK) == LOW) {  // Trong khi nút vẫn được nhấn
        delay(10);                            // Đợi 10ms
      }
    }
  }
}

// Điều hướng đến thư mục con
void navigateToFolder(const char* folderName) {
  char newPath[MAX_PATH_LENGTH];  // Khai báo biến cho đường dẫn mới

  // Tạo đường dẫn mới
  if (strcmp(currentPath, "/") == 0) {  // Nếu đang ở thư mục gốc
    // Chúng ta đang ở thư mục gốc
    snprintf(newPath, MAX_PATH_LENGTH, "/%s", folderName);  // Tạo đường dẫn "/tên_thư_mục"
  } else {
    // Chúng ta đang ở thư mục con
    snprintf(newPath, MAX_PATH_LENGTH, "%s/%s", currentPath, folderName);  // Tạo đường dẫn "đường_dẫn_hiện_tại/tên_thư_mục"
  }

  // Quét thư mục mới
  scanFiles(newPath);  // Quét tập tin trong thư mục mới

  // Hiển thị danh sách tập tin mới
  displayFileList();  // Hiển thị danh sách tập tin
}

// Điều hướng lên thư mục cha
void navigateUp() {
  // Nếu đã ở thư mục gốc, không làm gì cả
  if (strcmp(currentPath, "/") == 0) {  // Nếu đang ở thư mục gốc
    return;                             // Thoát khỏi hàm
  }

  char newPath[MAX_PATH_LENGTH];  // Khai báo biến cho đường dẫn mới
  strcpy(newPath, currentPath);   // Sao chép đường dẫn hiện tại

  // Tìm dấu gạch chéo cuối cùng trong đường dẫn
  char* lastSlash = strrchr(newPath, '/');  // Tìm vị trí của dấu / cuối cùng

  if (lastSlash == newPath) {  // Nếu dấu / cuối cùng là ký tự đầu tiên
    // Chúng ta đang ở thư mục cấp cao nhất, quay lại thư mục gốc
    strcpy(newPath, "/");  // Đặt đường dẫn là "/"
  } else {
    // Cắt đường dẫn tại dấu gạch chéo cuối cùng
    *lastSlash = '\0';  // Kết thúc chuỗi tại vị trí dấu / cuối cùng
  }

  // Quét thư mục cha
  scanFiles(newPath);  // Quét tập tin trong thư mục cha

  // Hiển thị danh sách tập tin đã cập nhật
  displayFileList();  // Hiển thị danh sách tập tin
}

// Tạo đường dẫn đầy đủ từ thư mục hiện tại và tên tập tin
void constructFullPath(char* fullPath, const char* filename) {
  if (strcmp(currentPath, "/") == 0) {  // Nếu thư mục hiện tại là thư mục gốc
    // Thư mục gốc
    sprintf(fullPath, "/%s", filename);  // Tạo đường dẫn "/tên_tập_tin"
  } else {
    // Thư mục con
    sprintf(fullPath, "%s/%s", currentPath, filename);  // Tạo đường dẫn "đường_dẫn_hiện_tại/tên_tập_tin"
  }
}

// Đọc nút bấm với chống dội
bool readButton(int pin) {
  static unsigned long lastDebounceTime = 0;  // Thời điểm dội nút cuối cùng
  static int lastButtonState = HIGH;          // Trạng thái nút trước đó

  // Trạng thái nút hiện tại
  int buttonState = digitalRead(pin);  // Đọc trạng thái nút

  // Kiểm tra nếu trạng thái nút đã thay đổi
  if (buttonState != lastButtonState) {  // Nếu trạng thái khác với lần trước
    lastDebounceTime = millis();         // Cập nhật thời điểm thay đổi
  }

  lastButtonState = buttonState;  // Cập nhật trạng thái nút trước đó

  // Kiểm tra nếu đã đủ thời gian kể từ lần thay đổi cuối cùng
  if ((millis() - lastDebounceTime) > 50) {  // Nếu đã qua 50ms
    if (buttonState == LOW) {                // Nếu nút đang được nhấn
      // Nút được nhấn, đợi cho đến khi nhả ra
      while (digitalRead(pin) == LOW) {  // Trong khi nút vẫn được nhấn
        delay(10);                       // Đợi 10ms
      }
      return true;  // Trả về true để xác nhận nút được nhấn
    }
  }

  return false;  // Trả về false nếu nút không được nhấn
}