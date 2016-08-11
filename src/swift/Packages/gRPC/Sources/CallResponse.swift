//
//  CallResponse.swift
//  gRPC
//
//  Created by Tim Burks on 8/11/16.
//  Copyright © 2016 Google. All rights reserved.
//
#if SWIFT_PACKAGE
  import CgRPC
#endif
import Foundation

/// Representation of a response to a gRPC call
public class CallResponse {

  // Error code that could be generated when the call is created
  public var error: grpc_call_error

  // Result of waiting for call completion
  public var completion: grpc_completion_type

  // Status code returned by server
  public var status: Int

  // Status message optionally returned by server
  public var statusDetails: String

  // Message returned by server
  public var message: ByteBuffer?

  // Initial metadata returned by server
  public var initialMetadata: Metadata?

  // Trailing metadata returned by server
  public var trailingMetadata: Metadata?

  // Initializes a response when error != GRPC_CALL_OK
  init(error: grpc_call_error) {
    self.error = error
    self.completion = GRPC_OP_COMPLETE
    self.status = 0
    self.statusDetails = ""
    self.message = nil
  }

  // Initializes a response when completion != GRPC_OP_COMPLETE
  init(completion: grpc_completion_type) {
    self.error = GRPC_CALL_OK
    self.completion = completion
    self.status = 0
    self.statusDetails = ""
    self.message = nil
  }

  // Initializes a response when error == GRPC_CALL_OK and completion == GRPC_OP_COMPLETE
  init(status:Int,
       statusDetails:String,
       message:ByteBuffer?) {
    self.error = GRPC_CALL_OK
    self.completion = GRPC_OP_COMPLETE
    self.status = status
    self.statusDetails = statusDetails
    self.message = message
  }

}
