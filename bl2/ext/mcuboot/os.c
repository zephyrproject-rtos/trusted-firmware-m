/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 Original code taken from mcuboot project at:
 https://github.com/runtimeco/mcuboot
 Modifications are Copyright (c) 2018 Arm Limited.
 */

#include <stdlib.h>
#include <string.h>

#include "os/os_heap.h"
#include <mbedtls/platform.h>

/* D(void *os_malloc(size_t size)) */
void *os_calloc(size_t nelem, size_t size)
{
    /* Note that this doesn't check for overflow.  Assume the
     * calls only come from within the app. */
    size_t total = nelem * size;
    void *buf = malloc(total);

    if (buf) {
        memset(buf, 0, total);
    }
    return buf;
}

void os_free(void *ptr)
{
    free(ptr);
}

/*
 * Initialize mbedtls to be able to use the local heap.
 */
void os_heap_init(void)
{
    mbedtls_platform_set_calloc_free(os_calloc, os_free);
}
