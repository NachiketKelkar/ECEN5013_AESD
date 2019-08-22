################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs900/ccs/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --include_path="C:/ti/ccs900/ccs/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="C:/Users/nachi/workspace_v9/freertos_demo" --include_path="C:/ti/tivaware_c_series_2_1_4_178/examples/boards/dk-tm4c129x" --include_path="C:/ti/tivaware_c_series_2_1_4_178" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/lwip-1.4.1/apps" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS/Source/include" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS" --include_path="C:/ti/tivaware_c_series_2_1_4_178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power=all --define=ccs="ccs" --define=PART_TM4C129XNCZAD --define=TARGET_IS_TM4C129_RA0 --define=SNOWFLAKE -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


