## ESP32-S3 Firmware (esp_idf_esp32s3)
本節說明使用 ESP32-S3 韌體的相關資訊，它負責根據搖桿和遠端網頁介面的輸入控制馬達。

## 程式碼
*   **main**: 存放主要程式碼
    *   **http.cpp/http.h**: 處理 HTTP 伺服器相關邏輯。
    *   **joystick.cpp/joystick.h**: 處理搖桿輸入。
    *   **main.cpp**: 程式主要進入點。
    *   **ULN2003.cpp/ULN2003.h**: 控制 28BYJ-48 步進馬達。
    *   **wifi.cpp/wifi.h**: 處理 Wi-Fi 連線。
    *   **wifi_config.h**: 存放 Wi-Fi SSID 和密碼 (需自行修改)。
## 硬體使用
*  **ESP32S3 開發板**: 型號：**Goouuu ESP32-S3 N16R8 16M flash/8M PSRAM 雙Type-C USB / W2812 RGB**，用於控制步進馬達和處理 HTTP 請求。
![Image](https://www.taiwansensor.com.tw/wp-content/uploads/2023/12/O1CN01Zmz8W21kAXm0aySdE_672934643.jpg)

*  **28BYJ-48 步進馬達和ULN2003驅動版**: 用於旋轉。
![Image](https://m.media-amazon.com/images/I/61Ml9ebd2kL._AC_UF894,1000_QL80_.jpg)

*  **搖桿模組**: 用於控制步進馬達的旋轉方向。
![Image](https://down-br.img.susercontent.com/file/bcf905ecd9ff177fde7f4fbcafc9c400)
*  **電源模組**: 專門用於供應步進馬達，需搭配7-12Vdc電源供應器。
![Image](https://http2.mlstatic.com/D_613161-MLA47599398871_092021-C.jpg)

## 安裝

1. **在VS Code安裝 ESP-IDF**:
    請按照 ESP-IDF Extension for VS Code Overview安裝 ESP-IDF 開發環境：[link](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)

2. **取得專案程式碼**:
    ```bash
    git clone https://github.com/whiteSHADOW1234/PetCam.git
    cd esp_idf_esp32s3
    ```

3. **設定 Wi-Fi 資訊**:
有兩種方法可以設定 Wi-Fi 資訊：
    1. 新增 `main/wifi_config.h` 檔案，將 `EXAMPLE_ESP_WIFI_SSID` 和 `EXAMPLE_ESP_WIFI_PASS` 設定為你的 Wi-Fi SSID 和密碼：

    ```cpp
    #ifndef WIFI_CONFIG_H
    #define WIFI_CONFIG_H

    #define EXAMPLE_ESP_WIFI_SSID "你的 Wi-Fi SSID"
    #define EXAMPLE_ESP_WIFI_PASS "你的 Wi-Fi 密碼"

    #endif
    ```
    2. 在 `main/wifi.cpp` 註解或刪除`#include "wifi_config.h"`，並將 `EXAMPLE_ESP_WIFI_SSID` 和 `EXAMPLE_ESP_WIFI_PASS` 設定為你的 Wi-Fi SSID 和密碼。


## 使用方法

1. **設定Espressif Device Target**:

    在 VS Code 中，按下 `Ctrl+Shift+P`，輸入 `ESP-IDF: Set Espressif Device Target`，選擇 `esp32s3`，並選擇第一個`ESP32-S3 chip (via builtin USB-JTAG)`。

2. **設定序列埠**:

    在 VS Code 中，按下 `Ctrl+Shift+P`，輸入 `ESP-IDF: Select Port to Use (COM, tty, usbserial)`，
    設定為 ESP32S3 開發板的序列埠。

3. **編譯、燒錄和監控**:

    在 VS Code 中，按下 `Ctrl+Shift+P`，輸入 `ESP-IDF: Build, Flash and Start a Monitor on your Device`。
4. **連接到 Wi-Fi**:
    Server-View 的程式會自動和 ESP32S3 連線，按下`左/右轉`按鈕，遠端的步進馬達將會旋轉。
## 接線

**28BYJ-48 步進馬達 (使用 ULN2003 驅動):**

馬達接上ULN2003驅動器不用特意接線，ULN2003驅動器接上ESP32S3開發板時參照下表。

| 28BYJ-48 | ULN2003 | ESP32S3/電源模組 |
| -------- | ------- | ---------------- |
| 紅色     | +       | 5V(電源模組)     |
| 橙色     | IN1     | GPIO15           |
| 黃色     | IN2     | GPIO16           |
| 粉色     | IN3     | GPIO17           |
| 藍色     | IN4     | GPIO18           |
| 黑色     | -       | GND(電源模組)    |

**搖桿模組:**

| 搖桿模組 | ESP32S3        |
| -------- | -------------- |
| VCC      | 3.3V           |
| GND      | GND            |
| VRx      | GPIO1 (ADC1_0) |
| VRy      | GPIO2 (ADC1_1) |
| SW       | GPIO4          |

> [!IMPORTANT]
>
> *   ESP32S3 開發板的 GPIO 編號可能因板而異，請參考你的開發板的引腳圖。
> *   搖桿模組的 VRx 和 VRy 連接到支援 ADC 的 GPIO。

## 照片
![Wiring Diagram](https://imgur.com/8PnjpOG.jpg)
![Hardware Setup](https://imgur.com/C6VxlT2.jpg)

## test 資料夾使用方法
把 `test` 資料夾中的 `*.cpp` 內容複製到 `main.cpp` 檔案中並且編譯執行。
* `joystick_motor.cpp`: 使用搖桿控制步進馬達旋轉方向。

## 除錯

*   如果缺少.h檔案，請確保在`main/CMakeLists.txt`中`REQUIRES`包含了需要的東西。

