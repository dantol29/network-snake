FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get install -y --no-install-recommends g++ make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY server /app

RUN make clean
RUN make

RUN chmod +x /app/nibbler

EXPOSE 8080

CMD ["/app/nibbler", "50", "50"]

