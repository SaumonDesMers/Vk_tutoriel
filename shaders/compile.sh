# TODO: See libshaderc for runtime compilation

# https://storage.googleapis.com/shaderc/badges/build_link_linux_clang_release.html to download glslc

/usr/local/bin/glslc shader.vert -o vert.spv
/usr/local/bin/glslc shader.frag -o frag.spv