FROM ubuntu:20.04

COPY ./target/debug/server /server
COPY ./lib/libcurl.so /usr/lib/libcurl.so

RUN apt update
RUN apt install -y libssl-dev

EXPOSE 8001/udp

CMD ["/bin/bash", "-c", "/server"]