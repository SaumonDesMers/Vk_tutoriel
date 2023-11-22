# TODO: See libshaderc for runtime compilation

# https://github.com/google/shaderc/blob/main/downloads.md to download glslc

# /usr/local/bin/glslc shader.vert -o vert.spv
# /usr/local/bin/glslc shader.frag -o frag.spv

./glslc shader.vert -o vert.spv
./glslc shader.frag -o frag.spv

# glslang -V shader.vert -o vert.spv
# glslang -V shader.frag -o frag.spv