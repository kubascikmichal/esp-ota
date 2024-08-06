#include "ota.h"

static const char *TAG = "ota_update";
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

char http_rcv_buffer[500];

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            strncpy(http_rcv_buffer, (char *)evt->data, evt->data_len);
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

esp_err_t get_last_firmware_version(const char *version_url, double firmware_version, char *new_version_url)
{

    ESP_LOGI(TAG, "Fetch json version file : %s", version_url);

    esp_http_client_config_t config = {
        .url = version_url,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .cert_pem = (char *)server_cert_pem_start,
        .event_handler = _http_event_handler,
        .keep_alive_enable = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "unable to download the json file, aborting...");
        esp_http_client_cleanup(client);
        return err;
    }

    cJSON *json = cJSON_Parse(http_rcv_buffer);
    if (json == NULL)
    {
        ESP_LOGI(TAG, "downloaded file is not a valid json, aborting...");
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }

    cJSON *version = cJSON_GetObjectItemCaseSensitive(json, JSON_FORMAT_VERSION_ATTRIBUTE);
    cJSON *bin_url = cJSON_GetObjectItemCaseSensitive(json, JSON_FORMAT_BINLOCATION_ATTRIBUTE);
    if (!cJSON_IsNumber(version))
    {
        ESP_LOGI(TAG, "unable to read new version, aborting...");
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }

    double new_version = version->valuedouble;
    if (new_version > firmware_version)
    {
        ESP_LOGI(TAG, "current firmware version (%.1f) is lower than the available one (%.1f), upgrading...", firmware_version, new_version);
        if (cJSON_IsString(bin_url) && (bin_url->valuestring != NULL))
        {
            strcpy(new_version_url, bin_url->valuestring);
            ESP_LOGI(TAG, "the new file name %s", new_version_url);
            esp_http_client_cleanup(client);
            return ESP_OK;
        }
        else
        {
            ESP_LOGI(TAG, "unable to read the new file name, aborting...");
            esp_http_client_cleanup(client);
            return ESP_FAIL;
        }
        return ESP_FAIL;
    }
    else
    {
        ESP_LOGI(TAG, "current firmware version (%.1f) is greater or equal to the available one (%.1f), nothing to do...", firmware_version, new_version);
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }
}

void ota_update_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting OTA example task");

    ota_update_task_param_t *param = (ota_update_task_param_t *)pvParameter;

    char bin_url[200];
    esp_err_t err = get_last_firmware_version(param->url, param->firmware, bin_url);

    if (err == ESP_OK)
    {
        esp_http_client_config_t config = {
            .url = bin_url,
            .crt_bundle_attach = esp_crt_bundle_attach,
            .cert_pem = (char *)server_cert_pem_start,
            .event_handler = _http_event_handler,
            .keep_alive_enable = true,
        };

        esp_https_ota_config_t ota_config = {
            .http_config = &config,
        };

        ESP_LOGI(TAG, "Attempting to download update from %s", config.url);
        esp_err_t ret = esp_https_ota(&ota_config);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "OTA Succeed, Rebooting...");
            esp_restart();
        }
        else
        {
            ESP_LOGE(TAG, "Firmware upgrade failed");
        }
    }

    vTaskDelay(10000 / portTICK_PERIOD_MS);
}
