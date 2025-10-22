#!/usr/bin/env python3
import os, sys
cl = int(os.environ.get("CONTENT_LENGTH") or "0")
body = sys.stdin.read(cl) if cl > 0 else ""
print("Status: 200 OK")
print("Content-Type: text/plain")
print()
print("LEN=", cl)
print("BODY=", body)
