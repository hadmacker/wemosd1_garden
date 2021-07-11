#ifndef aws_creds_h
#define aws_creds_h

#define AWS_IOT_ENDPOINT "am37g1k3fe12l-ats.iot.us-east-1.amazonaws.com"
#define DEVICE_NAME "grasshopper"
#define AWS_IOT_PORT 8883
#define GREENGRASS_DISCOVERY_PORT 8443
#define AWS_IOT_TOPIC "$aws/things/" DEVICE_NAME "/shadow/update"
#define AWS_MAX_RECONNECT_TRIES 50

const char AWS_CERT_CA[] = "-----BEGIN CERTIFICATE-----\n"\
"-----END CERTIFICATE-----\n";

// The private key for your device
const char AWS_CERT_PRIVATE[] = "-----BEGIN RSA PRIVATE KEY-----\n"\
"-----END RSA PRIVATE KEY-----\n";


// The certificate for your device
const char AWS_CERT_CRT[] = "-----BEGIN CERTIFICATE-----\n"\
"-----END CERTIFICATE-----\n";

#endif