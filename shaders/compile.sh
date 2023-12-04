# https://github.com/google/shaderc/blob/main/downloads.md to download glslc

# /usr/local/bin/glslc simple_shader.vert -o simple_shadervert.spv
# /usr/local/bin/glslc simple_shader.frag -o simple_shaderfrag.spv

glslang -V simple_shader.vert -o simple_shadervert.spv
glslang -V simple_shader.frag -o simple_shaderfrag.spv