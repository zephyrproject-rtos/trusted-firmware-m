###########################
Non-Secure Identity Manager
###########################
The ID of the current application/thread is known by TF-M, and the PS service
queries the ID of the currently running client via a dedicated API.

The identity of secure clients can be tracked by TF-M core, because it also
manages the contexts of the partitions. However to differentiate NS clients, it
relies on the services provided by the NS OS.

Tracking of context changes are possible by relying on the NS OS calling the
Thread Context Management for Armv8-M TrustZone APIs, as described
`here <https://www.keil.com/pack/doc/CMSIS/Core/html/group__context__trustzone__functions.html>`__

However TF-M needs an extra API, to assign a client ID to the TZ context created
as a result of the
``TZ_MemoryId_t TZ_AllocModuleContext_S (TZ_ModuleId_t module)`` call.
See ``interface/include/ext/tz_context.h`` for details of API declarations.

In case the NS OS doesn't use the Thread Context Management for Armv8-M
TrustZone APIs, then TF-M considers the NS SW as a single client, and assigns a
client ID to it automatically.

--------------

*Copyright (c) 2018-2021, Arm Limited. All rights reserved.*
