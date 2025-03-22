#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
// #include "pico/cyw43_driver.h"
#include "hardware/clocks.h"

// for wifi scan
#include <stdio.h>

static uint64_t time_boot_up;

// static uint64_t time_boot_up;
// static struct repeating_timer led_timer;
// static bool led_state = false;

static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    if (result) {
        printf("ssid: %-32s rssi: %4d channel: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
               result->ssid, result->rssi, result->channel,
               result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4],
               result->bssid[5],
               result->auth_mode);
    }
    return 0;
}

void blink_onboard_led(uint32_t blink_ms) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(blink_ms);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(blink_ms);
    printf("Blinked\n");
}

void print_system_info(uint32_t iteration_ct) {
    double timestamp = (time_us_64() - time_boot_up) / 1000000.0;
    double sys_clock_hz = clock_get_hz(clk_sys) / 1000000.0;
    printf("Iteration %d, Tiemstamp since boot: %.3fs, System Clock: %.3f Mhz\n", iteration_ct++, timestamp,
           sys_clock_hz);
}


int main() {
    // set_sys_clock_khz(266000, true);
    time_boot_up = time_us_64(); // get the time since boot up
    uint i = 0;

    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("Wifi init failed");
        return -1;
    }
    printf("Wifi cyw43 init passed");

    cyw43_arch_enable_sta_mode();
    printf("Wifi enable sta mode done");
    absolute_time_t scan_time = nil_time;
    bool scan_in_progress = false;
    while (true) {
        if (absolute_time_diff_us(get_absolute_time(), scan_time) < 0) {
            if (!scan_in_progress) {
                cyw43_wifi_scan_options_t scan_options = {0};
                int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
                if (err == 0) {
                    printf("\nPerforming Wifi Scan\n");
                    scan_in_progress = true;
                } else {
                    printf("Failed to start scan: %d\n", err);
                    scan_time = make_timeout_time_ms(10000);
                }
            } else if (!cyw43_wifi_scan_active(&cyw43_state)) {
                scan_time = make_timeout_time_ms(10000);
                scan_in_progress = false;
                blink_onboard_led(100);
                print_system_info(i++);
            }
        }
    }

}
