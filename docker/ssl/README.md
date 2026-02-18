# SSL Certificate Generation for ActiveMQ Testing

This directory contains scripts to generate self-signed SSL certificates for ActiveMQ integration testing.

## Quick Start

Start the SSL broker with automatic certificate generation:

```bash
docker compose --profile ssl up
```

That's it! The `ssl-cert-generator` service will:
1. Check if valid certificates already exist
2. Generate new certificates if needed (or if they're expiring soon)
3. Start ActiveMQ SSL broker on `ssl://localhost:61617`

**Prerequisites:**
- Docker Desktop (Windows/macOS) or Docker Engine (Linux)
- Docker Compose V2

**Tip:** See [QUICKSTART.md](QUICKSTART.md) for just the essential commands.

## Generated Files

The scripts create the following files in the `certs/` directory:

| File | Description | Used By |
|------|-------------|---------|
| `ca.pem` | CA certificate (PEM format) | C++ client for trust verification |
| `ca-key.pem` | CA private key | Certificate signing (not used in tests) |
| `broker.p12` | Broker keystore (PKCS12) | ActiveMQ broker (server certificate) |
| `broker-truststore.p12` | Broker truststore (PKCS12) | ActiveMQ broker (CA trust) |
| `broker-cert.pem` | Broker certificate (PEM) | Intermediate file |
| `broker-key.pem` | Broker private key (PEM) | Intermediate file |

**Password:** All keystores use the password `password`

**Validity:** All certificates are valid for 3650 days (10 years)

**Subject Alternative Names:** The broker certificate includes:
- DNS: `localhost`
- IP: `127.0.0.1`

## How It Works

When you run `docker compose --profile ssl up`:

1. The `ssl-cert-generator` service starts first using the `alpine/openssl` image
2. It runs [generate-certs-entrypoint.sh](generate-certs-entrypoint.sh) inside the container
3. The script checks if valid certificates exist (valid for at least 30 more days)
4. If certificates are missing or expiring, it generates new ones
5. Once complete, the service exits successfully
6. The `activemq-ssl` service starts only after certificate generation succeeds
7. ActiveMQ loads the certificates from the shared `certs/` volume

## Common Operations

### Viewing Logs

```bash
# View certificate generator logs
docker compose --profile ssl logs ssl-cert-generator

# View ActiveMQ SSL broker logs
docker compose --profile ssl logs activemq-ssl

# Follow logs in real-time
docker compose --profile ssl logs -f
```

### Stopping the Broker

```bash
# Stop and remove containers
docker compose --profile ssl down

# Stop but keep containers
docker compose --profile ssl stop
```

### Regenerating Certificates

Certificates are valid for 10 years and automatically regenerate when:
- They don't exist
- They're expiring within 30 days

To force regeneration:

```bash
# Stop the broker
docker compose --profile ssl down

# Delete certificates
rm -rf docker/ssl/certs/*.pem docker/ssl/certs/*.p12
# On Windows PowerShell: Remove-Item docker/ssl/certs/*.pem,docker/ssl/certs/*.p12

# Restart (will regenerate automatically)
docker compose --profile ssl up
```

### Running Tests

The SSL integration tests provide comprehensive validation of the SSL/TLS transport layer (29 tests total, mirroring all OpenWire test functionality).

```bash
# Build tests
cmake --build --preset x86-windows-debug-test

# Run all SSL integration tests (29 tests)
SSL_CERT_FILE=docker/ssl/certs/ca.pem ctest --preset x86-windows-debug-test -L integration-openwire-ssl --output-on-failure

# Or run all integration tests (OpenWire + OpenWire SSL + STOMP)
ctest --preset x86-windows-debug-test -L integration --output-on-failure
```

**SSL Test Coverage:**
- ✅ All acknowledgment modes (client, individual, optimized)
- ✅ Advisory messages
- ✅ Async sending and callbacks
- ✅ Message selectors and groups
- ✅ Durable subscriptions
- ✅ Transactions (local and XA)
- ✅ Redelivery policies and session recovery
- ✅ Temporary destinations
- ✅ Message compression and priority
- ✅ Queue browsing and virtual topics
- ✅ Slow consumers and expiration
- ✅ Enhanced connection features

## Troubleshooting

### "No such file or directory" error
Make sure you're running from the project root directory (where `docker-compose.yml` is located).

### Docker not found
Install Docker Desktop and ensure it's running:
```bash
docker --version
docker ps
```

### Certificate validation failures in tests
1. Check certificates were generated: `ls docker/ssl/certs/`
2. Verify broker is running: `docker compose --profile ssl ps`
3. Check broker logs: `docker compose --profile ssl logs activemq-ssl`
4. Verify certificate expiry: `openssl x509 -in docker/ssl/certs/ca.pem -noout -dates`

### Port already in use (61617)
Another service is using port 61617. Change the port mapping in `docker-compose.yml`:
```yaml
ports:
  - "127.0.0.1:61627:61617"  # Map to different local port
```

## Integration with Tests

The generated certificates are used by:

1. **ActiveMQ Broker** (via `activemq-ssl.xml`):
   - Keystore: `broker.p12`
   - Truststore: `broker-truststore.p12`

2. **C++ Client Tests** (via SSL tests):
   - Trust certificate: `ca.pem`

## Security Notes

⚠️ **These certificates are for testing only!**

- Self-signed certificates should never be used in production
- The keystore password is hardcoded and publicly known
- Certificates are checked into source control
- No certificate revocation mechanism exists

For production use, obtain certificates from a trusted Certificate Authority (CA).
