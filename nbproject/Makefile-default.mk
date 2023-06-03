#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/FLYBACK_PEAK_CURRENT_CONTROL.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/FLYBACK_PEAK_CURRENT_CONTROL.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=pid_TIPO_II_Q3_12_buck2_CPC.s src/isr_asm_Q3_12_buck2_CPC.s src/init_Q3_12_CPC.c src/rs232_params_Q3_12_CPC.c src/main_CPC.c src/isr_CPC.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o ${OBJECTDIR}/src/init_Q3_12_CPC.o ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o ${OBJECTDIR}/src/main_CPC.o ${OBJECTDIR}/src/isr_CPC.o
POSSIBLE_DEPFILES=${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o.d ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o.d ${OBJECTDIR}/src/init_Q3_12_CPC.o.d ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o.d ${OBJECTDIR}/src/main_CPC.o.d ${OBJECTDIR}/src/isr_CPC.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o ${OBJECTDIR}/src/init_Q3_12_CPC.o ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o ${OBJECTDIR}/src/main_CPC.o ${OBJECTDIR}/src/isr_CPC.o

# Source Files
SOURCEFILES=pid_TIPO_II_Q3_12_buck2_CPC.s src/isr_asm_Q3_12_buck2_CPC.s src/init_Q3_12_CPC.c src/rs232_params_Q3_12_CPC.c src/main_CPC.c src/isr_CPC.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/FLYBACK_PEAK_CURRENT_CONTROL.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ16GS502
MP_LINKER_FILE_OPTION=,-Tp33FJ16GS502.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o: pid_TIPO_II_Q3_12_buck2_CPC.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o.d 
	@${RM} ${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o.ok ${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o.err 
	@${RM} ${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_AS} $(MP_EXTRA_AS_PRE)  pid_TIPO_II_Q3_12_buck2_CPC.s -o ${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 --defsym=__MPLAB_DEBUG=1 --defsym=__ICD2RAM=1 --defsym=__DEBUG=1 --defsym=__MPLAB_DEBUGGER_ICD3=1 -g  -MD "${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o.d" -I"h" -I"inc"$(MP_EXTRA_AS_POST)
	
${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o: src/isr_asm_Q3_12_buck2_CPC.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o.d 
	@${RM} ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o.ok ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o.err 
	@${RM} ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_AS} $(MP_EXTRA_AS_PRE)  src/isr_asm_Q3_12_buck2_CPC.s -o ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 --defsym=__MPLAB_DEBUG=1 --defsym=__ICD2RAM=1 --defsym=__DEBUG=1 --defsym=__MPLAB_DEBUGGER_ICD3=1 -g  -MD "${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o.d" -I"h" -I"inc"$(MP_EXTRA_AS_POST)
	
else
${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o: pid_TIPO_II_Q3_12_buck2_CPC.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o.d 
	@${RM} ${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o.ok ${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o.err 
	@${RM} ${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_AS} $(MP_EXTRA_AS_PRE)  pid_TIPO_II_Q3_12_buck2_CPC.s -o ${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 -g  -MD "${OBJECTDIR}/pid_TIPO_II_Q3_12_buck2_CPC.o.d" -I"h" -I"inc"$(MP_EXTRA_AS_POST)
	
${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o: src/isr_asm_Q3_12_buck2_CPC.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o.d 
	@${RM} ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o.ok ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o.err 
	@${RM} ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_AS} $(MP_EXTRA_AS_PRE)  src/isr_asm_Q3_12_buck2_CPC.s -o ${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 -g  -MD "${OBJECTDIR}/src/isr_asm_Q3_12_buck2_CPC.o.d" -I"h" -I"inc"$(MP_EXTRA_AS_POST)
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/src/init_Q3_12_CPC.o: src/init_Q3_12_CPC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/init_Q3_12_CPC.o.d 
	@${RM} ${OBJECTDIR}/src/init_Q3_12_CPC.o.ok ${OBJECTDIR}/src/init_Q3_12_CPC.o.err 
	@${RM} ${OBJECTDIR}/src/init_Q3_12_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/src/init_Q3_12_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"h" -I"inc" -I"C:/Program Files (x86)/Microchip/MPLABC30/v3.31/include" -MMD -MF "${OBJECTDIR}/src/init_Q3_12_CPC.o.d" -o ${OBJECTDIR}/src/init_Q3_12_CPC.o src/init_Q3_12_CPC.c    
	
${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o: src/rs232_params_Q3_12_CPC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o.d 
	@${RM} ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o.ok ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o.err 
	@${RM} ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"h" -I"inc" -I"C:/Program Files (x86)/Microchip/MPLABC30/v3.31/include" -MMD -MF "${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o.d" -o ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o src/rs232_params_Q3_12_CPC.c    
	
${OBJECTDIR}/src/main_CPC.o: src/main_CPC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main_CPC.o.d 
	@${RM} ${OBJECTDIR}/src/main_CPC.o.ok ${OBJECTDIR}/src/main_CPC.o.err 
	@${RM} ${OBJECTDIR}/src/main_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/src/main_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"h" -I"inc" -I"C:/Program Files (x86)/Microchip/MPLABC30/v3.31/include" -MMD -MF "${OBJECTDIR}/src/main_CPC.o.d" -o ${OBJECTDIR}/src/main_CPC.o src/main_CPC.c    
	
${OBJECTDIR}/src/isr_CPC.o: src/isr_CPC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/isr_CPC.o.d 
	@${RM} ${OBJECTDIR}/src/isr_CPC.o.ok ${OBJECTDIR}/src/isr_CPC.o.err 
	@${RM} ${OBJECTDIR}/src/isr_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/src/isr_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"h" -I"inc" -I"C:/Program Files (x86)/Microchip/MPLABC30/v3.31/include" -MMD -MF "${OBJECTDIR}/src/isr_CPC.o.d" -o ${OBJECTDIR}/src/isr_CPC.o src/isr_CPC.c    
	
else
${OBJECTDIR}/src/init_Q3_12_CPC.o: src/init_Q3_12_CPC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/init_Q3_12_CPC.o.d 
	@${RM} ${OBJECTDIR}/src/init_Q3_12_CPC.o.ok ${OBJECTDIR}/src/init_Q3_12_CPC.o.err 
	@${RM} ${OBJECTDIR}/src/init_Q3_12_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/src/init_Q3_12_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"h" -I"inc" -I"C:/Program Files (x86)/Microchip/MPLABC30/v3.31/include" -MMD -MF "${OBJECTDIR}/src/init_Q3_12_CPC.o.d" -o ${OBJECTDIR}/src/init_Q3_12_CPC.o src/init_Q3_12_CPC.c    
	
${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o: src/rs232_params_Q3_12_CPC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o.d 
	@${RM} ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o.ok ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o.err 
	@${RM} ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"h" -I"inc" -I"C:/Program Files (x86)/Microchip/MPLABC30/v3.31/include" -MMD -MF "${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o.d" -o ${OBJECTDIR}/src/rs232_params_Q3_12_CPC.o src/rs232_params_Q3_12_CPC.c    
	
${OBJECTDIR}/src/main_CPC.o: src/main_CPC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main_CPC.o.d 
	@${RM} ${OBJECTDIR}/src/main_CPC.o.ok ${OBJECTDIR}/src/main_CPC.o.err 
	@${RM} ${OBJECTDIR}/src/main_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/src/main_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"h" -I"inc" -I"C:/Program Files (x86)/Microchip/MPLABC30/v3.31/include" -MMD -MF "${OBJECTDIR}/src/main_CPC.o.d" -o ${OBJECTDIR}/src/main_CPC.o src/main_CPC.c    
	
${OBJECTDIR}/src/isr_CPC.o: src/isr_CPC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/isr_CPC.o.d 
	@${RM} ${OBJECTDIR}/src/isr_CPC.o.ok ${OBJECTDIR}/src/isr_CPC.o.err 
	@${RM} ${OBJECTDIR}/src/isr_CPC.o 
	@${FIXDEPS} "${OBJECTDIR}/src/isr_CPC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -I"h" -I"inc" -I"C:/Program Files (x86)/Microchip/MPLABC30/v3.31/include" -MMD -MF "${OBJECTDIR}/src/isr_CPC.o.d" -o ${OBJECTDIR}/src/isr_CPC.o src/isr_CPC.c    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/FLYBACK_PEAK_CURRENT_CONTROL.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/FLYBACK_PEAK_CURRENT_CONTROL.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-L"h",-L"C:/Archivos de programa/Microchip/MPLAB C30/lib",-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/FLYBACK_PEAK_CURRENT_CONTROL.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/FLYBACK_PEAK_CURRENT_CONTROL.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-L"h",-L"C:/Archivos de programa/Microchip/MPLAB C30/lib",-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/FLYBACK_PEAK_CURRENT_CONTROL.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
