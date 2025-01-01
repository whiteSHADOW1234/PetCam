# PetCam：基於 WebRTC 技術的即時寵物攝影機（可搭配硬體實現遠端轉動）
[**English README**](README.md) | [**ESP32-S3 Firmware Guide**](esp_idf_esp32s3/README_hardware.md) | [**ESP32-S3 設定指南**](esp_idf_esp32s3/README_hardware_zh_tw.md)

此專案實作了一個即時寵物攝影機應用程式，具有以下功能：

*   **Webcam 串流：** 使用 WebRTC 技術，從指定的裝置（例如筆記型電腦或智慧型手機）擷取並串流即時影像。
*   **遠端觀看：** 允許您透過區域網路上的任何裝置，使用網頁瀏覽器觀看即時影像串流。
*   **遠端平移控制：** 可遠端控制一個平移 (pan) 機制（使用 ESP32-S3、步進馬達和搖桿），以改變攝影機的視角。
*   **與寵物對話：** 觀看者可以透過 `server-view` 網頁將聲音傳送到 `open-cam` 裝置，實現與寵物的互動。
*   **HTTPS：** 使用 HTTPS 進行安全通訊。
*   **閒置智慧型手機再利用：** 此專案提供了一種實用的方法來重新利用許多家庭中閒置的智慧型手機，將其轉變為遠端網路攝影機，從而減少電子垃圾並提供額外價值。

## 專案結構

```
pet-camera-project/
├── client/                     # 客戶端程式碼 (網頁應用程式)
│   └── open-cam/               # 用於架設攝影機的網頁應用程式
│       ├── index.html          # open-cam 頁面 HTML
│       ├── script.js           # 處理攝影機存取、WebRTC 和串流的 JavaScript
│       └── style.css           # open-cam 頁面樣式
│   └── server-view/
│       └── script.js           # 處理 WebRTC 接收邏輯和馬達控制的 JavaScript
├── esp_idf_esp32s3/            # ESP32-S3 韌體 (馬達控制)
│   ├── .devcontainer/          # 開發容器設定 (用於開發)
│   ├── test/                   # 個別元件的測試檔案
│   └── main/                   # ESP32-S3 的主要原始碼
│       ├── CMakeLists.txt      # CMake 建置設定
│       ├── http.cpp/http.h     # HTTP 伺服器邏輯
│       ├── joystick.cpp/joystick.h # 搖桿輸入處理
│       ├── main.cpp            # 主要程式進入點
│       ├── wifi.cpp/wifi.h     # Wi-Fi 連線管理
│       ├── wifi_config.h       # Wi-Fi 認證 (請設定此項!)
│       └── ULN2003.cpp/ULN2003.h # 28BYJ-48 步進馬達控制
└── server/                     # 伺服器端程式碼 (Node.js)
    ├── server.js               # 帶有 WebRTC 訊號的 Node.js 伺服器
    ├── key.pem                 # SSL 私鑰 (用於測試的自簽章憑證)
    └── cert.pem                # SSL 憑證 (用於測試的自簽章憑證)
    └── templates/
        ├── index.html          # 伺服器登陸頁面
        └── server-view.html   # 觀看串流和控制攝影機的網頁
```

## 硬體

*   **Webcam 裝置：** 具有內建或外接網路攝影機的筆記型電腦、電腦或**閒置的智慧型手機**。
*   **伺服器：** 一台電腦 (可以與 webcam 裝置相同，以便進行測試) 來執行 Node.js 伺服器。
*   **ESP32-S3 開發板：**
    *   型號：**Goouuu ESP32-S3 N16R8** (16MB flash/8MB PSRAM, 雙 Type-C USB, W2812 RGB)。
    *   用於控制步進馬達和處理馬達控制的 HTTP 請求。
    *   ![圖片來源](https://www.taiwansensor.com.tw/wp-content/uploads/2023/12/O1CN01Zmz8W21kAXm0aySdE_672934643.jpg)
*   **28BYJ-48 步進馬達含 ULN2003 驅動板：**
    *   用於平移攝影機。
    *   ![圖片來源](https://m.media-amazon.com/images/I/61Ml9ebd2kL._AC_UF894,1000_QL80_.jpg)
*   **搖桿模組：**
    *   用於控制步進馬達的方向。
    *   ![圖片來源](https://down-br.img.susercontent.com/file/bcf905ecd9ff177fde7f4fbcafc9c400)
*   **電源模組：**
    *   步進馬達的專用電源 (需要 7-12V 直流電)。
    *   ![圖片來源](https://http2.mlstatic.com/D_613161-MLA47599398871_092021-C.jpg)

## 軟體先決條件

*   **Node.js 和 npm:** 從 [link](https://nodejs.org/) 安裝 Node.js 和 npm。
*   **Python 3:** 確保已安裝 Python 3 (可在終端機中執行 `python --version` 或 `python3 --version` 進行確認)。
*   **OpenSSL:** 您將需要 OpenSSL 來產生自簽章憑證以用於 HTTPS。它通常預先安裝在 Linux 和 macOS 上。對於 Windows，它通常與 Git for Windows 一起安裝，或者您可以單獨安裝它。使用 `openssl version` 驗證安裝。
*   **VS Code 與 ESP-IDF 擴充套件：** 按照此處的說明安裝適用於 VS Code 的 ESP-IDF 擴充套件：[link](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)
*   **Git:** 安裝 Git 進行版本控制: [link](https://git-scm.com/)
*   **網頁瀏覽器：** 支援 WebRTC 的現代網頁瀏覽器 (例如 Brave, Firefox 或 Edge, **但不是 Chrome**).

## 安裝和設定

**1. 取得專案：**

```bash
git clone https://github.com/whiteSHADOW1234/PetCam.git
cd PetCam
```

**2. 伺服器設定：**

*   **安裝 Node.js 依賴套件：**
    ```bash
    npm install # 在 PetCam 資料夾下執行此命令
    ```

*   **產生自簽章憑證：**
    *   在 `server` 目錄中執行以下指令，**將 `192.168.1.10` 替換為您伺服器的實際 LAN IP 位址**：

        ```bash
        openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -sha256 -days 365 -nodes -subj "/C=US/ST=CA/L=San Francisco/O=MyOrg/CN=192.168.1.10" -addext "subjectAltName=IP:192.168.1.10,DNS:localhost,IP:127.0.0.1" -addext "extendedKeyUsage=serverAuth"
        ```

**3. ESP32-S3 設定 (馬達控制)：**

*   **設定 Wi-Fi：**
    *   **選項 1：`wifi_config.h`：**
        *   在 `esp_idf_esp32s3/main` 目錄中建立名為 `wifi_config.h` 的檔案。
        *   新增以下內容，將預留位置替換為您實際的 Wi-Fi 認證：

            ```c
            #ifndef WIFI_CONFIG_H
            #define WIFI_CONFIG_H

            #define EXAMPLE_ESP_WIFI_SSID "您的 Wi-Fi SSID"
            #define EXAMPLE_ESP_WIFI_PASS "您的 Wi-Fi 密碼"

            #endif
            ```

    *   **選項 2：修改 `wifi.cpp`：**
        *   直接編輯 `esp_idf_esp32s3/main/wifi.cpp` 檔案。
        *   註解掉或刪除 `#include "wifi_config.h"` 這一行。
        *   將 `wifi.cpp` 中的 `EXAMPLE_ESP_WIFI_SSID` 和 `EXAMPLE_ESP_WIFI_PASS` 常數設定為您的 Wi-Fi 認證。

*   **建置和燒錄韌體：**
    1. **設定 Espressif 裝置目標：**
        *   在 VS Code 中，按下 `Ctrl+Shift+P` (或 macOS 上的 `Cmd+Shift+P`)。
        *   輸入 `ESP-IDF: Set Espressif Device Target` 並按下 Enter。
        *   選擇 `esp32s3` 並選擇第一個選項：`ESP32-S3 chip (via builtin USB-JTAG)`。
    2. **設定序列埠：**
        *   按下 `Ctrl+Shift+P` (或 `Cmd+Shift+P`)。
        *   輸入 `ESP-IDF: Select Port to Use` 並按下 Enter。
        *   選擇與您的 ESP32-S3 開發板相對應的序列埠。
    3. **建置、燒錄和監控：**
        *   按下 `Ctrl+Shift+P` (或 `Cmd+Shift+P`)。
        *   輸入 `ESP-IDF: Build, Flash and Start a Monitor on your Device` 並按下 Enter。
        *   這將建置韌體、將其燒錄到 ESP32-S3，並開啟序列埠監控器以檢視輸出。

**4. 接線：**

*   **28BYJ-48 步進馬達 (使用 ULN2003 驅動器)：**

    | 28BYJ-48 | ULN2003 | ESP32-S3/電源模組 |
    | -------- | ------- | ---------------------- |
    | 紅色     | +       | 5V (電源模組)      |
    | 橙色     | IN1     | GPIO15                |
    | 黃色     | IN2     | GPIO16                |
    | 粉色     | IN3     | GPIO17                |
    | 藍色     | IN4     | GPIO18                |
    | 黑色     | -       | GND (電源模組)     |

*   **搖桿模組：**

    | 搖桿模組 | ESP32-S3 |
    | -------- | -------- |
    | VCC      | 3.3V     |
    | GND      | GND      |
    | VRx      | GPIO1    |
    | VRy      | GPIO2    |
    | SW       | GPIO4    |

**注意：**

*   ESP32-S3 開發板的特定 GPIO 編號可能會有所不同。請參閱您開發板的引腳圖。
*   確保搖桿的 VRx 和 VRy 引腳連接到支援 ADC (類比數位轉換器) 的 GPIO 引腳。

## 執行應用程式

1. **啟動伺服器：**

    ```bash
    node start # 在 PetCam 資料夾下執行此命令
    ```

    伺服器主控台將輸出可存取的 URL，包括 `open-cam` 和 `server-view` 的 LAN IP 位址。

2. **開啟 Open-Cam 網站 (Webcam 用戶端)：**

    *   在配備網路攝影機的裝置 (筆記型電腦、電腦或智慧型手機) 上開啟網頁瀏覽器。
    *   前往 `https://<server_lan_ip>:5000/client/open-cam`，將 `<server_lan_ip>` 替換為伺服器主控台輸出中列出的其中一個 IP 位址。
    *   出現提示時，請授予網站存取您的攝影機和麥克風的權限。

3. **開啟 Server-View 網站：**

    *   在您區域網路上的任何裝置開啟另一個網頁瀏覽器。
    *   前往 `https://<server_lan_ip>:5000/server-view`，將 `<server_lan_ip>` 替換為伺服器的 LAN IP 位址。
    *   您應該會看到來自 `open-cam` 裝置的影像串流。

4. **控制馬達：**
    *   使用連接到 ESP32-S3 的搖桿來控制步進馬達的方向。
    *   點擊 `server-view` 頁面上的「Talk to Pet」按鈕，即可將音訊從觀看者傳輸到 `open-cam` 裝置。

## 疑難排解

*   **Open-Cam 上攝影機未啟動：**
    *   檢查瀏覽器和作業系統的攝影機和麥克風存取權限。
    *   確認 `client/open-cam/script.js` 中的 `serverAddress` 是否正確。
    *   查看瀏覽器的開發人員主控台 (F12) 中是否有錯誤。
    *   確保沒有其他應用程式正在使用攝影機。
    *   嘗試使用不同的瀏覽器或裝置。

*   **Server-View 上沒有影像：**
    *   檢查伺服器、`open-cam` 和 `server-view` 的主控台記錄檔中是否有錯誤。
    *   使用瀏覽器開發人員工具中的「網路」頁籤檢查 `open-cam` 和 `server-view` 之間是否正在交換 `offer`、`answer` 和 `ICE` 候選訊息 (WebRTC)。
    *   確認 `server-view` 上的 `RTCPeerConnection` 已建立，並且 `ontrack` 事件正在觸發。

*   **馬達沒有反應：**
    *   檢查 ESP32-S3、馬達驅動器和搖桿之間的接線。
    *   確認 ESP32-S3 已連接到您的 Wi-Fi 網路。
    *   使用 VS Code 中的序列埠監控器查看 ESP32-S3 是否正在接收指令。

*   **伺服器無法存取：**
    *   仔細檢查您伺服器的 LAN IP 位址。
    *   確保伺服器正在執行並監聽連接埠 5000。
    *   檢查您的防火牆設定 (允許連接埠 5000)。

*   **SSL 錯誤：**
    *   如果您看到 SSL 錯誤，這是因為您正在使用自簽章憑證。在開發期間，您需要在瀏覽器中新增一個臨時例外以繼續。**在生產環境中，請使用來自受信任憑證授權單位 (CA) 的有效憑證。**

*   **重新整理：** 如果您對程式碼進行了變更，請重新啟動伺服器並強制重新整理 (Ctrl+Shift+R 或 Cmd+Shift+R) 網頁瀏覽器中的網頁。

## `esp_idf_esp32s3/test` 資料夾用法

`test` 資料夾包含用於測試個別元件的程式碼：

*   **`joystick_motor.cpp`:** 測試使用搖桿控制步進馬達。

**要使用測試檔案：**

1. 將 `test` 資料夾中 `.cpp` 檔案的內容複製到 `esp_idf_esp32s3/main/main.cpp`。
2. 使用 VS Code 中的 ESP-IDF 擴充套件建置、燒錄並監控 ESP32-S3。

## 重要注意事項

*   **閒置智慧型手機再利用：** 此專案提供了一個很好的方式來利用舊的或閒置的智慧型手機作為網路攝影機，從而減少資源浪費並賦予它們新的用途。
*   **HTTPS:** 大多數瀏覽器都需要使用 HTTPS 才能存取 `getUserMedia`。自簽章憑證僅適用於開發用途。
*   **效能：** 此專案使用 WebRTC 進行更高效的影像串流。
*   **音訊：** 此專案中的音訊是從 `server-view` 到 `open-cam` 的單向傳輸。實作雙向音訊需要進一步開發。
*   **延展性：** 對於大量的客戶端，您將需要一個更具延展性的解決方案，可能需要使用媒體伺服器。
*   **安全性：** 對於生產環境，請使用來自受信任憑證授權單位的有效 SSL 憑證，並實施適當的安全措施。
