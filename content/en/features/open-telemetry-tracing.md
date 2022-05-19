---
title: 'Open Telemetry Tracing'
description: 'How to trace request through Hub?'
date: '2022-05-19'
lastmod: '2022-05-19'
categories: [features]
keywords: [opentelemetry, tracing]
menu:
  docs:
    parent: features
    weight: 30
toc: true
---

Traces track the progression of a single request, called a trace, as it is handled by services that make up an application. The request may be initiated by a user or an application. Distributed tracing is a form of tracing that traverses process, network and security boundaries. Each unit of work in a trace is called a span; a trace is a tree of spans. Spans are objects that represent the work being done by individual services or components involved in a request as it flows through a system. A span contains a span context, which is a set of globally unique identifiers that represent the unique request that each span is a part of. A span provides Request, Error and Duration (RED) metrics that can be used to debug availability as well as performance issues.

A trace contains a single root span which encapsulates the end-to-end latency for the entire request. You can think of this as a single logical operation, such as clicking a button in a web application to add a product to a shopping cart. The root span would measure the time it took from an end-user clicking that button to the operation being completed or failing (so, the item is added to the cart or some error occurs) and the result being displayed to the user. A trace is comprised of the single root span and any number of child spans, which represent operations taking place as part of the request. Each span contains metadata about the operation, such as its name, start and end timestamps, attributes, events, and status.

To create and manage spans in OpenTelemetry, the OpenTelemetry API provides the tracer interface. This object is responsible for tracking the active span in your process, and allows you to access the current span in order to perform operations on it such as adding attributes, events, and finishing it when the work it tracks is complete. One or more tracer objects can be created in a process through the tracer provider, a factory interface that allows for multiple tracers to be instantiated in a single process with different options.

Generally, the lifecycle of a span resembles the following:

- A request is received by a service. The span context is extracted from the request headers, if it exists.
- A new span is created as a child of the extracted span context; if none exists, a new root span is created.
- The service handles the request. Additional attributes and events are added to the span that are useful for understanding the context of the request, such as the hostname of the machine handling the request, or customer identifiers.
- New spans may be created to represent work being done by sub-components of the service.
- When the service makes a remote call to another service, the current span context is serialized and forwarded to the next service by injecting the span context into the headers or message envelope.
- The work being done by the service completes, successfully or not. The span status is appropriately set, and the span is marked finished.
- For more information, see the [traces specification](https://opentelemetry.io/docs/reference/specification/overview/#tracing-signal), which covers concepts including: trace, span, parent/child relationship, span context, attributes, events and links.

This example shows open telemetry tracing in action:
![](/images/open-telemetry-tracing/opentelemetry-update-resource.png)

[more info](https://opentelemetry.io/docs)

## Open Telemetry collector

The OpenTelemetry Collector offers a vendor-agnostic implementation on how to receive, process and export telemetry data.

Plgd services requires to have a collector with otlp protocol secured by TLS.

[Helm chart](https://artifacthub.io/packages/helm/opentelemetry-helm/opentelemetry-collector)

### Basic config for Open Telemetry collector

```yaml
receivers:
  otlp:
    protocols:
      grpc:
        endpoint: 0.0.0.0:4317
        tls:
          # Set if you want to verify the client certificate
          # client_ca_file: certs/root_ca.crt
          cert_file: certs/cert.crt
          key_file: certs/cert.key
exporters:
  otlp:
    endpoint: tempo:4317
    # tls:
      # Set if the client does not verify the server certificate
      # insecure: true
service:
  pipelines:
    traces:
      receivers: [otlp]
      exporters: [otlp]
```

## Setup Plgd services

To enable tracing in plgd services, you need to set the following variables in the plgd helm chart:

```yaml
global:
  openTelemetryExporter:
    # -- Enable OTLP gRPC exporter
    enabled: false
    # -- The gRPC collector to which the exporter is going to send data
    address:
    # -- Expoter keep alive configuration
    keepAlive:
      time: 10s
      timeout: 20s
      permitWithoutStream: true
    # -- Exporter TLS configuration
    tls:
      caPool:
      keyFile:
      certFile:
      useSystemCAPool: false
```

## Example setup clients and servers in golang

### Initialize tracer

```go
package test

import (
  "context"

  "google.golang.org/grpc"

  "go.opentelemetry.io/otel"
  "go.opentelemetry.io/otel/exporters/otlp/otlptrace/otlptracegrpc"
  "go.opentelemetry.io/otel/propagation"
  "go.opentelemetry.io/otel/sdk/resource"
  sdktrace "go.opentelemetry.io/otel/sdk/trace"
  semconv "go.opentelemetry.io/otel/semconv/v1.10.0"
)

func initTracer(ctx context.Context) *sdktrace.TracerProvider {
  res, err := resource.New(ctx,
    resource.WithAttributes(
      // the service name used to display traces in backends
      semconv.ServiceNameKey.String("myService"),
    ),
  )
  if err != nil {
    panic(err)
  }
  // dial to otel collector
  conn, err := grpc.Dial("otel-collector:4317")
  if err != nil {
    panic(err)
  }
  // Set up a trace exporter
  traceExporter, err := otlptracegrpc.New(ctx, otlptracegrpc.WithGRPCConn(conn))
  if err != nil {
    _ = conn.Close()
    panic(err)
  }

  // Register the trace exporter with a TracerProvider, using a batch
  // span processor to aggregate spans before export.
  bsp := sdktrace.NewBatchSpanProcessor(traceExporter)
  tracerProvider := sdktrace.NewTracerProvider(
    sdktrace.WithSampler(sdktrace.AlwaysSample()),
    sdktrace.WithResource(res),
    sdktrace.WithSpanProcessor(bsp),
  )

  // set global trace provider
  otel.SetTracerProvider(tracerProvider)
  // set global propagator to tracecontext (the default is no-op).
  otel.SetTextMapPropagator(propagation.NewCompositeTextMapPropagator(propagation.TraceContext{}, propagation.Baggage{}))
  return tracerProvider
}
```

### Setup HTTP Client

```go
package test

import (
  "context"
  "fmt"
  "io/ioutil"
  "log"
  "net/http"

  "go.opentelemetry.io/contrib/instrumentation/net/http/otelhttp"
)

func main() {
  tp := initTracer(context.Background())
  defer func() {
    if err := tp.Shutdown(context.Background()); err != nil {
      log.Printf("Error shutting down tracer provider: %v", err)
    }
  }()

  c := http.Client{Transport: otelhttp.NewTransport(http.DefaultTransport)}
  req, _ := http.NewRequestWithContext(context.Background(), "GET", "http://localhost:7777/hello", nil)
  res, err := c.Do(req)
  if err != nil {
    panic(err)
  }
  defer res.Body.Close()
  body, err := ioutil.ReadAll(res.Body)
  if err != nil {
    panic(err)
  }
  fmt.Printf("Response Received: %s\n\n\n", body)
}
```

### Setup HTTP Server

```go
package main

import (
  "context"
  "io"
  "log"
  "net/http"

  "github.com/plgd-dev/hub/v2/pkg/opentelemetry/otelhttp"
)

func main() {
  tp := initTracer(context.Background())
  defer func() {
    if err := tp.Shutdown(context.Background()); err != nil {
      log.Printf("Error shutting down tracer provider: %v", err)
    }
  }()
  helloHandler := func(w http.ResponseWriter, req *http.Request) {
    _, _ = io.WriteString(w, "Hello, world!\n")
  }
  otelHandler := otelhttp.NewHandler(http.HandlerFunc(helloHandler), "Hello")
  http.Handle("/hello", otelHandler)
  err := http.ListenAndServe(":7777", nil)
  if err != nil {
    panic(err)
  }
}
```

### Setup GRPC Client

```go
package main

import (
  "context"
  "log"

  "go.opentelemetry.io/contrib/instrumentation/google.golang.org/grpc/otelgrpc"
  "google.golang.org/grpc"
)

func main() {
  tp := initTracer(context.Background())
  defer func() {
    if err := tp.Shutdown(context.Background()); err != nil {
      log.Printf("Error shutting down tracer provider: %v", err)
    }
  }()

  conn, err := grpc.Dial("localhost:7777",
    grpc.WithUnaryInterceptor(otelgrpc.UnaryClientInterceptor()),
    grpc.WithStreamInterceptor(otelgrpc.StreamClientInterceptor()),
    ... // setup credentials, jwt token
  )
  if err != nil {
    panic(err)
  }
  defer conn.Close()
  // c := api.NewHelloServiceClient(conn)
  ...
```

### Setup GRPC Server

```go
package main

import (
  "context"
  "log"
  "net"

  "go.opentelemetry.io/contrib/instrumentation/google.golang.org/grpc/otelgrpc"
  "google.golang.org/grpc"
)

func main() {
  tp := initTracer(context.Background())
  defer func() {
    if err := tp.Shutdown(context.Background()); err != nil {
      log.Printf("Error shutting down tracer provider: %v", err)
    }
  }()

  lis, err := net.Listen("tcp", ":7777")
  if err != nil {
    panic(err)
  }
  s := grpc.NewServer(
    grpc.UnaryInterceptor(otelgrpc.UnaryServerInterceptor()),
    grpc.StreamInterceptor(otelgrpc.StreamServerInterceptor()),
    // setup credentials, setup authorization for jwt token
  )

  //api.RegisterHelloServiceServer(s, &server{})
  if err := s.Serve(lis); err != nil {
    log.Fatalf("failed to serve: %v", err)
  }
}
```
