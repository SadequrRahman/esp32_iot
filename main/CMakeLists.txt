# Edit following two lines to set component requirements (see docs)


set(src 
"app.c"
"heartBeat.c"
"custom_list_iterator.c"
"custom_list_node.c"
"custom_list.c"
"wifi_driver.c"
"mqtt_manager.c"
"MessageQueue.c"
)
idf_component_register(SRCS "${src}" 
                    INCLUDE_DIRS "." )
include_directories(../inc) 