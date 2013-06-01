#!/bin/bash
(echo "send" ; cat $1) | nc localhost 3490
