################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
Relay.lss \

FLASH_IMAGE += \
Relay.hex \

EEPROM_IMAGE += \
Relay.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: Relay.elf secondary-outputs

# Tool invocations
Relay.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Relay.map -mmcu=attiny13a -o "Relay.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Relay.lss: Relay.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Relay.elf  >"Relay.lss"
	@echo 'Finished building: $@'
	@echo ' '

Relay.hex: Relay.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex Relay.elf  "Relay.hex"
	@echo 'Finished building: $@'
	@echo ' '

Relay.eep: Relay.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex Relay.elf  "Relay.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Relay.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny13a Relay.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) Relay.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
