/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#include <can_inf.h>

#include <mcp2515.h>
#include <queue.h>

#include <utils.h>
#include <can.h>

#define MSG_QUEUE_SIZE  128

void can__init(void)
{
	printf("CAN device started...\n");
	mq_init(MSG_QUEUE_SIZE);
}

int can_setup(int baudrate)
{
	mcp2515_reset();

	/* Wait until reset finishes. */
	int count = 100;
	while (1) {
		if (get_mode() == REQOP_CONFIG) {
			break;
		}
		else {
			udelay(1);
			count--;
		}
		if (count <= 0) {
			printf("CAN controller not responding. Check daughter board connection!\n");
			return -1;
		}
	}

	set_baudrate(baudrate);
	enable_intrrupt();

	set_mode(REQOP_NORMAL);

	return 0;
}

void can_send(struct can_frame frame)
{
	int ret;

	do {
		ret = tx_queue_push(&frame);
		start_xmit();
	} while (ret < 0);
}

void can_recv(struct can_frame *frame)
{
	while (rx_queue_pop(frame) == NULL);
}

int can_try_recv(struct can_frame *frame)
{
	if (rx_queue_pop(frame) == NULL) {
		return -1;
	};

	return 0;
}

int can_set_filter(struct can_id id, unsigned int mask)
{
	int ret;

	set_mode(REQOP_CONFIG);

	ret = set_rx_filter(id, mask);

	set_mode(REQOP_NORMAL);

	return ret;
}

void can_clear_filter(int filter_id)
{
	set_mode(REQOP_CONFIG);

	clear_rx_filter(filter_id);

	set_mode(REQOP_NORMAL);
}

void can_disable_filtering(void)
{
	set_mode(REQOP_CONFIG);

	clear_filter_mask(2);

	set_mode(REQOP_NORMAL);
}
