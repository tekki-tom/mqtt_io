#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/cyw43_arch.h"

#define I2C_SDA 4
#define I2C_SCL 5
const uint LED_PIN = 14;
void error()
{
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    gpio_put(LED_PIN, 0);
    while (true)
        ;
}

uint8_t sendi2c(uint8_t io_expander, uint8_t mcp_register, uint8_t mcp_value)
{
    uint8_t src[2];
    uint8_t result = -1;
    src[0] = mcp_register; // 0x15;
    src[1] = mcp_value;    // 0x01;
    result = i2c_write_blocking(i2c_default, io_expander, src, 2, false);
    printf("i2c %x.%x: %x result: %x\n", io_expander, mcp_register, mcp_value, result);
    return result;
}

int main()
{

    const int io_expander0 = 0x20;
    const int io_expander1 = 0x21;
    const int io_expander2 = 0x22;
    bool wl_ledStage = false;
    bool i2c_ledStage = false;

    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    i2c_init(i2c_default, 48 * 1000); // 48000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    printf("hello blink");
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed");
        return -1;
    }
    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)

    sleep_ms(500);
    // src[0] = 0x01;
    // src[1] = 0x00;
    // result = i2c_write_blocking(i2c_default, io_expander2, src, 2, false);
    // src[0] = 0x01;
    // src[1] = 0x00;
    // result = i2c_write_blocking(i2c_default, io_expander0, src, 2, false);
    // sendi2c(io_expander0, 0x00, 0x00);
    // sendi2c(io_expander0, 0x01, 0x00);

    sendi2c(io_expander1, 0x00, 0x00);
    sendi2c(io_expander1, 0x01, 0x00);

    // sendi2c(io_expander2, 0x00, 0x00);
    // sendi2c(io_expander2, 0x01, 0x00);

    printf("Hello, world!");
    // printf("i2c result: %d\n", result);
    // if (2 != result)
    //     error();

    while (true)
    {
        for (int i = 0x01; i < 0x08; i *= 2)
        {
            sendi2c(io_expander1, 0x14, i); // Port a
            sendi2c(io_expander1, 0x15, i); // Port b
            if (i2c_ledStage)
                i2c_ledStage = false;
            else
                i2c_ledStage = true;
            gpio_put(LED_PIN, i2c_ledStage);
            sleep_ms(1000);
        }

        sendi2c(io_expander1, 0x14, 0xff); // Port a
        sendi2c(io_expander1, 0x15, 0xff); // Port b
        sleep_ms(30000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, wl_ledStage);
        if (wl_ledStage)
            wl_ledStage = false;
        else
            wl_ledStage = true;
        // sleep_ms(1000);
    }

    // src[0] = 0x15;
    // src[1] = 0x00;
    // result = i2c_write_blocking(i2c_default, io_expander2, src, 2, false);
    // printf("i2c 0.1.0 OFF result: %d\n", result);
    // src[0] = 0x15;
    // src[1] = 0x01;
    // result = i2c_write_blocking(i2c_default, io_expander0, src, 2, false);
    // printf("i2c 2.1.0 ON  result: %d\n", result);
    // // if (2 != result)
    // //     error();
    // gpio_put(LED_PIN, 0);
    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    // sleep_ms(1000);
    // printf("blink\n");

    return 0;
}
