#ifdef UNIT_TEST
#include "mbed.h"
#include "rtos.h"
#include <unity.h>


Serial pcc(SERIAL_TX, SERIAL_RX); // tx, rx

extern "C" void HardFault_Handler() {
	pcc.printf("Hard Fault!\n");
	while (true) continue;
}
extern "C" void NMI_Handler() {
	pcc.printf("NMI Fault!\n");
	while (true) continue;
}
extern "C" void MemManage_Handler() {
	pcc.printf("MEM Manager Fault!\n");
	while (true) continue;
}
extern "C" void BusFault_Handler() {
	pcc.printf("Bus Fault Fault!\n");
	while (true) continue;
}
extern "C" void UsageFault_Handler() {
	pcc.printf("Usage Fault!\n");
	while (true) continue;
}


/* Mail */
typedef struct {
  float    voltage; /* AD result of measured voltage */
  float    current; /* AD result of measured current */
  uint32_t counter; /* A counter value               */
} mail_t;

Mail<mail_t, 16> mail_box;

void send_thread (void) {
    uint32_t i = 0;
    while (true) {
        i++; // fake data update
        mail_t *mail = mail_box.alloc();
        mail->voltage = (i * 0.1) * 33;
        mail->current = (i * 0.1) * 11;
        mail->counter = i;
        mail_box.put(mail);
        Thread::wait(1000);
    }
}


void setUp(void) {

}

void tearDown(void) {
}

void test_add_flowmail(void) {
	Thread thread;
	thread.start(callback(send_thread));

	while (true) {
			osEvent evt = mail_box.get();
			if (evt.status == osEventMail) {
					mail_t *mail = (mail_t*)evt.value.p;
					pcc.printf("\nVoltage: %.2f V\n\r"   , mail->voltage);
					pcc.printf("Current: %.2f A\n\r"     , mail->current);
					pcc.printf("Number of cycles: %u\n\r", mail->counter);

					mail_box.free(mail);
					TEST_ASSERT(true);
			}
	}
}

int main() {
	wait(2.0f);
	pcc.baud(115200);
	pcc.printf("hello");

	UNITY_BEGIN();
	RUN_TEST(test_add_flowmail);
	UNITY_END();
}
#endif
