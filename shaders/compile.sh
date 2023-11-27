# TODO: See libshaderc for runtime compilation

# https://github.com/google/shaderc/blob/main/downloads.md to download glslc

# /usr/local/bin/glslc simple_shader.vert -o simple_shader.vert.spv
# /usr/local/bin/glslc simple_shader.frag -o simple_shader.frag.spv
# /usr/local/bin/glslc point_light.vert -o point_light.vert.spv
# /usr/local/bin/glslc point_light.frag -o point_light.frag.spv

./glslc simple_shader.vert -o simple_shader.vert.spv
./glslc simple_shader.frag -o simple_shader.frag.spv
./glslc point_light.vert -o point_light.vert.spv
./glslc point_light.frag -o point_light.frag.spv

# glslang -V simple_shader.vert -o simple_shader.vert.spv
# glslang -V simple_shader.frag -o simple_shader.frag.spv
# glslang -V point_light.vert -o point_light.vert.spv
# glslang -V point_light.frag -o point_light.frag.spv