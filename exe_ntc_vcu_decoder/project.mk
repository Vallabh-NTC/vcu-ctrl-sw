THIS_EXE_NTC_VCU_DECODER := $(call get-my-dir)

EXE_NTC_VCU_DECODER_SRC := \
  $(THIS_EXE_NTC_VCU_DECODER)/main.cpp \
  $(THIS_EXE_NTC_VCU_DECODER)/VcuDecodedBufferAdapter.cpp \
  $(THIS_EXE_NTC_VCU_DECODER)/PreloadedFileSource.cpp \
  exe_decoder/SinkYuvCrc.cpp \
  exe_decoder/CmdParser.cpp \
  exe_decoder/IpDevice.cpp \
  exe_decoder/CodecUtils.cpp \
  exe_decoder/InputLoader.cpp

ifneq ($(ENABLE_DEC_SW_HIGH_DYNAMIC_RANGE),0)
  EXE_NTC_VCU_DECODER_SRC += exe_decoder/HDRWriter.cpp
endif

NTC_PRIVACY_CALLBACK_DEPENDENCIES :=

ifeq ($(NTC_ENABLE_PRIVACY_CALLBACK),1)
  ifeq ($(strip $(NTC_PRIVACY_CALLBACK_INCLUDE_DIR)),)
    $(error NTC_PRIVACY_CALLBACK_INCLUDE_DIR is required when NTC_ENABLE_PRIVACY_CALLBACK=1)
  endif
  ifeq ($(strip $(NTC_PRIVACY_CALLBACK_LIBS)),)
    $(error NTC_PRIVACY_CALLBACK_LIBS is required when NTC_ENABLE_PRIVACY_CALLBACK=1)
  endif

  EXE_NTC_VCU_DECODER_SRC += \
    $(THIS_EXE_NTC_VCU_DECODER)/VcuPrivacyCallbackIntegration.cpp

  NTC_PRIVACY_CALLBACK_DEPENDENCIES := $(NTC_PRIVACY_CALLBACK_LIBS)

  $(BIN)/$(THIS_EXE_NTC_VCU_DECODER)/main.cpp.o \
  $(BIN)/$(THIS_EXE_NTC_VCU_DECODER)/VcuPrivacyCallbackIntegration.cpp.o: \
    CFLAGS += \
      -DNTC_ENABLE_PRIVACY_CALLBACK=1 \
      -I$(NTC_PRIVACY_CALLBACK_INCLUDE_DIR)

  ifeq ($(NTC_ENABLE_PRIVACY_GDB_EVIDENCE),1)
    $(BIN)/$(THIS_EXE_NTC_VCU_DECODER)/VcuPrivacyCallbackIntegration.cpp.o: \
      CFLAGS += \
        -DNTC_ENABLE_PRIVACY_GDB_EVIDENCE=1
  endif
endif

EXE_NTC_VCU_DECODER_OBJ := \
  $(EXE_NTC_VCU_DECODER_SRC:%=$(BIN)/%.o)

$(BIN)/$(THIS_EXE_NTC_VCU_DECODER)/main.cpp.o \
$(BIN)/$(THIS_EXE_NTC_VCU_DECODER)/VcuDecodedBufferAdapter.cpp.o \
$(BIN)/$(THIS_EXE_NTC_VCU_DECODER)/PreloadedFileSource.cpp.o: \
  CFLAGS += -Iexe_decoder

$(BIN)/NTC_VcuDecoder.exe: \
  $(EXE_NTC_VCU_DECODER_OBJ) \
  $(LIB_REFDEC_A) \
  $(LIB_REFALLOC_A) \
  $(LIB_DECODER_A) \
  $(LIB_APP_A) \
  $(LIB_REFFBC_A) \
  $(LIB_REF_LCEVC_DEC_A) \
  $(LIB_LCEVC_DECODE_A) \
  $(NTC_PRIVACY_CALLBACK_DEPENDENCIES)

NTC_VcuDecoder.exe: $(BIN)/NTC_VcuDecoder.exe
TARGETS += NTC_VcuDecoder.exe

.PHONY: NTC_VcuDecoder.exe
