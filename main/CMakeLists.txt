idf_component_register(
    SRCS "sonde_main.cpp"
    SRCS "libs/CSondeApp.cpp"
    SRCS "libs/sampler/ProbeSampler.cpp"
    SRCS "libs/sampler/CDummySampler.cpp"
    SRCS "libs/publisher/CBluetoothPublisherService.cpp"
    SRCS "libs/command/CCommandProcessor.cpp"
    SRCS "libs/command/CTestModeCommand.cpp"
    SRCS "libs/command/CRebootCommand.cpp"
    SRCS "libs/command/CListFilesCommand.cpp"
    SRCS "libs/command/CGetFileCommand.cpp"
    SRCS "libs/storage/CStorageService.cpp"
    INCLUDE_DIRS ""
    INCLUDE_DIRS "libs"
    INCLUDE_DIRS "libs/publisher"
    INCLUDE_DIRS "libs/sampler"
    INCLUDE_DIRS "libs/command"
    INCLUDE_DIRS "libs/storage"
)

set(EXTRA_COMPONENT_DIRS "${PROJECT_DIR}/components")

add_compile_options(-std=c++11)
