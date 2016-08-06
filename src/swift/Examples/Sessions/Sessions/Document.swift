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
import Cocoa
import gRPC


// https://gist.github.com/rickw/cc198001f5f3aa59ae9f
extension NSTextView {
  func appendText(line: String) {
    DispatchQueue.main.async {
      let attrDict = [NSFontAttributeName: NSFont.systemFont(ofSize:12.0)]
      let astring = AttributedString(string: "\(line)\n", attributes: attrDict)
      self.textStorage?.append(astring)
      let loc = self.string?.lengthOfBytes(using:String.Encoding.utf8)

      let range = NSRange(location: loc!, length: 0)
      self.scrollRangeToVisible(range)
    }
  }
}

class Document: NSDocument {

  @IBOutlet weak var hostField: NSTextField!
  @IBOutlet weak var portField: NSTextField!
  @IBOutlet weak var connectionSelector: NSSegmentedControl!
  @IBOutlet weak var startButton: NSButton!
  @IBOutlet var textView: NSTextView!
  // http://stackoverflow.com/questions/24062437/cannot-form-weak-reference-to-instance-of-class-nstextview

  override init() {
      super.init()
  }

  override class func autosavesInPlace() -> Bool {
    return true
  }

  override var windowNibName: String? {
    return "Document"
  }

  override func data(ofType typeName: String) throws -> Data {
    // Insert code here to write your document to data of the specified type. If outError != nil, ensure that you create and set an appropriate error when returning nil.
    // You can also choose to override fileWrapperOfType:error:, writeToURL:ofType:error:, or writeToURL:ofType:forSaveOperation:originalContentsURL:error: instead.
    throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
  }

  override func read(from data: Data, ofType typeName: String) throws {
    // Insert code here to read your document from the given data of the specified type. If outError != nil, ensure that you create and set an appropriate error when returning false.
    // You can also choose to override readFromFileWrapper:ofType:error: or readFromURL:ofType:error: instead.
    // If you override either of these, you should also override -isEntireFileLoaded to return false if the contents are lazily loaded.
    throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
  }

  func log(_ line:String) {
    if let view = self.textView {
      view.appendText(line:line)
    }
  }

  @IBAction func startButtonPressed(sender: NSButton){
    if sender.title == "Start" {
      sender.title = "Stop"
      hostField.isEnabled = false
      portField.isEnabled = false
      connectionSelector.isEnabled = false

      self.textView.textStorage!.setAttributedString(AttributedString(string:"", attributes: [:]))


      let address = hostField.stringValue + ":" + portField.stringValue
      if (connectionSelector.selectedSegment == 0) {
        startClient(address:address)
      } else {
        startServer(address:address)
      }
    } else {
      stopEverything()
    }
  }

  func stopEverything() {
    startButton.title = "Start"
    hostField.isEnabled = true
    portField.isEnabled = true
    connectionSelector.isEnabled = true

  }

  func startServer(address:String) {
    DispatchQueue.global(attributes: [.qosDefault]).async {
      self.log("GRPC version " + gRPC.version())
      do {
        let server = gRPC.Server(address:address)
        server.start()
        var running = true
        while(running) {
          let requestHandler = server.getNextRequest()
          self.log("HOST: " + requestHandler.host())
          self.log("METHOD: " + requestHandler.method())
          let initialMetadata = requestHandler.requestMetadata
          for i in 0..<initialMetadata.count() {
            self.log("INITIAL METADATA -> " + initialMetadata.key(index:i) + ":" + initialMetadata.value(index:i))
          }

          let (_, message) = requestHandler.receiveMessage()
          self.log("MESSAGE " + message!.string())
          if requestHandler.method() == "/quit" {
            running = false
          }
          let _ = requestHandler.sendResponse(message:ByteBuffer(string:"thank you very much!"))
        }
      }
      self.log("DONE")
    }
  }

  func startClient(address:String) {
    DispatchQueue.global(attributes: [.qosDefault]).async {
      let host = "foo.test.google.fr"
      let message = gRPC.ByteBuffer(string:"hello gRPC server!")

      self.log("GRPC version " + gRPC.version())

      do {
        let c = gRPC.Client(address:address)
        let steps = 30
        for i in 0..<steps {
          let method = (i < steps-1) ? "/hello" : "/quit"

          let metadata = Metadata(pairs:[MetadataPair(key:"x", value:"xylophone"),
                                         MetadataPair(key:"y", value:"yu"),
                                         MetadataPair(key:"z", value:"zither")])

          let response = c.performRequest(host:host,
                                          method:method,
                                          message:message,
                                          metadata:metadata)
          //self.log("status: " + response.status)
          self.log("statusDetails: " + response.statusDetails)
          if let message = response.message {
            self.log("message: " + message.string())
          }

          let initialMetadata = response.initialMetadata!
          for i in 0..<initialMetadata.count() {
            self.log("INITIAL METADATA -> " + initialMetadata.key(index:i) + " : " + initialMetadata.value(index:i))
          }

          let trailingMetadata = response.trailingMetadata!
          for i in 0..<trailingMetadata.count() {
            self.log("TRAILING METADATA -> " + trailingMetadata.key(index:i) + " : " + trailingMetadata.value(index:i))
          }

          if (response.status != 0) {
            break
          }
        }
      }
      self.log("DONE")
    }
  }

}
