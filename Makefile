ROMNAME = "n64base"
ROMTITLE = "BASE N64 GAME LOADER"
FINAL = 0	# Build the final ROM (tune up compression)
DEBUG = 1	# Enable printf debugging

BUILD_DIR = build
ASSETS_DIR = assets
GAME_DIR = code
FILESYSTEM_DIR = filesystem

SRC = main.cpp core.cpp \
	$(wildcard $(GAME_DIR)/*.c) \
	$(wildcard $(GAME_DIR)/**/*.c) \
	$(wildcard $(GAME_DIR)/*.cpp) \
	$(wildcard $(GAME_DIR)/**/*.cpp) \
	$(wildcard $(GAME_DIR)/**/**/*.cpp)

include $(N64_INST)/include/n64.mk
include $(N64_INST)/include/t3d.mk

N64_CXXFLAGS += -std=gnu++17 -fno-exceptions -Iinclude $(foreach dir,$(shell find include -type d),-I$(dir))

N64_ROM_SAVETYPE = eeprom4k
ifeq ($(strip $(FINAL)),1)
  N64_ROM_ELFCOMPRESS = 3
  N64_ROM_DSOCOMPRESS = 3
  MKSPRITE_FLAGS = --compress 2
  MKFONT_FLAGS = --compress 2
endif

IMAGE_LIST = $(wildcard $(ASSETS_DIR)/*.png)
FONT_LIST  = $(wildcard $(ASSETS_DIR)/*.ttf)
MODEL_LIST  = $(wildcard $(ASSETS_DIR)/*.glb)
SOUND_LIST  = $(wildcard $(ASSETS_DIR)/*.wav)
SOUND2_LIST  = $(wildcard $(ASSETS_DIR)/*.mp3)
MUSIC_LIST  = $(wildcard $(ASSETS_DIR)/*.xm)
DESC_LIST  = $(wildcard $(ASSETS_DIR)/*.txt)
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(IMAGE_LIST:%.png=%.sprite))
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(FONT_LIST:%.ttf=%.font64))
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(MODEL_LIST:%.glb=%.t3dm))
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(SOUND_LIST:%.wav=%.wav64))
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(SOUND2_LIST:%.mp3=%.wav64))
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(MUSIC_LIST:%.xm=%.xm64))
ASSETS_LIST += $(subst $(ASSETS_DIR),$(FILESYSTEM_DIR),$(DESC_LIST:%.txt=%.desc))

ifeq ($(strip $(DEBUG)), 1)
	N64_CFLAGS := $(filter-out -O%,$(N64_CFLAGS)) -g3 -Og -DDEBUG=$(strip $(DEBUG))
    N64_CXXFLAGS := $(filter-out -O%,$(N64_CXXFLAGS)) -g3 -Og -DDEBUG=$(strip $(DEBUG))
    N64_LDFLAGS += -g
endif

# Debug: Print compiler info
$(info N64_CC = $(N64_CC))
$(info N64_CXX = $(N64_CXX))
$(info N64_LD = $(N64_LD))
$(info N64_INST = $(N64_INST))
$(info CFLAGS = $(CFLAGS))
$(info CXXFLAGS = $(CXXFLAGS))
$(info N64_CXXFLAGS = $(N64_CXXFLAGS))
$(info N64_LDFLAGS = $(N64_LDFLAGS))
$(info DEBUG_RAW='$(DEBUG)')
$(info DEBUG_STRIPPED='$(strip $(DEBUG))')

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "    [CXX] $<"
	$(N64_CXX) -c $(CXXFLAGS) $(N64_CXXFLAGS) -MMD -MF $(BUILD_DIR)/$*.d -o $@ $<

all: $(ROMNAME).z64

$(FILESYSTEM_DIR)/%.sprite: $(ASSETS_DIR)/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/%.font64: $(ASSETS_DIR)/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/%.t3dm: $(ASSETS_DIR)/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) $(T3DM_FLAGS) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -o $(dir $@) $@

$(FILESYSTEM_DIR)/%.wav64: $(ASSETS_DIR)/%.wav
	@mkdir -p $(dir $@)
	@echo "    [SFX] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/%.wav64: $(ASSETS_DIR)/%.mp3
	@mkdir -p $(dir $@)
	@echo "    [SFX] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

$(FILESYSTEM_DIR)/%.xm64: $(ASSETS_DIR)/%.xm
	@mkdir -p $(dir $@)
	@echo "    [XM] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

N_ELF_EXTERNS := $(BUILD_DIR)/$(ROMNAME).externs
$(BUILD_DIR)/$(ROMNAME).dfs: $(ASSETS_LIST)
$(BUILD_DIR)/$(ROMNAME).elf: $(SRC:%.c=$(BUILD_DIR)/%.o) $(SRC:%.cpp=$(BUILD_DIR)/%.o)
$(ROMNAME).z64: N64_ROM_TITLE=$(ROMTITLE)
$(ROMNAME).z64: $(BUILD_DIR)/$(ROMNAME).dfs $(BUILD_DIR)/$(ROMNAME).msym

$(BUILD_DIR)/$(ROMNAME).msym: $(BUILD_DIR)/$(ROMNAME).elf

clean:
	rm -rf $(BUILD_DIR) $(FILESYSTEM_DIR) $(ROMNAME).z64 

-include $(wildcard $(BUILD_DIR)/*.d) $(wildcard $(BUILD_DIR)/*/*.d) $(wildcard $(BUILD_DIR)/*/*/*.d) $(wildcard $(BUILD_DIR)/*/*/*/*.d)

.PHONY: all clean
