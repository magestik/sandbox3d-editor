#!/bin/sh

mkdir -p data/shaders/


FRAG=src/Rendering/Shader/shaders/*.frag
VERT=src/Rendering/Shader/shaders/*.vert

DEST=data/shaders/

for f in $FRAG $VERT
do
	name=$(basename "$f")
	cpp -P -x c++ -nostdinc -nostdinc++ $f | sed 's/^#pragma GLSL(\(.*\)) \(.*\)$/#\1 \2/' > $DEST/$name
done

cd build
ctest check_shaders -VV
