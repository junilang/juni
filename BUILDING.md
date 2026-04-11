## Build Flags

- *BUILD_DEBUG* (bool)
- *BUILD_RELEASE* (bool) 
- *BUILD_SAFE* (bool) enabled by default, enables overflow and memory null checks
- *BUILD_TESTING* (bool) used with test generation
- *BUILD_ASAN* (bool) whether asan is enabled, currently unused
- *PTRTAG* (bool) whether pointer tagging is enabled, is automatically enabled on suitable platforms if undefined
- *IFACESPLIT* (bool) whether to split interfaces across two function arguments on native abi boundaries, enabled on windows automatically if undefined
