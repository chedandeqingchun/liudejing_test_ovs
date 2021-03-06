/*
 * Copyright (c) 2016 Nicira, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <config.h>
#undef NDEBUG
#include "fatal-signal.h"
#include "ovs-rcu.h"
#include "ovs-thread.h"
#include "ovstest.h"
#include "util.h"

static void *
quiescer_main(void *aux OVS_UNUSED)
{
    /* A new thread must be not be quiescent */
    ovs_assert(!ovsrcu_is_quiescent());
    ovsrcu_quiesce_start();
    /* After the above call it must be quiescent */
    ovs_assert(ovsrcu_is_quiescent());

    return NULL;
}

static void
test_rcu_quiesce(void)
{
    pthread_t quiescer;

    quiescer = ovs_thread_create("quiescer", quiescer_main, NULL);

    /* This is the main thread of the process. After spawning its first
     * thread it must not be quiescent. */
    ovs_assert(!ovsrcu_is_quiescent());

    xpthread_join(quiescer, NULL);
}

static void
add_count(void *_count)
{
    unsigned *count = (unsigned *)_count;
    (*count) ++;
}

static void
test_rcu_barrier(void)
{
    unsigned count = 0;
    for (int i = 0; i < 10; i ++) {
        ovsrcu_postpone(add_count, &count);
    }

    ovsrcu_barrier();
    ovs_assert(count == 10);
}

static void
test_rcu(int argc OVS_UNUSED, char *argv[] OVS_UNUSED) {
    test_rcu_quiesce();
    test_rcu_barrier();
}

OVSTEST_REGISTER("test-rcu", test_rcu);
