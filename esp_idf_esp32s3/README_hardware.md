## ESP32-S3 Firmware (esp_idf_esp32s3)

This section describes the firmware for the ESP32-S3, which handles motor control based on joystick input and provides a web interface for remote operation.

### Code Structure (`esp_idf_esp32s3/main`)

*   **`http.cpp/http.h`:** Implements the HTTP server logic for receiving control commands.
*   **`joystick.cpp/joystick.h`:** Handles reading and interpreting input from the joystick module.
*   **`main.cpp`:** Main program entry point. Initializes Wi-Fi, sets up the HTTP server, and coordinates motor control based on joystick input.
*   **`ULN2003.cpp/ULN2003.h`:** Provides functions to control the 28BYJ-48 stepper motor via the ULN2003 driver board.
*   **`wifi.cpp/wifi.h`:** Manages the Wi-Fi connection to your local network.
*   **`wifi_config.h`:** Contains Wi-Fi SSID and password. **You need to modify this file with your network credentials.**

### Hardware Setup

*   **ESP32-S3 Development Board:**
    *   Model: **Goouuu ESP32-S3 N16R8** (16MB flash/8MB PSRAM, dual Type-C USB, W2812 RGB).
    *   This board controls the stepper motor and handles HTTP requests for remote control.
    ![Image Source](https://www.taiwansensor.com.tw/wp-content/uploads/2023/12/O1CN01Zmz8W21kAXm0aySdE_672934643.jpg)
*   **28BYJ-48 Stepper Motor with ULN2003 Driver Board:**
    *   Used for panning the camera.
    ![Image Source](https://m.media-amazon.com/images/I/61Ml9ebd2kL._AC_UF894,1000_QL80_.jpg)
*   **Joystick Module:**
    *   Provides analog input for controlling the stepper motor's rotation.
    ![Image Source](https://down-br.img.susercontent.com/file/bcf905ecd9ff177fde7f4fbcafc9c400)
*   **Power Supply Module:**
    *   Provides a dedicated power supply for the stepper motor (requires a 7-12V DC power adapter).
    ![Image Source](https://http2.mlstatic.com/D_613161-MLA47599398871_092021-C.jpg)

### Installation

1. **Install the ESP-IDF Extension in VS Code:**
    *   Follow the instructions in the "ESP-IDF Extension for VS Code Overview" to set up the ESP-IDF development environment: [link](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)

2. **Obtain the Project Code:**
    ```bash
    git clone https://github.com/whiteSHADOW1234/PetCam.git
    cd esp_idf_esp32s3
    ```

3. **Configure Wi-Fi Credentials:**

    You have two options for configuring the Wi-Fi credentials:

    *   **Option 1: Create `main/wifi_config.h`:**
        1. Create a new file named `wifi_config.h` inside the `esp_idf_esp32s3/main` directory.
        2. Add the following content to `wifi_config.h`, replacing `"YOUR_WIFI_SSID"` and `"YOUR_WIFI_PASSWORD"` with your actual Wi-Fi SSID and password:

            ```c
            #ifndef WIFI_CONFIG_H
            #define WIFI_CONFIG_H

            #define EXAMPLE_ESP_WIFI_SSID "YOUR_WIFI_SSID"
            #define EXAMPLE_ESP_WIFI_PASS "YOUR_WIFI_PASSWORD"

            #endif
            ```

    *   **Option 2: Modify `main/wifi.cpp`:**
        1. Directly edit the `esp_idf_esp32s3/main/wifi.cpp` file.
        2. Comment out or delete the line: `#include "wifi_config.h"`.
        3. Modify the `EXAMPLE_ESP_WIFI_SSID` and `EXAMPLE_ESP_WIFI_PASS` definitions within `wifi.cpp` to your actual Wi-Fi credentials.

### Usage

1. **Set Espressif Device Target:**
    *   In VS Code, press `Ctrl+Shift+P` (or `Cmd+Shift+P` on macOS).
    *   Type `ESP-IDF: Set Espressif Device Target` and press Enter.
    *   Select `esp32s3` and then choose the first option, `ESP32-S3 chip (via builtin USB-JTAG)`.

2. **Select Serial Port:**
    *   In VS Code, press `Ctrl+Shift+P`.
    *   Type `ESP-IDF: Select Port to Use (COM, tty, usbserial)` and press Enter.
    *   Choose the serial port that corresponds to your ESP32-S3 development board.

3. **Build, Flash, and Monitor:**
    *   In VS Code, press `Ctrl+Shift+P`.
    *   Type `ESP-IDF: Build, Flash and Start a Monitor on your Device` and press Enter.
    *   This will build the firmware, flash it to your ESP32-S3, and open the serial monitor to view the output.

4. **Connect to Wi-Fi:**
    *   The ESP32-S3 will attempt to connect to the Wi-Fi network you configured.
    *   The serial monitor will display the ESP32-S3's assigned IP address.
    *   Use the `turn right`/`turn left` buttons in the `server-view` page to send control commands to the ESP32.

### Wiring

**28BYJ-48 Stepper Motor (with ULN2003 Driver):**

*   Connect the motor to the ULN2003 driver board (no special wiring needed on this connection).
*   Connect the ULN2003 driver board to the ESP32-S3 and power module as shown below:

| 28BYJ-48 Motor | ULN2003 Driver Board | ESP32-S3 / Power Module |
| :------------ | :----------------- | :---------------------- |
| Red           | +                  | 5V (Power Module)        |
| Orange        | IN1                | GPIO15                  |
| Yellow        | IN2                | GPIO16                  |
| Pink          | IN3                | GPIO17                  |
| Blue          | IN4                | GPIO18                  |
| Black         | -                  | GND (Power Module)       |

**Joystick Module:**

| Joystick Module | ESP32-S3      |
| :-------------- | :------------ |
| VCC             | 3.3V          |
| GND             | GND           |
| VRx             | GPIO1 (ADC1_0) |
| VRy             | GPIO2 (ADC1_1) |
| SW              | GPIO4         |


> [!IMPORTANT]  
>
> *   The specific GPIO numbers used on the ESP32-S3 might vary depending on your development board. **Always consult your board's pinout diagram to confirm the correct pins.**
> *   The joystick module's VRx and VRy pins should be connected to GPIOs that support ADC (Analog-to-Digital Conversion).

### Photos
![Wiring Diagram](https://imgur.com/8PnjpOG.jpg)
![Hardware Setup](https://imgur.com/C6VxlT2.jpg)

### `test` Folder Usage

The `esp_idf_esp32s3/test` folder contains example code for testing individual components:

*   **`joystick_motor.cpp`:** Demonstrates how to control the stepper motor using the joystick.

**To use a test file:**

1. Copy the contents of the desired `.cpp` file (e.g., `joystick_motor.cpp`) from the `test` folder.
2. Paste the contents into `esp_idf_esp32s3/main/main.cpp`, replacing the existing code.
3. Build, flash, and monitor the ESP32-S3 using the ESP-IDF extension in VS Code.

### Troubleshooting

*   **Missing `.h` files:** If you encounter errors about missing header files (e.g., `.h` files), make sure that the `REQUIRES` section in your `main/CMakeLists.txt` file includes all the necessary components. You might need to add components like `driver`, `esp_http_server`, etc., depending on the code you are using.

    ```cmake
    REQUIRES driver esp_http_server ...
    ```
