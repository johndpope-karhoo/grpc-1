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

public class Client {

  /// Pointer to underlying C representation
  var c: UnsafeMutablePointer<Void>!

  public init(address: String) {
    c = grpcshim_client_create(address)
  }

  deinit {
    grpcshim_client_destroy(c)
  }

  func completionQueue() -> CompletionQueue {
    return CompletionQueue(cq:grpcshim_client_completion_queue(c))
  }

  func createCall(method:String, host:String, timeout:Double) -> Call {
    let call = grpcshim_client_create_call(c, method, host, timeout)!
    return Call(call:call, owned:true)
  }

  public func performRequest(host: String,
                             method: String,
                             message: ByteBuffer,
                             metadata: Metadata) -> CallResponse  {

    let operation_sendInitialMetadata = Operation_SendInitialMetadata(metadata:metadata);

    let operation_sendMessage = Operation_SendMessage(message:message)

    let operation_sendCloseFromClient = Operation_SendCloseFromClient()

    let operation_receiveInitialMetadata = Operation_ReceiveInitialMetadata()

    let operation_receiveStatusOnClient = Operation_ReceiveStatusOnClient()

    let operation_receiveMessage = Operation_ReceiveMessage()

    let operations: [Operation] = [
      operation_sendInitialMetadata,
      operation_sendMessage,
      operation_sendCloseFromClient,
      operation_receiveInitialMetadata,
      operation_receiveStatusOnClient,
      operation_receiveMessage
    ]

    let call = self.createCall(method:method, host:host, timeout:5)
    let call_status = call.performOperations(completionQueue:self.completionQueue(),
                                             operations:operations,
                                             tag:111,
                                             timeout:5)
    if (call_status == GRPC_OP_COMPLETE) {
      let response = CallResponse(status:operation_receiveStatusOnClient.status(),
                                  statusDetails:operation_receiveStatusOnClient.statusDetails(),
                                  message:operation_receiveMessage.message())
      response.initialMetadata = operation_receiveInitialMetadata.metadata();
      response.trailingMetadata = operation_receiveStatusOnClient.metadata();
      return response
    } else {
      return CallResponse(status:0, statusDetails:"", message:nil)
    }
  }
}

public class CallResponse {
  public var status: Int
  public var statusDetails: String
  public var message: ByteBuffer?
  public var initialMetadata: Metadata?
  public var trailingMetadata: Metadata?

  init(status:Int, statusDetails:String, message:ByteBuffer?) {
    self.status = status
    self.statusDetails = statusDetails
    self.message = message
  }
}
