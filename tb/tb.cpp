// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Western Digital Corporation or its affiliates.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//********************************************************************************
// $Id$
//
// Function: Verilator testbench for SweRVolf
// Comments:
//
//********************************************************************************

#include <stdint.h>
#include <signal.h>

#include "verilated_vcd_c.h"
#include "Vswervolf_core_tb.h"

using namespace std;

static bool done;

vluint64_t main_time = 0;       // Current simulation time
// This is a 64-bit integer to reduce wrap over issues and
// allow modulus.  You can also use a double, if you wish.

double sc_time_stamp () {       // Called by $time in Verilog
  return main_time;           // converts to double, to match
  // what SystemC does
}

void INThandler(int signal)
{
	printf("\nCaught ctrl-c\n");
	done = true;
}

typedef struct {
  uint8_t state;
  char ch;
  uint32_t baud_t;
  vluint64_t last_update;
} uart_context_t;

void uart_init(uart_context_t *context, uint32_t clk_freq, uint32_t baud_rate) {
  context->baud_t = clk_freq/baud_rate;
}

void do_uart(uart_context_t *context, bool rx) {
  if (context->state == 0) {
    if (!rx) {
      context->last_update = main_time + context->baud_t/2;
      context->state++;
    }
  }
  else if(context->state == 1) {
    if (main_time > context->last_update) {
      context->last_update += context->baud_t;
      context->ch = 0;
      context->state++;
    }
  }
  else if (context->state < 10) {
    if (main_time > context->last_update) {
      context->last_update += context->baud_t;
      context->ch |= rx << (context->state-2);
      context->state++;
    }
  }
  else {
    if (main_time > context->last_update) {
      context->last_update += context->baud_t;
      putchar(context->ch);
      context->state=0;
    }
  }
}

int main(int argc, char **argv, char **env)
{
  Verilated::commandArgs(argc, argv);
  bool gpio0 = false;
  Vswervolf_core_tb* top = new Vswervolf_core_tb;

  VerilatedVcdC * tfp = 0;
  const char *vcd = Verilated::commandArgsPlusMatch("vcd=");
  if (vcd[0]) {
    Verilated::traceEverOn(true);
    tfp = new VerilatedVcdC;
    top->trace (tfp, 99);
    tfp->open ("trace.vcd");
  }

  uart_context_t uart_context;
  int baud_rate = 115200;
  uart_init(&uart_context, 1000*1000*1000, baud_rate);
  vluint64_t timeout = 0;
  const char *arg_timeout = Verilated::commandArgsPlusMatch("timeout=");
  if (arg_timeout[0])
    timeout = atoi(arg_timeout+9);

  signal(SIGINT, INThandler);

  top->clk = 1;
  top->rst = 1;
  while (!(done || Verilated::gotFinish())) {
    if (main_time == 100) {
      printf("Releasing reset\n");
      top->rst = 0;
    }
    top->eval();
    if (tfp)
      tfp->dump(main_time);
    if (baud_rate) do_uart(&uart_context, top->o_uart_tx);
    if (gpio0 != top->o_gpio) {
      printf("%lu: gpio0 is %s\n", main_time, top->o_gpio ? "on" : "off");
      gpio0 = top->o_gpio;
    }
    if (timeout && (main_time >= timeout)) {
      printf("Timeout: Exiting at time %lu\n", main_time);
      done = true;
    }
    top->clk = !top->clk;
    main_time+=20;
  }
  if (tfp)
    tfp->close();
  exit(0);
}
