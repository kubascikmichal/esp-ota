#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_crt_bundle.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "cJSON.h"

#define OTA_URL_SIZE 256

#define JSON_FORMAT_VERSION_ATTRIBUTE "last_version"
#define JSON_FORMAT_BINLOCATION_ATTRIBUTE "bin_url"

typedef struct ota_update_task_param_t {
    const char *url;     /*!< HTTP URL, the information on the URL*/
    double firmware;    /*!< Version of the current firmware*/
} ota_update_task_param_t;


void ota_update_task(void *pvParameter);
