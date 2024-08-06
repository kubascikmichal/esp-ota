#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "string.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "wifi.h"
#include "ota.h"
#include "boot.h"

#define FIRMWARE_VERSION 1.0

static const char *TAG = "ota_example";

void app_main(void)
{
    ESP_LOGI(TAG, "OTA example app_main start");
    // Initialize NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    get_sha256_of_partitions();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(connect_to_wifi());

    ota_update_task_param_t ota_param = {
        .url = CONFIG_FIRMWARE_UPGRADE_URL,
        .firmware = FIRMWARE_VERSION,
    };
    xTaskCreate(&ota_update_task, "ota_update_task", 8192, (void *)&ota_param, 5, NULL);
}
