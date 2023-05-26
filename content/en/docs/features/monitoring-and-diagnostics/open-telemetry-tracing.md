---
title: 'Open Telemetry Tracing'
description: 'How to trace request through plgd hub?'
docsOthersDisplay: true
date: '2022-05-19'
categories: [features]
keywords: [opentelemetry, tracing]
weight: 60
---

Traces track the progression of a single request, called a trace. The request may be initiated by a user or an application. Distributed tracing is a form of tracing that traverses process, network and security boundaries. Each unit of work in a trace is called a span; a trace is a tree of spans. Spans are objects that represent the work being done by individual services or components involved in a request as it flows through a system. A span contains a span context, which is a set of globally unique identifiers that represent the unique request that each span is a part of. A span provides Request, Error and Duration (RED) metrics that can be used to debug availability as well as performance issues.

> For more information, see the [traces specification](https://opentelemetry.io/docs/reference/specification/overview/#tracing-signal), which covers concepts including: trace, span, parent/child relationship, span context, attributes, events and links.

This example shows open telemetry tracing in action:
![opentelemetry-update-resource](/docs/features/control-plane/static/opentelemetry-update-resource.png)

## plgd & Open Telemetry

The plgd hub services emit telemetry to collectors, secured using TLS and supporting otlp encoding. The open telemetry integration can be enabled globally for each in the [plgd hub helm chart](https://plgd.dev/deployment/k8s/#register-plgd-helm-chart-registry). Read further for more information on how to enable open telemetry in plgd hub helm chart.

> The request content is included the gRPC as well as CoAP Gateway spans. As the HTTP Gateway is the proxy of the gRPC Gateway, the request content can be found in the gRPC Gateway spans.

### Setup Open Telemetry collector

Interested how to deploy OpenTelemetry Collector? Read more [here](https://artifacthub.io/packages/helm/opentelemetry-helm/opentelemetry-collector).

> Don't forget to enable TLS!

```yaml
receivers:
  otlp:
    protocols:
      grpc:
        endpoint: 0.0.0.0:55680
        tls:
          # Set if you want to verify the client certificate
          # client_ca_file: certs/root_ca.crt
          cert_file: certs/cert.crt
          key_file: certs/cert.key
```

### Setup plgd hub

To enable tracing in plgd services, you need to set the following variables in the plgd helm chart:

```yaml
global:
  openTelemetryExporter:
    enabled: true
    address: grafana-agent.demo.svc.cluster.local:55680
```

> Service certificates (those which are used to secure internal communication between plgd hub services) are reused to secure the communication with the collector.

## Example clients and servers in golang

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
