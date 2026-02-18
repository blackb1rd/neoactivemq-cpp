# SSL Testing Quick Start

## Start ActiveMQ with SSL

```bash
docker compose --profile ssl up
```

Certificates are generated automatically. The SSL broker will be available at `ssl://localhost:61617`

## Run SSL Tests

```bash
cmake --build --preset x86-windows-debug-test
ctest --preset x86-windows-debug-test -R SSL
```

## Stop the Broker

```bash
docker compose --profile ssl down
```

## Need More Info?

See [README.md](README.md) for detailed documentation, troubleshooting, and advanced usage.
