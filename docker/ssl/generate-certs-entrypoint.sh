#!/bin/sh
# Certificate generation script that runs inside alpine/openssl Docker container.
# This script is executed automatically by docker-compose when using the ssl profile.
#
# Creates:
#   - ca.pem / ca-key.pem          : CA certificate and key (PEM)
#   - broker.p12                    : Broker keystore (PKCS12, for ActiveMQ)
#   - broker-truststore.p12        : Broker truststore containing CA cert (PKCS12)
#
# All certificates are valid for 3650 days (10 years).
# The password for all keystores is "password".

set -e

CERT_DIR="/certs"
PASSWORD="password"

# Check if certificates already exist and are valid
if [ -f "${CERT_DIR}/ca.pem" ] && \
   [ -f "${CERT_DIR}/broker.p12" ] && \
   [ -f "${CERT_DIR}/broker-truststore.p12" ]; then

    # Check if CA certificate is still valid for at least 30 days
    if openssl x509 -in "${CERT_DIR}/ca.pem" -noout -checkend 2592000 2>/dev/null; then
        echo "=== Valid SSL certificates already exist, skipping generation ==="
        ls -lah "${CERT_DIR}/"
        exit 0
    else
        echo "=== Existing certificates expired or expiring soon, regenerating ==="
    fi
else
    echo "=== Certificates not found, generating new ones ==="
fi

# Clean up old/invalid certs
rm -rf "${CERT_DIR}"/*

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
echo "subjectAltName=DNS:localhost,IP:127.0.0.1" > "${CERT_DIR}/broker-ext.cnf"
openssl x509 -req \
    -in "${CERT_DIR}/broker.csr" \
    -CA "${CERT_DIR}/ca.pem" \
    -CAkey "${CERT_DIR}/ca-key.pem" \
    -CAcreateserial \
    -out "${CERT_DIR}/broker-cert.pem" \
    -days 3650 \
    -extfile "${CERT_DIR}/broker-ext.cnf"

echo "=== Creating broker PKCS12 keystore ==="
openssl pkcs12 -export \
    -in "${CERT_DIR}/broker-cert.pem" \
    -inkey "${CERT_DIR}/broker-key.pem" \
    -certfile "${CERT_DIR}/ca.pem" \
    -out "${CERT_DIR}/broker.p12" \
    -name broker \
    -password "pass:${PASSWORD}"

echo "=== Creating broker truststore (PKCS12 with CA cert) ==="
openssl pkcs12 -export \
    -nokeys \
    -in "${CERT_DIR}/ca.pem" \
    -out "${CERT_DIR}/broker-truststore.p12" \
    -name ca \
    -password "pass:${PASSWORD}"

echo "=== Cleaning up intermediate files ==="
rm -f "${CERT_DIR}/broker.csr" \
      "${CERT_DIR}/ca.srl" \
      "${CERT_DIR}/broker-ext.cnf"

echo "=== Certificate generation complete ==="
echo "Generated files:"
ls -lah "${CERT_DIR}/"
echo ""
echo "CA certificate:      ${CERT_DIR}/ca.pem"
echo "Broker keystore:     ${CERT_DIR}/broker.p12"
echo "Broker truststore:   ${CERT_DIR}/broker-truststore.p12"
echo ""
echo "All keystores use password: ${PASSWORD}"
