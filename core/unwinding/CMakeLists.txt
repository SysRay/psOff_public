enable_language(C ASM_MASM)
add_library(unwinding OBJECT
  unwind.asm
)

file(COPY unwind.h DESTINATION ${DST_INCLUDE_DIR}/imports/unwinding)