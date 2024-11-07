idf_component_register(
    SRCS "sonde_main.cpp"
    SRCS "libs/CSondeApp.cpp"
    SRCS "libs/sampler/CDummySampler.cpp"
    SRCS "libs/publisher/CBluetoothPublisherService.cpp"
    INCLUDE_DIRS ""
    INCLUDE_DIRS "libs"
    INCLUDE_DIRS "libs/publisher"
    INCLUDE_DIRS "libs/sampler"
)

add_compile_options(-std=c++11)
