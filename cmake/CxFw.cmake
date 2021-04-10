set(CXFW_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(CXFW_SOURCE_DIR "${CXFW_ROOT}/src")
set(CXFW_DIST_DIR "${CXFW_ROOT}/dist")
set(CXFW_BINARY_DIR "${CXFW_DIST_DIR}/bin")

set(CMAKE_INSTALL_PREFIX "${CXFW_DIST_DIR}")

list(APPEND CMAKE_PREFIX_PATH "${CXFW_DIST_DIR}/cmake")