#!/bin/sh
protoc -I=doc/ --go_out=slg/pb/ doc/cdragon.proto
