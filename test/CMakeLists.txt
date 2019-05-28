message("Adding a test")

add_test(NAME basictest COMMAND shademachine -I "." --output "." WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")