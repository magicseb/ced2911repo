#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITXENON)),)
$(error "Please set DEVKITXENON in your environment. export DEVKITXENON=<path to>devkitxenon")
endif

include $(DEVKITXENON)/rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	genesisplus
TARGETDIR   	:= 	executables
BUILD		:=	build_xenon
SOURCES		:= 	megadrive megadrive/cart_hw megadrive/cart_hw/svp megadrive/sound_md megadrive/unzip megadrive/xenon megadrive/m68k megadrive/cpu megadrive/ntsc\
		 	menu menu/shaders menu/png menu/video_filter/hq2x  menu/include menu/font menu/video_filter/scale2x menu menu/include menu/sound menu/video menu/igm

INCLUDES	:=	megadrive/ megadrive/cart_hw megadrive/cart_hw/svp megadrive/sound_md megadrive/unzip  megadrive/xenon megadrive/m68k   megadrive/cpu megadrive/ntsc \
			menu menu/include menu/video_filter/hq2x menu/font	menu/video_filter/scale2x menu menu/sound build_xenon  menu/video  menu/igm

BUILD_DATE	:=	`date +%Y%m%d`

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS	=	-g -O3 -Wall $(MACHDEP) $(INCLUDE) \
			-DNGC 
			-DNO_ASM -DRIGHTSHIFT_IS_SAR \
			-DCPU_SHUTDOWN -DSPC700_SHUTDOWN \
			-DSPC700_C -DSDD1_DECOMP \
			-DCORRECT_VRAM_READS -DNEW_COLOUR_BLENDING \
			-D_SZ_ONE_DIRECTORY -D_LZMA_IN_CB -D_LZMA_OUT_READ \
			-fomit-frame-pointer \
			-DGENESIS_HACKS="1" -m32 -mno-altivec -fno-pic -mpowerpc64\
			-Wno-unused-parameter -Wno-strict-aliasing -DWORDS_BIGENDIAN\
			-DUSE_CZ80 -DUSE_SMS_SOUND

CXXFLAGS	=	-save-temps -Xassembler -aln=$@.lst $(CFLAGS)

LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
#LIBS	:=	-ldb -ldi -lpngu -lpng -lmxml -lmetaphrasis \
#			-lfat -lwiiuse -lz -lbte -logc -lasnd -ltremor -lfreetype -ltinysmb
LIBS = -lm -lxenon -lz -lpng 

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(CURDIR)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGETDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
PNGFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.png)))
BINFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.bin)))
TTFFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.ttf)))
OGGFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.ogg)))
PCMFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.pcm)))
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(PNGFILES:.png=.png.o) $(OGGFILES:.ogg=.ogg.o) \
					$(PCMFILES:.pcm=.pcm.o) $(BINFILES:.bin=.bin.o)\
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o) \
					$(TTFFILES:.ttf=.ttf.o)  \
					

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBXENON_INC)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					-L$(LIBXENON_LIB)

export OUTPUT	:=	$(CURDIR)/$(TARGETDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@[ -d $(TARGETDIR) ] || mkdir -p $(TARGETDIR)
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).elf32
	@echo cleaning some file
	@find . -name \*.\*~ -exec rm {} \;

#---------------------------------------------------------------------------------
all: $(BUILD) $(OUTPUT).elf $(OUTPUT).elf32

run: $(OUTPUT).elf32
	cp $(OUTPUT).elf32 /tftpboot/tftpboot/xenon
	@echo Copie Ok !!
	@echo
#	cp $(OUTPUT).elf32 g:/xenon.elf
#	cp $(OUTPUT).elf32 h:/xenon.elf
distro:clean $(BUILD)
	@echo $(BUILD_DATE)
	@echo making distribuable binaries
	@mkdir -p distro
	@cp $(OUTPUT).elf32 distro/xenon.elf
	@cp Readme.txt distro/Readme.txt
	#@tar -zcvf smsplus_'eval date +%Y%m%d'.tar.gz distro/ 
	@rm *.zip -f
	@zip -r $(TARGET)_$(BUILD_DATE).zip distro/
	@rm *.tar.gz -f
	@tar -zcvf  $(TARGET)_src_$(BUILD_DATE).tar.gz . --exclude=build_xenon/* --exclude=executables/* --exclude=$(TARGET)_src_$(BUILD_DATE).tar.gz
	@echo -en "\033[1;32m" Distribution done !! "\033[m\017"
	@echo
	

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).elf32: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with .ttf, .png, and .mp3 extensions
#---------------------------------------------------------------------------------
%.ttf.o : %.ttf
	@echo $(notdir $<)
	$(bin2o)

%.png.o : %.png
	@echo $(notdir $<)
	@$(bin2o)

%.bin.o : %.bin
	@echo $(notdir $<)
	@$(bin2o)
	
%.pcm.o : %.pcm
	@echo $(notdir $<)
	$(bin2o)
	

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
