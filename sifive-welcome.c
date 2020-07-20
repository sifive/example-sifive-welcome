/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdbool.h>
#include <stdio.h>
#include <metal/cpu.h>
#include <metal/led.h>
#include <metal/button.h>
#include <metal/switch.h>

#define RTC_FREQ    32768

volatile bool timer_isr_flag = 0;

// This demo will toggle LEDs colors so we define them here
#ifdef metal_led_ld0red
struct metal_led led0_red = metal_led_ld0red;
#else
struct metal_led led0_red = metal_led_none;
#endif
#ifdef metal_led_ld0blue
struct metal_led led0_blue = metal_led_ld0blue;
#else
struct metal_led led0_blue = metal_led_none;
#endif
#ifdef metal_led_ld0green
struct metal_led led0_green = metal_led_ld0green;
#else
struct metal_led led0_green = metal_led_none;
#endif

void display_banner (void) {

    printf("\n");
    printf("\n");
    printf("                  SIFIVE, INC.\n");
    printf("\n");
    printf("           5555555555555555555555555\n");
    printf("          5555                   5555\n");
    printf("         5555                     5555\n");
    printf("        5555                       5555\n");
    printf("       5555       5555555555555555555555\n");
    printf("      5555       555555555555555555555555\n");
    printf("     5555                             5555\n");
    printf("    5555                               5555\n");
    printf("   5555                                 5555\n");
    printf("  5555555555555555555555555555          55555\n");
    printf("   55555           555555555           55555\n");
    printf("     55555           55555           55555\n");
    printf("       55555           5           55555\n");
    printf("         55555                   55555\n");
    printf("           55555               55555\n");
    printf("             55555           55555\n");
    printf("               55555       55555\n");
    printf("                 55555   55555\n");
    printf("                   555555555\n");
    printf("                     55555\n");
    printf("                       5\n");
    printf("\n");

    printf("\n");
    printf("               Welcome to SiFive!\n");

}

void metal_riscv_cpu_intc_mtip_handler() {
    struct metal_cpu cpu = metal_cpu_get(metal_cpu_get_current_hartid());

    // Disable Timer interrupt
    metal_cpu_disable_timer_interrupt(cpu);

    // Flag showing we hit timer isr
    timer_isr_flag = 1;
}

void wait_for_timer(struct metal_led led) {
    struct metal_cpu cpu = metal_cpu_get(metal_cpu_get_current_hartid());

    // clear global timer isr flag
    timer_isr_flag = 0;

    // Turn on desired LED
    metal_led_on(led);

    // Set timer
    metal_cpu_set_mtimecmp(cpu, metal_cpu_get_mtime(cpu) + RTC_FREQ);

    // Enable Timer interrupt
    metal_cpu_enable_timer_interrupt(cpu);

    // wait till timer triggers and isr is hit
    while (timer_isr_flag == 0){};

    timer_isr_flag = 0;

    // Turn off this LED
    metal_led_off(led);
}

int main (void)
{
    // Enable each LED
    metal_led_enable(led0_red);
    metal_led_enable(led0_green);
    metal_led_enable(led0_blue);

    // All Off
    metal_led_off(led0_red);
    metal_led_off(led0_green);
    metal_led_off(led0_blue);

    // Lets get the CPU and and its interrupt
    struct metal_cpu cpu = metal_cpu_get(metal_cpu_get_current_hartid());
    metal_cpu_enable_interrupts(cpu);

    // display welcome banner
    display_banner();

    // Red -> Green -> Blue, repeat
    while (1) {

        // Turn on RED
        wait_for_timer(led0_red);

        // Turn on Green
        wait_for_timer(led0_green);

        // Turn on Blue
        wait_for_timer(led0_blue);
    }

    // return
    return 0;
}
