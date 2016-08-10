//
//  Mutex.swift
//  gRPC
//
//  Created by Tim Burks on 8/10/16.
//  Copyright © 2016 Google. All rights reserved.
//

import Foundation

#if SWIFT_PACKAGE
  import gRPC_Core
#endif

public class Mutex {
  var mu: UnsafeMutablePointer<Void>!

  init() {
    mu = grpcshim_mutex_create();
  }

  deinit {
    grpcshim_mutex_destroy(mu);
  }

  func lock() {
    grpcshim_mutex_lock(mu);
  }

  func unlock() {
    grpcshim_mutex_unlock(mu);
  }
}
