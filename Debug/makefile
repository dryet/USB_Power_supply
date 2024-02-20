################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include USBX/App/subdir.mk
-include Middlewares/ST/usbx/common/usbx_stm32_device_controllers/subdir.mk
-include Middlewares/ST/usbx/common/usbx_device_classes/src/subdir.mk
-include Middlewares/ST/usbx/common/core/src/subdir.mk
-include Middlewares/ST/threadx/ports/cortex_m0/gnu/src/subdir.mk
-include Middlewares/ST/threadx/common/src/subdir.mk
-include Drivers/STM32G0xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include AZURE_RTOS/App/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := USB_Power_supply_fv0.1
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
USB_Power_supply_fv0.1.elf \

MAP_FILES += \
USB_Power_supply_fv0.1.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
USB_Power_supply_fv0.1.list \


# All Target
all: main-build

# Main-build Target
main-build: USB_Power_supply_fv0.1.elf secondary-outputs

# Tool invocations
USB_Power_supply_fv0.1.elf USB_Power_supply_fv0.1.map: $(OBJS) $(USER_OBJS) C:\Users\Petr\Electronics\Projects\USB_Power_supply\Firmware\USB_Power_supply_fv0.1\STM32G0B1CBTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "USB_Power_supply_fv0.1.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0plus -T"C:\Users\Petr\Electronics\Projects\USB_Power_supply\Firmware\USB_Power_supply_fv0.1\STM32G0B1CBTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="USB_Power_supply_fv0.1.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

USB_Power_supply_fv0.1.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "USB_Power_supply_fv0.1.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) USB_Power_supply_fv0.1.elf USB_Power_supply_fv0.1.list USB_Power_supply_fv0.1.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
