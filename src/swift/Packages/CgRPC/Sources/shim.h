/*
 *
 * Copyright 2016, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef grpcshim_h
#define grpcshim_h

#import <stdlib.h>

// This file lists C functions and types used to build Swift gRPC support

#ifndef grpcshim_internal_h
// all domain types are opaque pointers
#define grpcshim_byte_buffer                      void
#define grpcshim_call                             void
#define grpcshim_client                           void
#define grpcshim_completion_queue                 void
#define grpcshim_handler                          void
#define grpcshim_metadata                         void
#define grpcshim_metadata_array                   void
#define grpcshim_mutex                            void
#define grpcshim_observer                         void
#define grpcshim_observer_send_initial_metadata   void
#define grpcshim_observer_send_message            void
#define grpcshim_observer_send_close_from_client  void
#define grpcshim_observer_send_status_from_server void
#define grpcshim_observer_recv_initial_metadata   void
#define grpcshim_observer_recv_message            void
#define grpcshim_observer_recv_status_on_client   void
#define grpcshim_observer_recv_close_on_server    void
#define grpcshim_server                           void

/** Result of a grpc call. If the caller satisfies the prerequisites of a
 particular operation, the grpc_call_error returned will be GRPC_CALL_OK.
 Receiving any other value listed here is an indication of a bug in the
 caller. */
typedef enum grpc_call_error {
  /** everything went ok */
  GRPC_CALL_OK = 0,
  /** something failed, we don't know what */
  GRPC_CALL_ERROR,
  /** this method is not available on the server */
  GRPC_CALL_ERROR_NOT_ON_SERVER,
  /** this method is not available on the client */
  GRPC_CALL_ERROR_NOT_ON_CLIENT,
  /** this method must be called before server_accept */
  GRPC_CALL_ERROR_ALREADY_ACCEPTED,
  /** this method must be called before invoke */
  GRPC_CALL_ERROR_ALREADY_INVOKED,
  /** this method must be called after invoke */
  GRPC_CALL_ERROR_NOT_INVOKED,
  /** this call is already finished
   (writes_done or write_status has already been called) */
  GRPC_CALL_ERROR_ALREADY_FINISHED,
  /** there is already an outstanding read/write operation on the call */
  GRPC_CALL_ERROR_TOO_MANY_OPERATIONS,
  /** the flags value was illegal for this call */
  GRPC_CALL_ERROR_INVALID_FLAGS,
  /** invalid metadata was passed to this call */
  GRPC_CALL_ERROR_INVALID_METADATA,
  /** invalid message was passed to this call */
  GRPC_CALL_ERROR_INVALID_MESSAGE,
  /** completion queue for notification has not been registered with the
   server */
  GRPC_CALL_ERROR_NOT_SERVER_COMPLETION_QUEUE,
  /** this batch of operations leads to more operations than allowed */
  GRPC_CALL_ERROR_BATCH_TOO_BIG,
  /** payload type requested is not the type registered */
  GRPC_CALL_ERROR_PAYLOAD_TYPE_MISMATCH
} grpc_call_error;

/** The type of completion (for grpc_event) */
typedef enum grpc_completion_type {
  /** Shutting down */
  GRPC_QUEUE_SHUTDOWN,
  /** No event before timeout */
  GRPC_QUEUE_TIMEOUT,
  /** Operation completion */
  GRPC_OP_COMPLETE
} grpc_completion_type;

#endif

// directly expose a few grpc library functions
void grpc_init();
void grpc_shutdown();
const char *grpc_version_string();

// client support
grpcshim_client *grpcshim_client_create(const char *address);
void grpcshim_client_destroy(grpcshim_client *client);
grpcshim_call *grpcshim_client_create_call(grpcshim_client *client,
                                   const char *method,
                                   const char *host,
                                   double timeout);
grpcshim_completion_queue *grpcshim_client_completion_queue(grpcshim_client *client);

// server support
grpcshim_server *grpcshim_server_create(const char *address);
void grpcshim_server_destroy(grpcshim_server *s);
void grpcshim_server_start(grpcshim_server *s);
grpcshim_completion_queue *grpcshim_server_get_completion_queue(grpcshim_server *s);

// completion queues
grpc_completion_type grpcshim_completion_queue_get_next_event(grpcshim_completion_queue *cq,
                                                              double timeout);
void grpcshim_completion_queue_drain(grpcshim_completion_queue *cq);

// server request handlers
grpcshim_handler *grpcshim_handler_create_with_server(grpcshim_server *server);
void grpcshim_handler_destroy(grpcshim_handler *h);
grpcshim_call *grpcshim_handler_get_call(grpcshim_handler *h);
grpcshim_completion_queue *grpcshim_handler_get_completion_queue(grpcshim_handler *h);

grpc_call_error grpcshim_handler_request_call(grpcshim_handler *h,
                                              grpcshim_metadata_array *metadata,
                                              long tag);
grpc_completion_type grpcshim_handler_wait_for_request(grpcshim_handler *h,
                                                       grpcshim_metadata_array *metadata,
                                                       double timeout);
const char *grpcshim_handler_host(grpcshim_handler *h);
const char *grpcshim_handler_method(grpcshim_handler *h);
const char *grpcshim_handler_call_peer(grpcshim_handler *h);

// call support
void grpcshim_call_destroy(grpcshim_call *call);
void grpcshim_call_reserve_space_for_operations(grpcshim_call *call, int max_operations);
void grpcshim_call_add_operation(grpcshim_call *call, grpcshim_observer *observer);
grpc_call_error grpcshim_call_perform(grpcshim_call *call, long tag);

// metadata support
grpcshim_metadata_array *grpcshim_metadata_array_create();
void grpcshim_metadata_array_destroy(grpcshim_metadata_array *array);
size_t grpcshim_metadata_array_get_count(grpcshim_metadata_array *array);
const char *grpcshim_metadata_array_get_key_at_index(grpcshim_metadata_array *array, size_t index);
const char *grpcshim_metadata_array_get_value_at_index(grpcshim_metadata_array *array, size_t index);
void grpcshim_metadata_array_move_metadata(grpcshim_metadata_array *dest, grpcshim_metadata_array *src);
void grpcshim_metadata_array_append_metadata(grpcshim_metadata_array *metadata, const char *key, const char *value);

// mutex support
grpcshim_mutex *grpcshim_mutex_create();
void grpcshim_mutex_destroy(grpcshim_mutex *mu);
void grpcshim_mutex_lock(grpcshim_mutex *mu);
void grpcshim_mutex_unlock(grpcshim_mutex *mu);

// byte buffer support
void grpcshim_byte_buffer_destroy(grpcshim_byte_buffer *bb);
grpcshim_byte_buffer *grpcshim_byte_buffer_create_with_string(const char *string);
const char *grpcshim_byte_buffer_as_string(grpcshim_byte_buffer *bb);

// observers

// constructors
grpcshim_observer_send_initial_metadata   *grpcshim_observer_create_send_initial_metadata(grpcshim_metadata_array *metadata);
grpcshim_observer_send_message            *grpcshim_observer_create_send_message();
grpcshim_observer_send_close_from_client  *grpcshim_observer_create_send_close_from_client();
grpcshim_observer_send_status_from_server *grpcshim_observer_create_send_status_from_server(grpcshim_metadata_array *metadata);
grpcshim_observer_recv_initial_metadata   *grpcshim_observer_create_recv_initial_metadata();
grpcshim_observer_recv_message            *grpcshim_observer_create_recv_message();
grpcshim_observer_recv_status_on_client   *grpcshim_observer_create_recv_status_on_client();
grpcshim_observer_recv_close_on_server    *grpcshim_observer_create_recv_close_on_server();

// destructor
void grpcshim_observer_destroy(grpcshim_observer *observer);

// GRPC_OP_SEND_INITIAL_METADATA


// GRPC_OP_SEND_MESSAGE
void grpcshim_observer_send_message_set_message(grpcshim_observer_send_message *observer,
                                            grpcshim_byte_buffer *message);

// GRPC_OP_SEND_CLOSE_FROM_CLIENT
// -- no special handlers --

// GRPC_OP_SEND_STATUS_FROM_SERVER
void grpcshim_observer_send_status_from_server_set_status
(grpcshim_observer_send_status_from_server *observer,
 int status);

void grpcshim_observer_send_status_from_server_set_status_details
(grpcshim_observer_send_status_from_server *observer,
 const char *statusDetails);

// GRPC_OP_RECV_INITIAL_METADATA
grpcshim_metadata_array *grpcshim_observer_recv_initial_metadata_get_metadata
(grpcshim_observer_recv_initial_metadata *observer);

long grpcshim_observer_recv_initial_metadata_count
(grpcshim_observer_recv_initial_metadata *observer);

grpcshim_metadata *grpcshim_observer_recv_initial_metadata_metadata
(grpcshim_observer_recv_initial_metadata *observer,
 long i);

// GRPC_OP_RECV_MESSAGE
grpcshim_byte_buffer *grpcshim_observer_recv_message_get_message
(grpcshim_observer_recv_message *observer);

// GRPC_OP_RECV_STATUS_ON_CLIENT
grpcshim_metadata_array *grpcshim_observer_recv_status_on_client_get_metadata
(grpcshim_observer_recv_status_on_client *observer);



long grpcshim_observer_recv_status_on_client_count
(grpcshim_observer_recv_status_on_client *observer);

grpcshim_metadata *grpcshim_observer_recv_status_on_client_metadata
(grpcshim_observer_recv_status_on_client *observer,
 long i);

long grpcshim_observer_recv_status_on_client_status
(grpcshim_observer_recv_status_on_client *observer);

const char *grpcshim_observer_recv_status_on_client_status_details
(grpcshim_observer_recv_status_on_client *observer);

// GRPC_OP_RECV_CLOSE_ON_SERVER
int grpcshim_observer_recv_close_on_server_was_cancelled
(grpcshim_observer_recv_close_on_server *observer);

#endif
