#include "http.h"
#include "esp_log.h"

static const char *TAG = "http_server";

// 宣告馬達控制函數 (讓 http.cpp 知道這些函數的存在)
extern void rotate_clockwise();
extern void rotate_counterclockwise();

/* An HTTP POST handler */
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
        httpd_register_uri_handler(server, &post);
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