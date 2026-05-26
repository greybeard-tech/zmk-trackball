#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <zmk/input.h>

#define REG_POWER_UP_RESET 0x3A
#define REG_DELTA_X_L 0x03
#define REG_DELTA_Y_L 0x04

static const struct device *spi_dev;

static void spi_init(void) {
    spi_dev = DEVICE_DT_GET(DT_NODELABEL(spi1));
}

static void write_reg(uint8_t reg, uint8_t val) {
    uint8_t tx[2] = {reg | 0x80, val};

    struct spi_buf buf = { .buf = tx, .len = 2 };
    struct spi_buf_set tx_set = { .buffers = &buf, .count = 1 };

    spi_write(spi_dev, &(struct spi_config){
        .frequency = 2000000,
        .operation = SPI_WORD_SET(8),
    }, &tx_set);
}

static uint8_t read_reg(uint8_t reg) {
    uint8_t tx = reg & 0x7F;
    uint8_t rx;

    struct spi_buf tx_buf = { .buf = &tx, .len = 1 };
    struct spi_buf rx_buf = { .buf = &rx, .len = 1 };

    struct spi_buf_set tx_set = { .buffers = &tx_buf, .count = 1 };
    struct spi_buf_set rx_set = { .buffers = &rx_buf, .count = 1 };

    spi_transceive(spi_dev,
        &(struct spi_config){
            .frequency = 2000000,
            .operation = SPI_WORD_SET(8),
        },
        &tx_set,
        &rx_set
    );

    return rx;
}

void main(void) {
    spi_init();

    write_reg(REG_POWER_UP_RESET, 0x5A);
    k_msleep(50);

    while (1) {
        int8_t dx = read_reg(REG_DELTA_X_L);
        int8_t dy = read_reg(REG_DELTA_Y_L);

        if (dx || dy) {
            zmk_input_report_rel(NULL, INPUT_REL_X, dx, false, K_NO_WAIT);
            zmk_input_report_rel(NULL, INPUT_REL_Y, dy, false, K_NO_WAIT);
        }

        k_msleep(5);
    }
}
