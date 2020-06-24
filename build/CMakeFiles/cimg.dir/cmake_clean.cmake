file(REMOVE_RECURSE
  "libcimg.pdb"
  "libcimg.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/cimg.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
