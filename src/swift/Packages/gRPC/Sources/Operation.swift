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

public class Operation {

  /// Pointer to underlying C representation
  var observer: UnsafeMutablePointer<Void>

  init(observer: UnsafeMutablePointer<Void>) {
    self.observer = observer
  }

  deinit {
    grpcshim_observer_destroy(observer);
  }
}

class Operation_SendInitialMetadata : Operation {
  init(metadata:Metadata) {
    super.init(observer:grpcshim_observer_create_send_initial_metadata(metadata.array))
  }
}

class Operation_SendMessage : Operation {
  init(message:ByteBuffer) {
    super.init(observer:grpcshim_observer_create_send_message())
    grpcshim_observer_send_message_set_message(observer, message.b);
  }
}

class Operation_SendCloseFromClient : Operation {
  init() {
    super.init(observer:grpcshim_observer_create_send_close_from_client())
  }
}

class Operation_SendStatusFromServer : Operation {
  init(status:Int,
       statusDetails:String,
       metadata:Metadata) {
    super.init(observer:grpcshim_observer_create_send_status_from_server(metadata.array))
    grpcshim_observer_send_status_from_server_set_status(observer, Int32(status));
    grpcshim_observer_send_status_from_server_set_status_details(observer, statusDetails);
  }
}

class Operation_ReceiveInitialMetadata : Operation {
  init() {
    super.init(observer:grpcshim_observer_create_recv_initial_metadata())
  }
  func metadata() -> Metadata {
    return Metadata(array:grpcshim_observer_recv_initial_metadata_get_metadata(observer));
  }
}

class Operation_ReceiveMessage : Operation {
  init() {
    super.init(observer:grpcshim_observer_create_recv_message())
  }
  func message() -> ByteBuffer? {
    if let b = grpcshim_observer_recv_message_get_message(observer) {
      return ByteBuffer(b:b)
    } else {
      return nil
    }
  }
}

class Operation_ReceiveStatusOnClient : Operation {
  init() {
    super.init(observer:grpcshim_observer_create_recv_status_on_client())
  }
  func metadata() -> Metadata {
    return Metadata(array:grpcshim_observer_recv_status_on_client_get_metadata(observer));
  }
  func status() -> Int {
    return grpcshim_observer_recv_status_on_client_status(observer);
  }
  func statusDetails() -> String {
    return String(cString:grpcshim_observer_recv_status_on_client_status_details(observer), encoding:String.Encoding.utf8)!
  }
}

class Operation_ReceiveCloseOnServer : Operation {
  init() {
    super.init(observer:grpcshim_observer_create_recv_close_on_server())
  }
}
