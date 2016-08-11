//
//  CallResponse.swift
//  gRPC
//
//  Created by Tim Burks on 8/11/16.
//  Copyright © 2016 Google. All rights reserved.
//

/// Representation of a response to a gRPC call 
public class CallResponse {

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

  init(status:Int,
       statusDetails:String,
       message:ByteBuffer?) {
    self.status = status
    self.statusDetails = statusDetails
    self.message = message
  }
}
