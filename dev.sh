#!/bin/bash

(cd libs/lib1 && make)
(cd libs/lib2 && make)
(cd libs/lib3 && make) || true
(cd client && make)
(cd client && ./client)

