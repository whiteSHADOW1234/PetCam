idf_component_register(SRCS "main.cpp" "ULN2003.cpp" "joystick.cpp" "wifi.cpp" "http.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp_wifi esp_event nvs_flash esp_adc esp_http_server)