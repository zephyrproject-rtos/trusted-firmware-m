##############################
Mailbox NS Agent Design Update
##############################

:Organization: Arm Limited
:Contact: tf-m@lists.trustedfirmware.org

**********
Background
**********
The SPE component that maintains the non-secure clients' request is called
'NS Agent' in TF-M. Besides the Trustzone-based isolation mechanism, there is
one other isolation mechanism that implements individual PEs in physically
isolated cores respectively. NSPE and SPE transfer non-secure client requests
via inter-processor communication based on mailboxes. The component that
handles inter-processor communication messages is called ``mailbox NS Agent``.

.. note::
  There may be hardware components and software solutions containing 'mailbox'
  in their names. The concept ``mailbox`` in this document represent the
  mechanism described above, which is not referring to the external concepts.

When the first version ``mailbox NS Agent`` was introduced, the generic FFM
interrupt handling was not ready. Hence a customized solution
``Multiple Core`` is implemented. This customized implementation:

- Perform customized operations on SPM internal data in a deferred interrupt
  handler.
- Process mailbox operations as special cases in SPM common logic.

These behaviours couple SPM tightly with mailbox logic, which bring issues for
maintenance. To address the issue, an updated design shall:

- Make SPM manage other components in a unified way (For example, it is
  simpler for SPM if all non-SPM components under the IPC model act as
  ``processes``.)
- Can use FF-M compliant interrupt mechanism and APIs.

Following the above guidelines makes the ``mailbox NS agent`` work like a
``partition``. The agent has an endless loop and waits for signals, calls FFM
API based on the parsing result on the communication messages. But there are
still issues after looking closer to the requirements of the agent:

- SPM treats FFM Client API caller's ID as the client ID. While the mailbox NS
  agent may represent multiple non-secure clients. Hence it needs to tell
  SPM which non-secure client it is representing, and the default FFM Client
  API does not have such capability.
- FFM Client API blocks caller before the call is replied; while the
  mailbox NS Agent needs to respond to the non-secure interrupts in time.
  Blocking while waiting for a reply may cause the non-secure communication
  message not to be handled in time.

Extra design items need to be added to address the issues.

*************
Design Update
*************
The below figure shows the overall design to cover various component types.
NS Agents are the implementation-defined components that provide FF-M compliant
Client API to the non-secure clients. Hence from the view of the non-secure
clients, the FF-M client API behaviour follows the FF-M definition. And NS
Agent needs customization in SPM since it has extra requirements compared to
a generic secure partition.

.. figure:: /design_docs/media/mailbox_ns_agent1.*
    :align: center
    :name: fig-mailbox1
    :width: 70%

    Component types and the callable API set

.. note::
  3 non-SPM component types here: FFM-compliant Secure Partition
  (aka ``partition``), Trustzone-based NS Agent (aka ``Trustzone NS agent``)
  and mailbox-based NS Agent (aka ``mailbox NS agent``).
  ``Trustzone NS agent`` is mentioned here for the comparison purpose. The
  implementation details for this NS agent type is not introduced here.

To make the programming model close to the FFM compliance, the
``mailbox NS agent`` is designed as:

- Working like a standard Secure Partition under the IPC model, has one
  single thread, can call FFM standard API.
- Having a manifest file to describe the attributes and resources and a
  positive valued ``Partition ID`` in the manifest.

Services rely on the ``client_id`` to apply policy-checking, hence SPM
needs to know which ``client_id`` the mailbox NS Agent is representing when
mailbox API is calling Client API. The standard API treats the caller as the
client of the service, which means that an extended API is needed to support
identifying the non-secure client that is represented. SPM sets the
non-secure ``client_id`` into the message right at the moment the message is
going to be sent. Before this point, SPM performs the call based on the
agent's ID.

These ``Extended APIs`` are non-blocking, unlike the standard FF-M Client
APIs. This can improve the communication efficiency between NS clients and
mailbox NS agents. With this mechanism, extra signals and APIs for message
acknowledges are also required.

.. note::
  A standard Secure Partition gets errors when calling the ``Extended API``.

The secure mapped memory for mailbox communication is one of the agent's
assets. As the agent has the capability to forward and represent non-secure
clients, it is the agent's duty to identify the non-secure clients it is
representing.

Updated programming interfaces
==============================
These Client APIs are expanded from the standard Client APIs:

- ``agent_psa_connect`` is extended from ``psa_connect``.
- ``agent_psa_call`` is extended from ``psa_call``.

And to cooperate with the changed behaviour of these APIs, extra defined
signals and types are also involved.

.. note::
  Namespace ``agent`` is involved for NS Agent callable API; namespace ``tfm``
  is involved for TF-M specific concepts. Even though ``agent`` is TF-M
  specific at the current stage, it is proposed to be a common concept for
  general FF-M compliant implementations, hence assigning ``agent`` for
  proposed API and data structures.

Agent-specific Client API
=========================
``agent_psa_connect`` is the API added to support agent forwarding NS
requests.

.. code-block:: c

  psa_handle_t agent_psa_connect(uint32_t sid, uint32_t version,
                                 int32_t ns_client_id, void *client_data);

One extra parameter ``ns_client_id`` added to tell SPM which NS client the
agent is representing when API gets called. It is recorded in the handle
association data in SPM and requires to be a negative value; ZERO or positive
values are invalid non-secure client IDs, SPM does not use these invalid IDs
in the message. Instead, it puts the agent's ID into the messaging in this
case. This mechanism can provide chances for the agents calling APIs for their
own service accessing and API works asynchronously.

As mentioned, the standard FFM Client service accessing API are blocked until
the IPC message gets replied to. This API returns immediately without waiting
for acknowledgement. A ``psa_handle_t`` is allocated and returned if no error
occurred because a unique value is needed to help the agent manage the
non-secure clients and the requests. The subsequent ``agent_psa_call``
or ``agent_psa_close`` with this allocated but not acknowledged handle gets
an ``In progress`` status code.

.. code-block:: c

  typedef struct {
      psa_invec     in_vecs[PSA_MAX_IOVEC];
      psa_outvec    out_vecs[PSA_MAX_IOVEC];
  } client_vectors_t;

  typedef struct {
      int32_t ns_client_id;
      void    *client_data;
  } client_param_t;

  psa_status_t agent_psa_call(psa_handle_t handle, int32_t type,
                              client_vectors_t *vecs, client_param_t *params);

Compared to the standard ``psa_call``, this API:

- Is asynchronous.
- Squashes the ``psa_invec_t`` and ``psa_outvec_t`` into a new composited
  parameter ``vecs`` to make the ABI APCS compliant.
- one extra parameter ``params`` for ``agent_psa_call`` stands for the
  auxiliary data added. This member is ignored for connection-based services
  because ``agent_psa_connect`` already assigned one hence this member is for
  stateless services only. And the same, ZERO or positive values are ignored
  during the ``agent_psa_call`` lifecycle.

.. note::
  For stateless services, a temporary ``psa_handle_t`` is allocated and
  returned. This handle is freed after the agent read the acknowledgement by
  ``psa_get``. This is also introduced in the subsequent introduction on
  `Agent-specific signal`_.

Agent-specific signal
=====================
To cooperate with the agent-specific API, one extra acknowledgement signal is
defined:

.. code-block:: c

  #define PSA_MSG_ACK            (0x00000004u)

This signal can be sent to agent type component only. An agent can call
``psa_get`` with this signal to get one acknowledged message. This signal is
cleared when all queued messages for the agent have been retrieved using
``psa_get``. SPM assembles the information into agent provided message object.
For the stateless handle, the internal handle object is freed after this
``psa_get`` call. The agent can know what kind of message is acknowledged by
the ``type`` member in the ``psa_msg_t``, and the ``client_data`` passed in is
put in member ``rhandle``. If no 'PSA_MSG_ACK' signals pending, calling
``psa_get`` gets an state representing ``not ready`` to the caller. This
state is to be defined.

Code Example
============

.. code-block:: c

  /*
   * The actual implementation can change this __customized_t freely, or
   * discard this type and apply some in-house mechanism - the example
   * here is to introduce how an agent works only.
   */
  struct __customized_t {
      int32_t      type;
      int32_t      client_id;
      psa_handle_t handle;
      psa_handle_t status;
  };

  void mailbox_main(void)
  {
      psa_signal_t   signals;
      psa_status_t   status;
      psa_msg_t      msg;
      client_param_t client_param;
      struct __customized_t ns_msg;

      while (1) {
          signals = psa_wait(ALL, BLOCK);

          if (signals & MAILBOX_INTERRUPT_SIGNAL) {
              /* NS memory check needs to be performed. */
              __customized_platform_get_mail(&ns_msg);

              /*
               * MACRO 'SID', 'VER', 'NSID' and 'VECTORS' represents necessary
               * information extraction from 'ns_msg', put MACRO names here
               * and leave the details to the implementation.
               */
              if (ns_msg.type == PSA_IPC_CONNECT) {
                  ns_msg.handle = agent_psa_connect(SID(ns_msg), VER(ns_msg),
                                                    NSID(ns_msg), &ns_msg);
              } else if (ns_msg.type == PSA_IPC_CLOSE) {
                  psa_close(ns_msg.handle);
              } else {
                  /* Other types as call type and let API check errors. */
                  client_param.ns_client_id = ns_msg.client_id;
                  client_param.client_data  = &ns_msg;

                  ns_msg.status = agent_psa_call(ns_msg.handle,
                                                 ns_msg.type,
                                                 VECTORS(ns_msg),
                                                 &client_param);
                  /* Handle the stateless service case. */
                  if (ns_msg.handle == NULL &&
                      ns_msg.status != PSA_ERROR_IN_PROGRESS) {
                      ns_msg.handle = (psa_handle_t)ns_msg.status;
                  }
              }
          } else if (signals & PSA_MSG_ACK) {
              /* The handle is freed for stateless service after 'psa_get'. */
              status        = psa_get(PSA_MSG_ACK, &msg);
              ms_msg        = msg.rhandle;
              ns_msg.status = status;
              __customized_platform__send_mail(&ns_msg);
          }
      }
  }

.. note::
  ``__customized*`` API are implementation-specific APIs to be implemented by
  the mailbox Agent developer.

****************************
API implementation reference
****************************
Takes ``psa_call`` as the example here to showcase the difference between the
interface and its calling implementation logic. The prototype of the
implementation logic for ``psa_call`` in SPM is:

.. code-block:: c

  psa_status_t tfm_spm_client_psa_call(psa_handle_t handle,
                                       uint32_t ctrl_param,
                                       const psa_invec *inptr,
                                       psa_outvec *outptr);

And the prototype for agent-specific ``agent_psa_call``:

.. code-block:: c

  psa_status_t agent_psa_call(psa_handle_t handle, int32_t ctrl_param,
                              client_vectors_t *vecs, client_param_t *params);


``agent_psa_call`` reuses the existing ``tfm_spm_client_psa_call`` as the
internal implementation, the procedure after ``agent_psa_call`` gets called is
slightly different compared to a classic ``psa_call`` procedure
(Error handling is not described here as it works as usual):

- Extract ``inptr`` and ``outptr`` from ``vecs`` before calling
  ``tfm_spm_client_psa_call``.
- After ``tfm_spm_client_psa_call`` created a ``psa_msg_t`` instance, the
  member ``client_id`` in the instance needs to be updated to a value given by
  ``ns_client_id`` of argument ``params`` to indicate the non-secure client
  the mailbox agent is representing.
- The member ``client_data`` in the argument ``params`` needs to be recorded
  for a future reply usage.

Here ``tfm_spm_client_psa_call`` needs more inputs to accomplish the required
operations such as NS client ID updating and backup the ``client_data``. But
it would be inefficient if these inputs were given by arguments, because the
caller was not always an agent so in most of the cases these extra arguments
were not used, but the classic ``psa_call`` procedure would be forced to fill
them always before calling ``tfm_spm_client_psa_call``.

A solution referencing the local storage scheme can save the cost spent on
extra arguments passing, this solution:

- Calls an agent-specific callback for the extra steps during
  ``tfm_spm_client_psa_call`` when the caller is a mailbox agent.
- Puts callback required inputs in the local storage.

Here is the pseudo-code for this solution:

.. code-block:: c

  psa_status_t tfm_spm_client_psa_call(psa_handle_t handle,
                                       uint32_t ctrl_param,
                                       const psa_invec *inptr,
                                       psa_outvec *outptr)
  {
      ...
      if (CALLER()->flags & MAILBOX_AGENT) {
          post_handling_mailbox(p_connection);
      }
      ...
  }

  void post_handling_mailbox(connection_t *p_conn)
  {
      p_conn->msg.client_id   = LOCAL_STORAGE()->client_param.ns_client_id;
      p_conn->msg.client_data = LOCAL_STORAGE()->client_param.client_data;
  }

The ``client_data`` saved in the connection instance will be returned to the
caller when it calls ``psa_get`` to retrieve the reply.

Local storage for SPM
=====================
The local storage mechanism can be similar to what :doc:`SPRTL </design_docs/services/secure_partition_runtime_library>`
does. The stack top is still the ideal place for local storage indicator
because SPM also has its dedicated stack. For Armv8m, shifting the xSPLIM to
detect stack overflow is an advantage. For earlier architecture versions, a
global variable saving the stack top is still applicable.

.. important::
  This mechanism may conflict with some private 'alloca' implementations,
  remember the local storage must be put at the top of the stack,
  and `alloca` working buffer is put after (usually higher addresses for the
  descending stack case) the local storage data.

Example:

.. code-block:: c

  void *claim_local_storage(uint32_t sz)
  {
      PSPLIM += sz;
      return PSPLIM;
  }

Customized manifest attribute
=============================
Two extra customized manifest attributes are added:

============= ====================================================
Name          Description
============= ====================================================
ns_agent      Indicate if manifest owner is an Agent.
------------- ----------------------------------------------------
ns_client_ids Possible non-secure Client ID values (<0).
============= ====================================================

Attribute 'ns_client_ids' can be a set of numbers, or it can use a range
expression such as [min, max]. The tooling can detect ID overlap between
multiple non-secure agents.

***********************
Manifest tooling update
***********************
The manifest for agents involves specific keys ('ns_agent' e.g.), these keys
give hints about how to achieve out-of-FFM partitions which might be abused
easily by developers, for example, claim partitions as agents. Some
restrictions need to be applied in the manifest tool to limit the general
secure service development referencing these keys.

.. note::
  The limitations can mitigate the abuse but can't prevent it, as developers
  own all the source code they are working with.

One mechanism: adding a confirmation in the partition list file.

.. parsed-literal::

  "description": "Non-Secure Mailbox Agent",
  "manifest": "${CMAKE_SOURCE_DIR}/secure_fw/partitions/ns_agent_mailbox/ns_agent_mailbox.yaml",
  "non_ffm_attributes": "ns_agent", "other_option",

``non_ffm_attributes`` tells the manifest tool that ``ns_agent`` is valid
in ns_agent_mailbox.ymal. Otherwise, the manifest tool reports an error when a
non-agent service abuses ns_agent in its manifest.

***********************************
Runtime programming characteristics
***********************************

Mailbox agent shall not be blocked by Agent-specific APIs. It can be blocked when:

- It is calling standard PSA Client APIs.
- It is calling ``psa_wait``.

IDLE processing
===============
Only ONE place is recommended to enter IDLE. The place is decided based on the
system topologies:

- If there is one Trustzone-based NSPE, this NSPE is the recommended place no
  matter how many mailbox agents are in the system.
- If there are only mailbox-based NSPEs, entering IDLE can happen in
  one of the mailbox agents.

The solution is:

- An IDLE entering API is provided in SPRTL.
- A partition without specific flag can't call this API.
- The manifest tooling counts the partitions with this specific flag, and
  assert errors when multiple instances are found.

--------------

*Copyright (c) 2022, Arm Limited. All rights reserved.*
