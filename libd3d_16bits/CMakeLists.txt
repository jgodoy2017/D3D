# declare additional include directories
INCLUDE_DIRECTORIES("${dcmd3d_SOURCE_DIR}/libd3d" "${ofstd_SOURCE_DIR}/include")

# create library from source files
DCMTK_ADD_LIBRARY(d3d Intrface Writer2 Coder Reader2 Context ContextRun Decoder Image CodedImage Racha)

DCMTK_TARGET_LINK_MODULES(d3d ofstd oflog)
