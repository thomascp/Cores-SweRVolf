/*
 * Copyright (c) 2019 Western Digital Corporation or its affiliates
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __RISCV32_SWERVOLF_SOC_H_
#define __RISCV32_SWERVOLF_SOC_H_

#include <soc_common.h>

/* Following defines are needed for LED support until there is
   devices trees are in place. LED controller is defined in soc.c */
#define LED0_GPIO_CONTROLLER "LED0"
#define LED0_GPIO_PIN 0

#define RISCV_MTIME_BASE    0x80001020
#define RISCV_MTIMECMP_BASE 0x80001028

/* Timer configuration */
#define SERV_TIMER_BASE             0x80001018
#define SERV_TIMER_IRQ              7

/* lib-c hooks required RAM defined variables */
#define RISCV_RAM_BASE              CONFIG_RISCV_RAM_BASE_ADDR
#define RISCV_RAM_SIZE              CONFIG_RISCV_RAM_SIZE

#endif /* __RISCV32_SERV_SOC_H_ */
