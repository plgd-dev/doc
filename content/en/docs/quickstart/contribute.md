---
title: 'Contribute'
description: 'Contribute to development'
date: '2024-05-10'
categories: [quickstart]
keywords: [quickstart, development]
weight: 5
---
This guide will describe how to setup your environment to enable contribution to development.

## Development on Linux (Ubuntu 20.02+ LTS)

### Install Go

a. Follow official [golang installation guide](https://golang.org/doc/install).
b. Use [update-golang script](https://github.com/udhos/update-golang).

Both options install golang to `/usr/local/go`. For everything to work correctly you must add path to go to your environment PATH variable. Add the following line to your `$HOME/.profile` file.

```shell
export PATH=$PATH:/usr/local/go/bin
```

(Restart may be needed for the changes in the `.profile` file to be applied). To test that go is setup correctly run in terminal:

```shell
go version
```

The command should succeed and print out the currently installed version of go.

### Checkout hub in your go workspace and install go modules dependencies

Some parts of the `hub` repository are sensitive to location of the code. The source code of the repository must reside in your go workspace, at `$HOME/go/src/plgd-dev/hub`.

Manually create the folder structure and use `git clone --recursive` to download source code and all dependencies:

```shell
cd $HOME/go
mkdir -p src/github.com/plgd-dev
cd src/github.com/plgd-dev
git clone --recursive git@github.com:plgd-dev/hub.git
```

After a successful checkout use `go mod download` and `go mod tidy` commands to download dependencies.

```shell
cd $HOME/go/src/plgd-dev/hub
go mod download
go mod tidy
```

### Install Make and other building tools

Execute the following command in terminal:

```shell
sudo apt install build-essential
```

### Install Docker

Follow official [docker installation guide](https://docs.docker.com/engine/install/ubuntu/).

Additionally, after succesfull docker installation you must allow non-root users to use docker. To do so follow [Manage Docker as a non-root user](https://docs.docker.com/engine/install/linux-postinstall/) guide.

### Install protobuf compiler

Services use protobuf messages to communicate. To generate `.pb` files from `.proto` files `protoc` binary is used in Makefiles. To install latest protoc do the following steps:

1. Download latest protoc `.zip` from <https://github.com/protocolbuffers/protobuf/releases>
2. The `.zip` file contains two folders: `bin` and `include`.
 a) move the contents of the extracted `bin` folder to `/usr/local/bin`
 b) move the contents of the extracted `include` folder to `/usr/local/include`
3. Set permissions of the copied files and folders by chmod to `0755` (read+write+execute for owner, read+execute for group and world)

These steps are automated in convenience script `hub/tools/install/install-latest-protoc.py`.

### Install protoc modules to generate protobuf definitions for gRPC-Gateway

To automatically generate API clients and server stubs in golang protoc plugins from [gRPC-Gateway](https://github.com/grpc-ecosystem/grpc-gateway) are used. To install them run:

```shell
go install github.com/grpc-ecosystem/grpc-gateway/v2/protoc-gen-grpc-gateway@latest;
go install github.com/grpc-ecosystem/grpc-gateway/v2/protoc-gen-openapiv2@latest;
go install google.golang.org/protobuf/cmd/protoc-gen-go@latest;
go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest;
go install github.com/favadi/protoc-go-inject-tag@latest;
go install github.com/pseudomuto/protoc-gen-doc/cmd/protoc-gen-doc@latest;
```

### Instal yq to set up environment

`yq` is a tool for manipulating YAML files. It is used in Makefiles to set up environment variables. To install `yq` run:

```shell
go install github.com/mikefarah/yq/v4@latest
```

### Setup sysctl

Set up sysctl to allow for more inotify watches and aio requests. Add the following lines to `/etc/sysctl.conf`:

```txt
fs.inotify.max_user_watches = 5242880
fs.aio-max-nr = 1048576
```

### Test your environment

Your system should now be fully setup and capable of compiling all of the code in the repository.

Test go compilation by running unit tests:

```shell
make test
```

The command should succeed and execute all available unit test in the repository.

Test protobuf generation:

```shell
make proto/generate
```

To setup development environment run the following command:

```shell
make env
```

To clean development environment run the following command:

```shell
make clean
```
