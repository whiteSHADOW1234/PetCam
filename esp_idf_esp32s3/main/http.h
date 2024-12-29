#pragma once

#include "esp_http_server.h"

httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);

// 宣告馬達控制函數
void rotate_clockwise();
void rotate_counterclockwise();