idf_component_register(
    SRCS "sonde_main.cpp"
    SRCS "libs/CSondeApp.cpp"
    SRCS "libs/sampler/ProbeSampler.cpp"
    SRCS "libs/sampler/CDummySampler.cpp"
    SRCS "libs/publisher/CBluetoothPublisherService.cpp"
    SRCS "libs/command/CCommandProcessor.cpp"
    SRCS "libs/command/CTestModeCommand.cpp"
    INCLUDE_DIRS ""
    INCLUDE_DIRS "libs"
    INCLUDE_DIRS "libs/publisher"
    INCLUDE_DIRS "libs/sampler"
    INCLUDE_DIRS "libs/command"
)

set(EXTRA_COMPONENT_DIRS "${PROJECT_DIR}/components")

add_compile_options(-std=c++11)
