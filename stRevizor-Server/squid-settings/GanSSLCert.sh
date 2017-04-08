#!/bin/bash

openssl req -new -newkey rsa:1024 -days 3650 -nodes -x509 -keyout squidCA.pem -out squidCA.pem;
openssl x509 -in squidCA.pem -outform DER -out squidCA.der
