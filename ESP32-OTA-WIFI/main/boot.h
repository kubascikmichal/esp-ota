#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_flash_partitions.h"
#include "esp_ota_ops.h"

#define HASH_LEN 32

void get_sha256_of_partitions();