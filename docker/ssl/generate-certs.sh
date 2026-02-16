#!/bin/bash
# Generate self-signed certificates for ActiveMQ SSL integration testing.
#
# Creates:
#   - ca.pem / ca-key.pem          : CA certificate and key (PEM)
#   - broker.p12                    : Broker keystore (PKCS12, for ActiveMQ)
#   - broker-truststore.p12        : Broker truststore containing CA cert (PKCS12)
#   - ca.pem                        : CA certificate for C++ client trust
#
# All certificates are valid for 3650 days (10 years) to avoid expiry issues in CI.
# The password for all keystores is "password".

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CERT_DIR="${SCRIPT_DIR}/certs"

# Keystore password
PASSWORD="password"

# Clean up old certs
rm -rf "${CERT_DIR}"
mkdir -p "${CERT_DIR}"

echo "=== Generating CA key and certificate ==="
openssl req -x509 -newkey rsa:2048 -nodes \
    -keyout "${CERT_DIR}/ca-key.pem" \
    -out "${CERT_DIR}/ca.pem" \
    -days 3650 \
    -subj "/CN=ActiveMQ Test CA/O=Test/C=US"

echo "=== Generating broker key and certificate signing request ==="
openssl req -newkey rsa:2048 -nodes \
    -keyout "${CERT_DIR}/broker-key.pem" \
    -out "${CERT_DIR}/broker.csr" \
    -subj "/CN=localhost/O=Test/C=US"

echo "=== Signing broker certificate with CA ==="
openssl x509 -req \
    -in "${CERT_DIR}/broker.csr" \
    -CA "${CERT_DIR}/ca.pem" \
    -CAkey "${CERT_DIR}/ca-key.pem" \
    -CAcreateserial \
    -out "${CERT_DIR}/broker-cert.pem" \
    -days 3650 \
    -extfile <(printf "subjectAltName=DNS:localhost,IP:127.0.0.1")

echo "=== Creating broker PKCS12 keystore ==="
openssl pkcs12 -export \
    -in "${CERT_DIR}/broker-cert.pem" \
    -inkey "${CERT_DIR}/broker-key.pem" \
    -certfile "${CERT_DIR}/ca.pem" \
    -out "${CERT_DIR}/broker.p12" \
    -name broker \
    -password "pass:${PASSWORD}"

echo "=== Creating broker truststore (PKCS12 with CA cert) ==="
keytool -importcert -noprompt \
    -alias ca \
    -file "${CERT_DIR}/ca.pem" \
    -storetype PKCS12 \
    -keystore "${CERT_DIR}/broker-truststore.p12" \
    -storepass "${PASSWORD}" 2>/dev/null || {
    # Fallback if keytool is not available: create PKCS12 truststore with openssl
    # This creates a PKCS12 file with just the CA certificate (no private key)
    openssl pkcs12 -export \
        -nokeys \
        -in "${CERT_DIR}/ca.pem" \
        -out "${CERT_DIR}/broker-truststore.p12" \
        -name ca \
        -password "pass:${PASSWORD}"
}

# Clean up intermediate files
rm -f "${CERT_DIR}/broker.csr" "${CERT_DIR}/ca.srl"

echo "=== Certificate generation complete ==="
echo "Files in ${CERT_DIR}:"
ls -la "${CERT_DIR}/"
echo ""
echo "CA certificate (for C++ client):  ${CERT_DIR}/ca.pem"
echo "Broker keystore (for ActiveMQ):   ${CERT_DIR}/broker.p12"
echo "Broker truststore (for ActiveMQ): ${CERT_DIR}/broker-truststore.p12"
