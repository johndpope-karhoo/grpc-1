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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

grpcshim_client *grpcshim_client_create(const char *address) {
  grpcshim_client *c = (grpcshim_client *) malloc(sizeof (grpcshim_client));
  // create the client
  grpc_channel_args client_args;
  client_args.num_args = 0;
  c->client = grpc_insecure_channel_create(address, &client_args, NULL);
  c->completion_queue = grpc_completion_queue_create(NULL);
  return c;
}

void grpcshim_client_destroy(grpcshim_client *c) {
  grpc_channel_destroy(c->client);
  c->client = NULL;

  grpc_completion_queue_shutdown(c->completion_queue);
  grpcshim_completion_queue_drain(c->completion_queue);
  grpc_completion_queue_destroy(c->completion_queue);
  free(c);
}

grpcshim_call *grpcshim_client_create_call(grpcshim_client *client,
                                           const char *method,
                                           const char *host,
                                           double timeout) {
  // create call
  gpr_timespec deadline = grpcshim_deadline_in_seconds_from_now(timeout);
  grpc_call *client_call = grpc_channel_create_call(client->client,
                                                    NULL,
                                                    GRPC_PROPAGATE_DEFAULTS,
                                                    client->completion_queue,
                                                    method,
                                                    host,
                                                    deadline,
                                                    NULL);
  grpcshim_call *call = (grpcshim_call *) malloc(sizeof(grpcshim_call));
  memset(call, 0, sizeof(grpcshim_call));
  call->call = client_call;
  return call;
}

grpcshim_completion_queue *grpcshim_client_completion_queue(grpcshim_client *client) {
  return client->completion_queue;
}
