FROM ubuntu:latest

RUN apt-get update && \
    apt-get install -y build-essential cmake gdb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
WORKDIR /app
