#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src
CXXFLAGS += -DST7735_DRIVER=1 -DST7735_GREENTAB2=1 -DTFT_WIDTH=128 -DTFT_HEIGHT=160 -DTFT_DC=2 -DTFT_RST=4 -DTFT_CS=15 -DTOUCH_CS=21 -DSPI_TOUCH_FREQUENCY=2500000 -DUSER_SETUP_LOADED=1
