import gRPC

gRPC.initialize()
print("gRPC version", gRPC.version())
do {
  let server = gRPC.Server(address:"localhost:8001")
  server.start()
  var running = true
  while(running) {
    let (_, status, requestHandler) = server.getNextRequest(timeout:600)
    if let requestHandler = requestHandler {
      print("HOST:", requestHandler.host())
      print("METHOD:", requestHandler.method())
      let initialMetadata = requestHandler.requestMetadata
      for i in 0..<initialMetadata.count() {
        print("INITIAL METADATA ->", initialMetadata.key(index:i), ":", initialMetadata.value(index:i))
      }
  
      let (_, _, message) = requestHandler.receiveMessage()
      print("MESSAGE", message!.string())
      if requestHandler.method() == "/quit" {
        running = false
      }
      let (_, _) = requestHandler.sendResponse(message:ByteBuffer(string:"thank you very much!"))
    }
  }
}
gRPC.shutdown()
print("DONE")
