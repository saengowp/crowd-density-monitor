/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "mqtt_client.h"

#include "esp8266/spi_struct.h"
#include "esp8266/gpio_struct.h"
#include "driver/gpio.h"
#include "driver/spi.h"
#include "freertos/stream_buffer.h"

#define SPI_SLAVE_HANDSHARK_GPIO                4

#define SPI_WRITE_BUFFER_MAX_SIZE               2048
#define SPI_READ_BUFFER_MAX_SIZE                1024


/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/

#include "secret.c"

#define EXAMPLE_ESP_WIFI_SSID      WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      WIFI_PASS
#define EXAMPLE_ESP_MAXIMUM_RETRY  10

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "CWSYS MQTT CoModule";

static int s_retry_num = 0;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS
        },
    };

    /* Setting a password implies station will connect to all security modes including WEP/WPA.
        * However these modes are deprecated and not advisable to be used. Incase your Access point
        * doesn't support WPA2, these mode can be enabled by commenting below line */

    if (strlen((char *)wifi_config.sta.password)) {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler));
    vEventGroupDelete(s_wifi_event_group);
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
            ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://mqtt.netpie.io",
		.client_id = "5fb2f796-747b-429a-b0dd-614e78d3fc47",
		.username = "b49QQNtiuKDdAsMBMZspZFyBd1yMkeBP",
		.password = "do~7nc6ExOb(_e~f!UwX_((-4sQ6QbU(",
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

#define SPI_SLAVE_HANDSHARK_GPIO     4
#define SPI_SLAVE_HANDSHARK_SEL      (1ULL<<SPI_SLAVE_HANDSHARK_GPIO)

#define SPI_WRITE_BUFFER_MAX_SIZE               2048
#define SPI_READ_BUFFER_MAX_SIZE                1024

#define ESP_SPI_SLAVE_RECV    // Define the macro is master send mode, delete will be slave send modes

static StreamBufferHandle_t spi_slave_tx_ring_buf;
static StreamBufferHandle_t spi_slave_rx_ring_buf;
static uint32_t total_recv_len = 0;
static bool wait_recv_data = false;
static bool sending_flag = false;
static volatile uint32_t total_tx_count = 0;
static volatile uint32_t total_send_len = 0;

static void IRAM_ATTR spi_event_callback(int event, void* arg)
{
    int x;
    BaseType_t xHigherPriorityTaskWoken;
    uint32_t status;
    uint32_t trans_done;
    uint32_t data[16];
    spi_trans_t trans = {0};
    uint16_t cmd = 0;
    bool trigger_flag = false;

    switch (event) {
        case SPI_TRANS_DONE_EVENT: {
            gpio_set_level(SPI_SLAVE_HANDSHARK_GPIO, 0);
            trans_done = *(uint32_t*)arg;

            if (trans_done & SPI_SLV_RD_BUF_DONE) {   // slave -> master data
                if (total_send_len == 0) {
                    sending_flag = false;
                    total_send_len = xStreamBufferBytesAvailable(spi_slave_tx_ring_buf);

                    if (total_send_len > 0) {    // Have some data send to MCU
                        spi_slave_set_status(HSPI_HOST, (uint32_t*)&total_send_len);
                        sending_flag = true;
                        trigger_flag = true;
                    }
                } else {     // Have some data send to MCU
                    memset(&trans, 0x0, sizeof(trans));
                    trans.cmd = &cmd;
                    trans.addr = NULL;
                    trans.bits.val = 0;
                    trans.bits.cmd = 8 * 1;
                    trans.bits.addr = 8 * 1;
                    trans.bits.mosi = 0;
                    trans.miso = data;
                    trans.bits.miso = xStreamBufferReceiveFromISR(spi_slave_tx_ring_buf, data, 64, &xHigherPriorityTaskWoken);    // send max 32bytes

                    if (trans.bits.miso != 0) {
                        total_send_len -= trans.bits.miso;
                        trans.bits.miso <<= 3;
                        spi_trans(HSPI_HOST, &trans);
                        trigger_flag = true;;
                    }
                }
            }

            if (trans_done & SPI_SLV_WR_BUF_DONE) {      // master -> slave   data
                uint32_t len = total_recv_len;

                if (len > 64) {   // only send max 32bytes one time
                    len = 64;
                }

                if (len > 0) {
                    for (x = 0; x < 16; x++) {
                        data[x] = SPI1.data_buf[x];
                    }

                    xStreamBufferSendFromISR(spi_slave_rx_ring_buf, (void*) data, len, &xHigherPriorityTaskWoken);
                    total_recv_len -= len;
                }

                if (xStreamBufferSpacesAvailable(spi_slave_rx_ring_buf) >= 64) {    // Stream buffer not full, can be read agian
                    trigger_flag = true;
                } else {
                    wait_recv_data = true;
                }
            }

            if (trans_done & SPI_SLV_WR_STA_DONE) {        // master -> slave status len
                spi_slave_get_status(HSPI_HOST, &status);
                total_recv_len = status;
                uint32_t tx_size = xStreamBufferBytesAvailable(spi_slave_tx_ring_buf);

                if (total_recv_len > 0) {
                    trigger_flag = true;
                } else if (tx_size > 0) {                // SPI send done and ESP8266 send buffer have data
                    if (sending_flag == false) {
                        spi_slave_set_status(HSPI_HOST, &tx_size);
                    }

                    trigger_flag = true;
                }
            }

            if (trans_done & SPI_SLV_RD_STA_DONE) {      // Slave -> Master status len
                memset(&trans, 0x0, sizeof(trans));
                trans.cmd = &cmd;
                trans.addr = NULL;
                trans.bits.val = 0;
                trans.bits.cmd = 8 * 1;
                trans.bits.addr = 8 * 1;
                trans.bits.mosi = 0;
                trans.miso = data;
                trans.bits.miso = xStreamBufferReceiveFromISR(spi_slave_tx_ring_buf, data, 64, &xHigherPriorityTaskWoken);

                if (trans.bits.miso != 0) {
                    total_send_len -= trans.bits.miso;
                    trans.bits.miso <<= 3;
                    spi_trans(HSPI_HOST, &trans);
                    trigger_flag = true;
                }
            }

            if (trigger_flag) {
                gpio_set_level(SPI_SLAVE_HANDSHARK_GPIO, 1);
            }

            if (xHigherPriorityTaskWoken == pdTRUE) {
                taskYIELD();
            }
        }
        break;

        case SPI_DEINIT_EVENT: {

        }
        break;
    }

}

uint32_t read_count = 0;

static void IRAM_ATTR spi_slave_read_master_task(void* arg)
{
    uint8_t read_data[SPI_READ_BUFFER_MAX_SIZE + 1];
    size_t xReceivedBytes;

    while (1) {
        xReceivedBytes = xStreamBufferReceive(spi_slave_rx_ring_buf, read_data, SPI_READ_BUFFER_MAX_SIZE, 2000 / portTICK_RATE_MS);

        if (xReceivedBytes != 0) {
            for (int i = 0; i < xReceivedBytes; i++) {
                if (read_data[i] != 0x33) {
                    printf("receive error data: %x\n", read_data[i]);
                }
            }

            read_count += xReceivedBytes;
            memset(read_data, 0x0, xReceivedBytes);

            // steam buffer full
            if (wait_recv_data) {
                if (xStreamBufferBytesAvailable(spi_slave_rx_ring_buf) > 64) {
                    gpio_set_level(SPI_SLAVE_HANDSHARK_GPIO, 1);
                    wait_recv_data = false;
                }
            }
        }
    }
}

#include "time.h"
static void IRAM_ATTR spi_slave_write_master_task(void* arg)
{
    static uint8_t write_data[SPI_WRITE_BUFFER_MAX_SIZE];
    memset(write_data, 0x44, SPI_WRITE_BUFFER_MAX_SIZE);
    vTaskDelay(5000 / portTICK_RATE_MS);
    printf("Test send\r\n");
    time_t start = time(NULL);

    while (1) {
        total_tx_count += xStreamBufferSend(spi_slave_tx_ring_buf, write_data, SPI_WRITE_BUFFER_MAX_SIZE, portMAX_DELAY);
        portENTER_CRITICAL();

        if (sending_flag == false) {
            total_send_len = xStreamBufferBytesAvailable(spi_slave_tx_ring_buf);
            spi_slave_set_status(HSPI_HOST, (uint32_t*)&total_send_len);
            sending_flag = true;
            gpio_set_level(SPI_SLAVE_HANDSHARK_GPIO, 1);
        }

        portEXIT_CRITICAL();

        if (total_tx_count >= 20 * 1024 * 1024) {
            printf("tx done; %d bytes, time : %ld\r\n", total_tx_count, time(NULL) - start);

            for (;;) {
                vTaskDelay(100);
            }
        }

    }
}

void spi_start()
{
	spi_slave_tx_ring_buf = xStreamBufferCreate(4096, 1);
	spi_slave_rx_ring_buf = xStreamBufferCreate(4096, 1024);

	ESP_LOGI(TAG, "init gpio");
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = SPI_SLAVE_HANDSHARK_SEL;
	io_conf.pull_down_en = 0;
	io_conf.pull_up_en = 0;
	gpio_config(&io_conf);
	gpio_set_level(SPI_SLAVE_HANDSHARK_GPIO, 0);

	ESP_LOGI(TAG, "init spi");

	spi_config_t spi_config;
	// Load default interface parameters
	// CS_EN:1, MISO_EN:1, MOSI_EN:1, BYTE_TX_ORDER:1, BYTE_TX_ORDER:1, BIT_RX_ORDER:0, BIT_TX_ORDER:0, CPHA:0, CPOL:0
	spi_config.interface.val = SPI_DEFAULT_INTERFACE;

	// Load default interrupt enable
	// TRANS_DONE: false, WRITE_STATUS: true, READ_STATUS: true, WRITE_BUFFER: true, READ_BUFFER: ture
	spi_config.intr_enable.val = SPI_SLAVE_DEFAULT_INTR_ENABLE;
	// Set SPI to slave mode
	spi_config.mode = SPI_SLAVE_MODE;
	// Register SPI event callback function
	spi_config.event_cb = spi_event_callback;
	spi_init(HSPI_HOST, &spi_config);

	// create spi_slave_read_master_task
	xTaskCreate(spi_slave_read_master_task, "spi_slave_read_master_task", 2048, NULL, 6, NULL);
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
    mqtt_app_start();
    spi_start();
}
