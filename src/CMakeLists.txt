include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG        v3.13.2
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(entt)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(raylib)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(space_invaders main.cpp)

target_compile_options(space_invaders
    PRIVATE
      -g
      $<$<CONFIG:Debug>:-Wall>
      $<$<CONFIG:Debug>:-Wextra>
      $<$<CONFIG:Debug>:-pedantic>
)

target_compile_definitions(space_invaders
    PRIVATE
      $<$<CONFIG:Debug>:DEBUG_BUILD>
)

target_link_libraries(space_invaders PRIVATE raylib EnTT::EnTT)
