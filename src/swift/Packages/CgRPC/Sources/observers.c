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
#include "internal.h"
#include "shim.h"

#include <stdlib.h>
#include <string.h>

// create observers for each type of GRPC operation

grpcshim_observer_send_initial_metadata *grpcshim_observer_create_send_initial_metadata(grpcshim_metadata_array *metadata) {
  grpcshim_observer_send_initial_metadata *observer =
  (grpcshim_observer_send_initial_metadata *) malloc(sizeof (grpcshim_observer_send_initial_metadata));
  observer->op_type = GRPC_OP_SEND_INITIAL_METADATA;
  grpcshim_metadata_array_move_metadata(&(observer->initial_metadata), metadata);
  return observer;
}

grpcshim_observer_send_message *grpcshim_observer_create_send_message() {
  grpcshim_observer_send_message *observer =
  (grpcshim_observer_send_message *) malloc(sizeof (grpcshim_observer_send_message));
  observer->op_type = GRPC_OP_SEND_MESSAGE;

  return observer;
}

grpcshim_observer_send_close_from_client *grpcshim_observer_create_send_close_from_client() {
  grpcshim_observer_send_close_from_client *observer =
  (grpcshim_observer_send_close_from_client *) malloc(sizeof (grpcshim_observer_send_close_from_client));
  observer->op_type = GRPC_OP_SEND_CLOSE_FROM_CLIENT;

  return observer;
}

grpcshim_observer_send_status_from_server *grpcshim_observer_create_send_status_from_server(grpcshim_metadata_array *metadata) {
  grpcshim_observer_send_status_from_server *observer =
  (grpcshim_observer_send_status_from_server *) malloc(sizeof (grpcshim_observer_send_status_from_server));
  observer->op_type = GRPC_OP_SEND_STATUS_FROM_SERVER;
  grpcshim_metadata_array_move_metadata(&(observer->trailing_metadata), metadata);
  return observer;
}

grpcshim_observer_recv_initial_metadata *grpcshim_observer_create_recv_initial_metadata() {
  grpcshim_observer_recv_initial_metadata *observer =
  (grpcshim_observer_recv_initial_metadata *) malloc(sizeof (grpcshim_observer_recv_initial_metadata));
  observer->op_type = GRPC_OP_RECV_INITIAL_METADATA;
  return observer;
}

grpcshim_observer_recv_message *grpcshim_observer_create_recv_message() {
  grpcshim_observer_recv_message *observer =
  (grpcshim_observer_recv_message *) malloc(sizeof (grpcshim_observer_recv_message));
  observer->op_type = GRPC_OP_RECV_MESSAGE;
  observer->response_payload_recv = NULL;
  return observer;
}

grpcshim_observer_recv_status_on_client *grpcshim_observer_create_recv_status_on_client() {
  grpcshim_observer_recv_status_on_client *observer =
  (grpcshim_observer_recv_status_on_client *) malloc(sizeof (grpcshim_observer_recv_status_on_client));
  observer->op_type = GRPC_OP_RECV_STATUS_ON_CLIENT;
  return observer;
}

grpcshim_observer_recv_close_on_server *grpcshim_observer_create_recv_close_on_server() {
  grpcshim_observer_recv_close_on_server *observer =
  (grpcshim_observer_recv_close_on_server *) malloc(sizeof (grpcshim_observer_recv_close_on_server));
  observer->op_type = GRPC_OP_RECV_CLOSE_ON_SERVER;
  observer->was_cancelled = 0;
  return observer;
}

// apply observer to operation

void grpcshim_observer_apply(grpcshim_observer *observer, grpc_op *op) {
  op->op = observer->op_type;
  op->flags = 0;
  op->reserved = NULL;

  switch (observer->op_type) {
    case GRPC_OP_SEND_INITIAL_METADATA: {
      grpcshim_observer_send_initial_metadata *obs = (grpcshim_observer_send_initial_metadata *) observer;
      op->data.send_initial_metadata.count = obs->initial_metadata.count;
      op->data.send_initial_metadata.metadata = obs->initial_metadata.metadata;
      break;
    }
    case GRPC_OP_SEND_MESSAGE: {
      grpcshim_observer_send_message *obs = (grpcshim_observer_send_message *) observer;
      op->data.send_message = obs->request_payload;
      break;
    }
    case GRPC_OP_SEND_CLOSE_FROM_CLIENT: {
      break;
    }
    case GRPC_OP_SEND_STATUS_FROM_SERVER: {
      grpcshim_observer_send_status_from_server *obs = (grpcshim_observer_send_status_from_server *) observer;
      op->data.send_status_from_server.trailing_metadata_count = obs->trailing_metadata.count;
      op->data.send_status_from_server.trailing_metadata = obs->trailing_metadata.metadata;
      op->data.send_status_from_server.status = obs->status;
      op->data.send_status_from_server.status_details = obs->status_details;
      break;
    }
    case GRPC_OP_RECV_INITIAL_METADATA: {
      grpcshim_observer_recv_initial_metadata *obs = (grpcshim_observer_recv_initial_metadata *) observer;
      grpc_metadata_array_init(&(obs->initial_metadata_recv));
      op->data.recv_initial_metadata = &(obs->initial_metadata_recv);
      break;
    }
    case GRPC_OP_RECV_MESSAGE: {
      grpcshim_observer_recv_message *obs = (grpcshim_observer_recv_message *) observer;
      op->data.recv_message = &(obs->response_payload_recv);
      break;
    }
    case GRPC_OP_RECV_STATUS_ON_CLIENT: {
      grpcshim_observer_recv_status_on_client *obs = (grpcshim_observer_recv_status_on_client *) observer;
      grpc_metadata_array_init(&(obs->trailing_metadata_recv));
      obs->server_status = GRPC_STATUS_OK;
      obs->server_details = NULL;
      obs->server_details_capacity = 0;
      op->data.recv_status_on_client.trailing_metadata = &(obs->trailing_metadata_recv);
      op->data.recv_status_on_client.status = &(obs->server_status);
      op->data.recv_status_on_client.status_details = &(obs->server_details);
      op->data.recv_status_on_client.status_details_capacity = &(obs->server_details_capacity);
      break;
    }
    case GRPC_OP_RECV_CLOSE_ON_SERVER: {
      grpcshim_observer_recv_close_on_server *obs = (grpcshim_observer_recv_close_on_server *) observer;
      op->data.recv_close_on_server.cancelled = &(obs->was_cancelled);
      break;
    }
    default: {

    }
  }
}

// destroy all observers

void grpcshim_observer_destroy(grpcshim_observer *observer) {
  switch (observer->op_type) {
    case GRPC_OP_SEND_INITIAL_METADATA: {
      grpcshim_observer_send_initial_metadata *obs = (grpcshim_observer_send_initial_metadata *) observer;
      grpc_metadata_array_destroy(&(obs->initial_metadata));
      free(obs);
      break;
    }
    case GRPC_OP_SEND_MESSAGE: {
      grpcshim_observer_send_message *obs = (grpcshim_observer_send_message *) observer;
      grpc_byte_buffer_destroy(obs->request_payload);
      free(obs);
      break;
    }
    case GRPC_OP_SEND_CLOSE_FROM_CLIENT: {
      grpcshim_observer_send_close_from_client *obs = (grpcshim_observer_send_close_from_client *) observer;
      free(obs);
      break;
    }
    case GRPC_OP_SEND_STATUS_FROM_SERVER: {
      grpcshim_observer_send_status_from_server *obs = (grpcshim_observer_send_status_from_server *) observer;
      free(obs);
      break;
    }
    case GRPC_OP_RECV_INITIAL_METADATA: {
      grpcshim_observer_recv_initial_metadata *obs = (grpcshim_observer_recv_initial_metadata *) observer;
      grpc_metadata_array_destroy(&obs->initial_metadata_recv);
      free(obs);
      break;
    }
    case GRPC_OP_RECV_MESSAGE: {
      grpcshim_observer_recv_message *obs = (grpcshim_observer_recv_message *) observer;
      grpc_byte_buffer_destroy(obs->response_payload_recv);
      free(obs);
      break;
    }
    case GRPC_OP_RECV_STATUS_ON_CLIENT: {
      grpcshim_observer_recv_status_on_client *obs = (grpcshim_observer_recv_status_on_client *) observer;
      free(obs->server_details);
      grpc_metadata_array_destroy(&(obs->trailing_metadata_recv));
      free(obs);
      break;
    }
    case GRPC_OP_RECV_CLOSE_ON_SERVER: {
      grpcshim_observer_recv_close_on_server *obs = (grpcshim_observer_recv_close_on_server *) observer;
      free(obs);
      break;
    }
    default: {

    }
  }
}

grpcshim_byte_buffer *grpcshim_observer_recv_message_get_message(grpcshim_observer_recv_message *observer) {
  if (observer->response_payload_recv) {
    return grpc_byte_buffer_copy(observer->response_payload_recv);
  } else {
    return NULL;
  }
}

grpcshim_metadata_array *grpcshim_observer_recv_initial_metadata_get_metadata(grpcshim_observer_recv_initial_metadata *observer) {
  grpcshim_metadata_array *metadata = grpcshim_metadata_array_create();
  grpcshim_metadata_array_move_metadata(metadata, &(observer->initial_metadata_recv));
  return metadata;
}

long grpcshim_observer_recv_initial_metadata_count(grpcshim_observer_recv_initial_metadata *observer) {
  return observer->initial_metadata_recv.count;
}

grpcshim_metadata *grpcshim_observer_recv_initial_metadata_metadata(grpcshim_observer_recv_initial_metadata *observer, long i) {
  return &(observer->initial_metadata_recv.metadata[i]);
}

void grpcshim_observer_send_message_set_message(grpcshim_observer_send_message *observer, grpcshim_byte_buffer *message) {
  observer->request_payload = grpc_byte_buffer_copy(message);
}

void grpcshim_observer_send_status_from_server_set_status(grpcshim_observer_send_status_from_server *observer, int status) {
  observer->status = status;
}

void grpcshim_observer_send_status_from_server_set_status_details(grpcshim_observer_send_status_from_server *observer, const char *statusDetails) {
  observer->status_details = strdup(statusDetails);
}

grpcshim_metadata_array *grpcshim_observer_recv_status_on_client_get_metadata(grpcshim_observer_recv_status_on_client *observer) {
  grpcshim_metadata_array *metadata = grpcshim_metadata_array_create();
  grpcshim_metadata_array_move_metadata(metadata, &(observer->trailing_metadata_recv));
  return metadata;
}

long grpcshim_observer_recv_status_on_client_count(grpcshim_observer_recv_status_on_client *observer) {
  return observer->trailing_metadata_recv.count;
}

grpcshim_metadata *grpcshim_observer_recv_status_on_client_metadata(grpcshim_observer_recv_status_on_client *observer, long i) {
  return &(observer->trailing_metadata_recv.metadata[i]);
}

long grpcshim_observer_recv_status_on_client_status(grpcshim_observer_recv_status_on_client *observer) {
  return observer->server_status;
}

const char *grpcshim_observer_recv_status_on_client_status_details(grpcshim_observer_recv_status_on_client *observer) {
  return observer->server_details;
}

int grpcshim_observer_recv_close_on_server_was_cancelled(grpcshim_observer_recv_close_on_server *observer) {
  return observer->was_cancelled;
}

