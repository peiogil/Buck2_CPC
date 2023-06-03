; ******************************************************************************
; * © 2008 Microchip Technology Inc.
; *
; SOFTWARE LICENSE AGREEMENT:
; Microchip Technology Incorporated ("Microchip") retains all ownership and 
; intellectual property rights in the code accompanying this message and in all 
; derivatives hereto.  You may use this code, and any derivatives created by 
; any person or entity by or on your behalf, exclusively with Microchip's
; proprietary products.  Your acceptance and/or use of this code constitutes 
; agreement to the terms and conditions of this notice.
;
; CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
; WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
; TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
; PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
; PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
;
; YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
; IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
; STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
; PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
; ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
; ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
; ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
; THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
; HAVE THIS CODE DEVELOPED.
;
; You agree that you are solely responsible for testing the code and 
; determining its suitability.  Microchip has no obligation to modify, test, 
; certify, or support the code.
;
; *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.equ __33FJ16GS502, 1
.include "p33FJ16GS502.inc"
.include        "inc\dspcommon.inc"         ; fractsetup

; Buck 2 Maximum Current 3A ==>3/60*20=1V=>1V*1023/1,65=620 MaxCMPDAC=0x16C
.equ MaxCMPDAC, 0X200
; Buck 2 Minimum Duty cycle for minimum dutyvoltage mode control
;Buck 2 Minimum Current 0,33A ==>0,33/60*20=0,11V=>0,11V*1024/1,65=68 MinCMPDAC=0x48
.equ MinCMPDAC, 0X48 

.equ    offsetabcCoefficients, 0
.equ    offsetcontrolHistory, 2
.equ    offsetcontrolOutput, 4
.equ    offsetmeasuredOutput, 6
.equ    offsetcontrolReference, 8
;.equ 	offsetcorriente_in,10
;.equ  	offsetFactorComp,12



.text
; a partir de aqui a la memoria de programa


.global __ADCP1Interrupt

__ADCP1Interrupt:
    push w0
    push w1 
    push w2  

    mov #_Buck2VoltagePID, w0			
    ;mov #617, w1 ;PARA PROBAR 
	mov ADCBUF3, w1
    sl  w1, #2, w1 ;escalado en 2 bits hacia la izquierda para adecuarlo a 12 bits de
		  ;de Q3.12

    mov w1, [w0+#offsetmeasuredOutput]
    call _PIDBUCK2 						; Call PIDBUCK2 routine
     ; return from PIDBUCK2 routine with the updated control voltage
    mov.w [w0+#offsetcontrolOutput], w2 ; Clamp PID output to allowed limits
   asr w2, #5, w1 	;escalado de 5 bits a la derecha para ajustar
			;16bit(32767) a 10bit del DA del CMPDAC 
    mov.w #MinCMPDAC, w0 ;saturate to minimum current
	cpsgt w1, w0
    	mov.w w0, w1
    mov.w #MaxCMPDAC, w0	; saturate to maximum current
	cpsgt w0, w1
	mov.w w0, w1
	;mov #0x150,w1             ;INSTRUCCION EXTRA PARA ANULAR EL CONTROL EN MODO CORRIENTE Y CARGAR UN VALOR FIJO
	mov w1, CMPDAC1		  ;Update compator register to compare peak current with voltage value
				  ;Máximo número 3FF =1023 DA de 10bits
bclr ADSTAT,	#1	; Clear Pair 1 conversion status bit
bclr IFS6, #15	; Clear Pair 1 Interrupt Flag


pop w2
pop w1
pop w0
pop.s

retfie
.end
