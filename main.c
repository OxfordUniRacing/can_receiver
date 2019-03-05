#include <atmel_start.h>

#include <hri_tc_c21.h>

#include <stdio.h>
#include <string.h>

static struct io_descriptor *uart;

static uint32_t get_time() {
	hri_tc_set_CTRLB_CMD_bf(TC0, TC_CTRLBCLR_CMD_READSYNC_Val);
	return hri_tccount32_read_COUNT_reg(TC0);
}

static void CAN_0_rx_callback(struct can_async_descriptor *const descr) {
	gpio_toggle_pin_level(LED0);

	char str[32];
	int len = snprintf(str, sizeof(str), "[%u] ", get_time());

	struct can_message msg;
	uint8_t data[8];
	msg.data = data;
	can_async_read(descr, &msg);

	io_write(uart, str, len);

	len = snprintf(str, sizeof(str), "id=%03X data=", msg.id);
	io_write(uart, str, len);

	for (int i = 0; i < msg.len; i++) {
		len = snprintf(str, sizeof(str), "%02X", msg.data[i]);
		io_write(uart, str, len);
	}
	io_write(uart, "\n", 1);
}

static void setup_can(void) {
	can_async_register_callback(&CAN_0, CAN_ASYNC_RX_CB, CAN_0_rx_callback);
	//can_async_register_callback(&CAN_0, CAN_ASYNC_TX_CB, CAN_0_tx_callback);
	can_async_enable(&CAN_0);

	struct can_filter filter;
	filter.id   = 0;
	filter.mask = 0;
	can_async_set_filter(&CAN_0, 0, CAN_FMT_STDID, &filter);
}

int main(void) {
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	hri_tc_set_CTRLA_ENABLE_bit(TC0);

	setup_can();

	usart_sync_get_io_descriptor(&UART_EDBG, &uart);
	usart_sync_enable(&UART_EDBG);

	const char *str = "Can Receiver\n";
	io_write(uart, str, strlen(str));

	while (1) {}
}
