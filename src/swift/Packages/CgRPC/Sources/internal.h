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
#ifndef grpcshim_internal_h
#define grpcshim_internal_h

#include <grpc/grpc.h>
#include <grpc/grpc_security.h>
#include <grpc/byte_buffer_reader.h>
#include <grpc/impl/codegen/alloc.h>

typedef struct {
  grpc_call *call;
  grpc_op *ops;
  int ops_count;
} grpcshim_call;

typedef struct {
  grpc_channel *client;
  grpc_completion_queue *completion_queue;
} grpcshim_client;

typedef struct {
  grpc_server *server;
  grpc_completion_queue *completion_queue;
  int port;
} grpcshim_server;

typedef struct {
  grpcshim_server *server;
  grpc_completion_queue *completion_queue;

  grpc_metadata_array request_metadata_recv;
  grpc_call_details call_details;
  grpc_call *server_call; // session

} grpcshim_handler;

typedef grpc_byte_buffer grpcshim_byte_buffer;
typedef grpc_completion_queue grpcshim_completion_queue;
typedef grpc_metadata grpcshim_metadata;
typedef grpc_metadata_array grpcshim_metadata_array;
typedef gpr_mu grpcshim_mutex;

// OPERATIONS

typedef struct {
  grpc_op_type op_type;
} grpcshim_observer;

typedef struct {
  grpc_op_type op_type;
  grpc_metadata_array initial_metadata;
} grpcshim_observer_send_initial_metadata;

typedef struct {
  grpc_op_type op_type;
  grpc_byte_buffer *request_payload;
} grpcshim_observer_send_message;

typedef struct {
  grpc_op_type op_type;
} grpcshim_observer_send_close_from_client;

typedef struct {
  grpc_op_type op_type;
  grpc_metadata_array trailing_metadata;
  grpc_status_code status;
  char *status_details;
} grpcshim_observer_send_status_from_server;

typedef struct {
  grpc_op_type op_type;
  grpc_metadata_array initial_metadata_recv;
} grpcshim_observer_recv_initial_metadata;

typedef struct {
  grpc_op_type op_type;
  grpc_byte_buffer *response_payload_recv;
} grpcshim_observer_recv_message;

typedef struct {
  grpc_op_type op_type;
  grpc_metadata_array trailing_metadata_recv;
  grpc_status_code server_status;
  char *server_details;
  size_t server_details_capacity;
} grpcshim_observer_recv_status_on_client;

typedef struct {
  grpc_op_type op_type;
  int was_cancelled;
} grpcshim_observer_recv_close_on_server;

// internal utilities
void *grpcshim_create_tag(intptr_t t);
gpr_timespec grpcshim_deadline_in_seconds_from_now(float seconds);

void grpcshim_observer_apply(grpcshim_observer *observer, grpc_op *op);

#endif /* grpcshim_internal_h */
