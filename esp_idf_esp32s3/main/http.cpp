#include "http.h"
#include "esp_log.h"

static const char *TAG = "http_server";

// 宣告馬達控制函數 (讓 http.cpp 知道這些函數的存在)
extern void rotate_clockwise();
extern void rotate_counterclockwise();

/*  HTTP GET 處理器 - 用於測試與 ESP32 的連線
 *  當 Node.js 伺服器發送 GET 請求到 /test 路徑時，此函數會被調用。
 *  它會返回一個表示連線成功的訊息。
 */
static esp_err_t http_test_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Connection", "close");
    const char resp[] = "Connection test successful!";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* HTTP POST 處理器 - 用於接收來自 Node.js 伺服器的控制指令
 * 當 Node.js 伺服器發送 POST 請求到 /message 路徑時，此函數會被調用。
 * 它會解析請求內容，並根據內容控制步進電機。
 */
static esp_err_t http_post_handler(httpd_req_t *req)
{
    char content[100];
    size_t recv_size = sizeof(content);

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {  /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry sending this response */
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    // 打印接收到的訊息
    content[ret] = '\0'; // 確保字串正確結尾
    ESP_LOGI(TAG, "Received message: %s", content);

    // 根據接收到的訊息控制步進電機
    if (strcmp(content, "clockwise") == 0) {
        rotate_clockwise();
    } else if (strcmp(content, "counterclockwise") == 0) {
        rotate_counterclockwise();
    }

    // 傳送回應
    const char resp[] = "Message received";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// 定義 /test 路徑的 URI 處理器
static const httpd_uri_t test = {
    .uri       = "/test",
    .method    = HTTP_GET, // 使用 HTTP GET 方法
    .handler   = http_test_handler, // 指定處理函數為 http_test_handler
    .user_ctx  = NULL
};

// 定義 /message 路徑的 URI 處理器
static const httpd_uri_t post = {
    .uri       = "/message",
    .method    = HTTP_POST,
    .handler   = http_post_handler,
    .user_ctx  = NULL
};

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // 啟動 httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // 設定 URI 處理器
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &post); // 註冊 /message 的處理器
        httpd_register_uri_handler(server, &test); // 註冊 /test 的處理器
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // 停止 httpd server
    if (server) {
        httpd_stop(server);
    }
}