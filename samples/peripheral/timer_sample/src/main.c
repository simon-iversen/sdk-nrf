/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>

#define LOG_LEVEL LOG_LEVEL_INF
#include <logging/log.h>
LOG_MODULE_REGISTER(timer_sample);

volatile bool update_timer_expired=false;

static void update_timer_handler(struct k_timer *timer_id)
{
   LOG_INF("Timer expired in handler");
   update_timer_expired=true;

}

K_TIMER_DEFINE(update_timer, update_timer_handler, NULL);

//and inside the main function and I am starting the timer and checking the if the flag is set or not .

void main(){

  LOG_INF("Starting timer");
  k_timer_start(&update_timer, K_SECONDS(5),K_SECONDS(5));


  while(1){
    if(update_timer_expired){
      LOG_INF("Timer expired in main");
      update_timer_expired=false;
    }

  }
}