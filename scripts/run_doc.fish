#!/bin/fish

set BASE_DIR (dirname (readlink -m (status filename)))/..
set SCRIPTS_DIR (dirname (readlink -m (status filename)))

set doc_dir $BASE_DIR/doc

# where to install:
set install $doc_dir/install
set install_legacy $doc_dir/install_legacy


pd -noaudio \
	-noprefs \
	-path "$install" \
	"$doc_dir/sdScript_doc.pd"
