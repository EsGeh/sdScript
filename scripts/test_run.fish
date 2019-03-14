#!/bin/fish

set BASE_DIR (dirname (readlink -m (status filename)))/..
set SCRIPTS_DIR (dirname (readlink -m (status filename)))

set doc_dir $BASE_DIR/doc

pd -noaudio \
	-noprefs \
	-path "$doc_dir" \
	"$doc_dir/sdScript-help.pd"
