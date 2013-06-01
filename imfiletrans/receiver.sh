#!/bin/bash
(echo "recv" ; echo "$1") | nc localhost 3490
