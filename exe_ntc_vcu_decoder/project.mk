THIS_EXE_NTC_VCU_DECODER := $(call get-my-dir)

EXE_NTC_VCU_DECODER_SRC := \
  $(THIS_EXE_NTC_VCU_DECODER)/main.cpp \
  exe_decoder/SinkYuvCrc.cpp \
  exe_decoder/CmdParser.cpp \
  exe_decoder/IpDevice.cpp \
  exe_decoder/CodecUtils.cpp \
  exe_decoder/InputLoader.cpp

ifneq ($(ENABLE_DEC_SW_HIGH_DYNAMIC_RANGE),0)
  EXE_NTC_VCU_DECODER_SRC += exe_decoder/HDRWriter.cpp
endif

EXE_NTC_VCU_DECODER_OBJ := \
  $(EXE_NTC_VCU_DECODER_SRC:%=$(BIN)/%.o)

# The custom main.cpp reuses headers located in exe_decoder/.
$(BIN)/$(THIS_EXE_NTC_VCU_DECODER)/main.cpp.o: CFLAGS += -Iexe_decoder

$(BIN)/NTC_VcuDecoder.exe: \
  $(EXE_NTC_VCU_DECODER_OBJ) \
  $(LIB_REFDEC_A) \
  $(LIB_REFALLOC_A) \
  $(LIB_DECODER_A) \
  $(LIB_APP_A) \
  $(LIB_REFFBC_A) \
  $(LIB_REF_LCEVC_DEC_A) \
  $(LIB_LCEVC_DECODE_A)

NTC_VcuDecoder.exe: $(BIN)/NTC_VcuDecoder.exe
TARGETS += NTC_VcuDecoder.exe

.PHONY: NTC_VcuDecoder.exe
