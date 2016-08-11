//
//  Mutex.swift
//  gRPC
//
//  Created by Tim Burks on 8/10/16.
//  Copyright © 2016 Google. All rights reserved.
//

#if SWIFT_PACKAGE
  import CgRPC
#endif

public class Mutex {

  /// Pointer to underlying C representation
  private var mu: UnsafeMutablePointer<Void>!

  /// Initializes a Mutex
  public init() {
    mu = grpcshim_mutex_create();
  }

  deinit {
    grpcshim_mutex_destroy(mu);
  }

  /// Locks a Mutex
  ///
  /// Waits until no thread has a lock on the Mutex, 
  /// causes the calling thread to own an exclusive lock on the Mutex,
  /// then returns. 
  ///
  /// May block indefinitely or crash if the calling thread has a lock on the Mutex.
  public func lock() {
    grpcshim_mutex_lock(mu);
  }

  /// Unlocks a Mutex
  ///
  /// Releases an exclusive lock on the Mutex held by the calling thread.
  public func unlock() {
    grpcshim_mutex_unlock(mu);
  }
}
