################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
src/drivers/%.obj: ../src/drivers/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-msp430_20.2.2.LTS/bin/cl430" -vmspx --code_model=large --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/Users/asgaria1/OneDrive - University of Manitoba/Documents/GitHub/IrisSat-Power-Software/iris-powctrl-ccs/include" --include_path="C:/ti/ccs1020/ccs/ccs_base/msp430/include" --include_path="C:/Users/asgaria1/OneDrive - University of Manitoba/Documents/GitHub/IrisSat-Power-Software/iris-powctrl-ccs" --include_path="C:/Users/asgaria1/OneDrive - University of Manitoba/Documents/GitHub/IrisSat-Power-Software/iris-powctrl-ccs/driverlib/MSP430FR2xx_4xx" --include_path="C:/ti/ccs1020/ccs/tools/compiler/ti-cgt-msp430_20.2.2.LTS/include" --include_path="D:/IrisSat-Software/IrisSat-Power-Software/iris-powctrl-ccs/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR2475__ --define=_FRWP_ENABLE --define=_INFO_FRWP_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="src/drivers/$(basename $(<F)).d_raw" --obj_directory="src/drivers" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


