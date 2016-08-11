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
#if SWIFT_PACKAGE
  import CgRPC
#endif
import Foundation // for String.Encoding

public class Handler {
  /// Pointer to underlying C representation
  var h: UnsafeMutablePointer<Void>!

  public var requestMetadata: Metadata

  init(h:UnsafeMutablePointer<Void>!) {
    self.h = h;
    self.requestMetadata = Metadata()
  }

  deinit {
    grpcshim_handler_destroy(self.h)
  }

  public func host() -> String {
    return String(cString:grpcshim_handler_host(h), encoding:String.Encoding.utf8)!;
  }

  public func method() -> String {
    return String(cString:grpcshim_handler_method(h), encoding:String.Encoding.utf8)!;
  }

  public func caller() -> String {
    return String(cString:grpcshim_handler_call_peer(h), encoding:String.Encoding.utf8)!;
  }

  func call() -> Call {
    return Call(call: grpcshim_handler_get_call(h), owned:false)
  }

  func completionQueue() -> CompletionQueue {
    return CompletionQueue(cq:grpcshim_handler_get_completion_queue(h));
  }

  public func receiveMessage() -> (grpc_completion_type, ByteBuffer?) {

    let initialMetadata = Metadata()
    initialMetadata.add(key:"a", value:"Apple")
    initialMetadata.add(key:"b", value:"Banana")
    initialMetadata.add(key:"c", value:"Cherry")

    let operation_sendInitialMetadata = Operation_SendInitialMetadata(metadata:initialMetadata);

    let operation_receiveMessage = Operation_ReceiveMessage()

    let operations: [Operation] = [
      operation_sendInitialMetadata,
      operation_receiveMessage
    ]

    let call = self.call()
    let call_status = call.performOperations(completionQueue:self.completionQueue(),
                                             operations:operations,
                                             tag:222,
                                             timeout:5)
    if (call_status == GRPC_OP_COMPLETE) {
      return (call_status, operation_receiveMessage.message())
    } else {
      return (call_status, nil)
    }
  }

  public func sendResponse(message: ByteBuffer) -> grpc_completion_type {
    let operation_receiveCloseOnServer = Operation_ReceiveCloseOnServer();

    let trailingMetadata = Metadata()
    trailingMetadata.add(key:"0", value:"zero")
    trailingMetadata.add(key:"1", value:"one")
    trailingMetadata.add(key:"2", value:"two")

    let operation_sendStatusFromServer = Operation_SendStatusFromServer(status:0,
                                                                        statusDetails:"wxyz",
                                                                        metadata:trailingMetadata)

    let operation_sendMessage = Operation_SendMessage(message:message)

    let operations: [Operation] = [
      operation_receiveCloseOnServer,
      operation_sendStatusFromServer,
      operation_sendMessage
    ]

    let call = self.call()
    return call.performOperations(completionQueue:self.completionQueue(),
                                  operations:operations,
                                  tag:333,
                                  timeout:5)
  }
}
