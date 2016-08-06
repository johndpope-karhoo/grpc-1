import PackageDescription
let package = Package (
    name: "gRPC",
    dependencies: [
        .Package(url: "../gRPC_Core", majorVersion:1),
    ]
)
