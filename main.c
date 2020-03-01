#include <atmel_start.h>

#include <hri_tc_c21.h>

#include <stdio.h>
#include <string.h>

#define QUEUE_LENGTH 1000

static struct io_descriptor *uart;

// can log to store in an array
typedef struct canlog_queue{
	struct can_message msg;
	uint32_t t;
} can_log;

static can_log canlog_queue[QUEUE_LENGTH];

static uint32_t get_time() {
	hri_tc_set_CTRLB_CMD_bf(TC0, TC_CTRLBCLR_CMD_READSYNC_Val);
	return hri_tccount32_read_COUNT_reg(TC0);
}

static int qhead = 0; // start of the queue
static int qtail = 0; // end of the queue
static int qlength = 0; // length of the queue
static int qdataloss = 0; // count the number of messages lost

/*	when handling a lot of can bus messages, the can bus transceiver 
	buffer might be overwhelmed, and disable the can bus. 
	One way around it is to let interrupt store the messages only, 
	and print them in the main loop so that CAN_0_rx_callback will 
	cost less time and able to manage more messages.
	MODIFICATION:
	alter callback so it only handles the storage of can messages.
*/
static void CAN_0_rx_callback(struct can_async_descriptor *const descr) {
	gpio_toggle_pin_level(LED0);
	
	if(qlength < QUEUE_LENGTH){
		struct can_message msg;
		uint8_t data[8];
		msg.data = data;
		can_async_read(descr, &msg);
		
		// if qtail is at the end of the list, move it to 0 b/c this list is a loop
		if(qtail == QUEUE_LENGTH){
			qtail = 0;
		}
		canlog_queue[qtail].msg = msg;
		canlog_queue[qtail].t = get_time();
		// move qtail 1 place forward
		qtail ++;
		qlength ++;
	} else {
		qdataloss++;
	}
}

// print the can messages to serial port
static void printCan(void){
	if(qlength > 0){
		// get the messages and deal with the indices
		can_log log = canlog_queue[qhead];
		// move qhead a place forward
		qhead = (qhead + 1) % QUEUE_LENGTH;
		qlength--;
		
		// print the message
		char str[32];
		int len = snprintf(str, sizeof(str), "[%u] ", log.t);
		io_write(uart, str, len); // [time]
		
		len = snprintf(str, sizeof(str), "id=%03X data=", log.msg.id);
		io_write(uart, str, len); // id of the message
		
		for (int i = 0; i < log.msg.len; i++) {
			len = snprintf(str, sizeof(str), "%02X", log.msg.data[i]);
			io_write(uart, str, len); // data
		}
		io_write(uart, "\n", 1);
		// finish printing
		
		// report number of loss messages if any
		if(qdataloss > 0){
			len = snprintf(str, sizeof(str), "Lost %u messages\n", qdataloss);
			io_write(uart, str, len);
			qdataloss = 0;
		}
	}
}

static void setup_can(void) {
	can_async_register_callback(&CAN_0, CAN_ASYNC_RX_CB, CAN_0_rx_callback);
	//can_async_register_callback(&CAN_0, CAN_ASYNC_TX_CB, CAN_0_tx_callback);
	int32_t s = can_async_enable(&CAN_0);
		
	char str2[32];
	int len2 = snprintf(str2, sizeof(str2), "Enable Status: %u\n", s);
	io_write(uart, str2, len2);

	// as a receiver, it doesn't filter anything
	struct can_filter filter;
	filter.id   = 0;
	filter.mask = 0;
	can_async_set_filter(&CAN_0, 0, CAN_FMT_STDID, &filter);
}

int main(void) {
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	hri_tc_set_CTRLA_ENABLE_bit(TC0);

	usart_sync_get_io_descriptor(&UART_EDBG, &uart);
	usart_sync_enable(&UART_EDBG);

	setup_can();
	const char *str = "Can Receiver\n";
	io_write(uart, str, strlen(str));

	
	while (1) {
		printCan();
	}
}
