#!/bin/bash

curl --location https://github.com/firebase/firebase-ios-sdk/releases/download/6.10.0/Firebase-6.10.0.zip --output "firebase_ios.zip"
unzip firebase_ios.zip
rm firebase_ios.zip
