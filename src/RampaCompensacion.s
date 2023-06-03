

.global _CompensarRampa
.include "p33FJ16GS502.inc"
.include  "inc\dspcommon.inc"         ; fractsetup

.equ RATIO_ESCALADO_Vo_Vi,0X5DB2 ;(vi*12,4/2,4/) y (vo*3,3/23,3)


.equ    offsetmeasuredOutput, 6
.equ  	offsetFactorComp,12


;capturamos vi
;se programa para iniciar la conversion del par 2 para obtener vi
;se configura TRGSR2 para Global software trigger selected
.bss
.align 2
_ksc2PORkscFrac: .space 2
_ksc2: .space 2
_kscFrac: .space 2
_kscEntera: .space 2
_kscPORkscEntera:.space 2

.text
_CompensarRampa:
push.s
push w4
push w5
push w7
push CORCON

;bclr _valoresNuevosRampa,#0
 ;Testigo para ver en el osciloscopio
bset LATA,#3
fractsetup      w8	; macro para programar CORCON: fraccional y saturacion

;**********************************************************************
;parte de la logica para no dejar que cuando acabe se le llame si antes
;no llega un fin de la interrupcion ADCP1Interrupt
;SI calculoRampaActiva=1  valoresNuevosRampa 1-->0 en ADCP1Interrupt
;SI calculoRampaActiva=0  valoresNuevosRampa 0-->1 en ADCP1Interrupt
;SI valoresNuevosRampa==1, quiere decir que han cambiado y desde main() 
;se llama a esta funcion cuando toca.
;**********************************************************************

mov #RATIO_ESCALADO_Vo_Vi,w5
; 0.732*vi---->w3
;el valor de vi solo se actualiza aqui
_WAIT_ADBUF4:
btsc ADCPC1,#6
bra _WAIT_ADBUF4
mov ADCBUF4, w4
sl w4,#5,w4
clr a
mac w4*w5, a
sac.r a,w3
;el valor de vo se actualiza siempre en ADCP1Interrupt
mov #_Buck2VoltagePID, w2
mov [w2+#offsetmeasuredOutput],w4
; w4(vo)---->b
lac w4, b		
;(vo-vi)---->b
sub b
; se redondea, porque acc 40 bits y w1 16 bits
sac.r b, w4
;ahora a se divide (vi-vo)/vi uso no fractional division cociente --->w0 resto--->w1
;en wo parte entera de ksc y en w1 resto. Despues se hace divf (division fraccional)
;con w1 y se obtiene la parte fraccional de ksc
;OBSERVESE QUE COMO ES DE 16 BIT LA INSTRUCCION DE DIVISION 
;TIENE QUE REPETIRSE 17 VECES
repeat #17
div.u w4,w3
;se guarda la parte entera en ksc
mov w0,_kscEntera
;se halla la parte fraccionara y se guarda
mov w1,w3

repeat #17
divf w3,w4
mov w0,_kscFrac
;se calcula 1/(1+ksc) y se guarda en ksc2 como fracionario 
;como (1+ksc)va a ser mayor que 1 se cambia la representacion
;de Q1.15 a Q(bits para representar parte entera.bits hasta completar 15 bits)
;el bit de signo se respeta porque lo usa la division
mov _kscEntera, w7
;como la operacion es (1+ksc) lo primero sumar 1 a la parte entera
add #0x1,w7
ff1l w7, w5 ;encuentra el primer 1 desde el MSB
sub #0x2,w5;shift hasta que el primer 1 esta
sl w7,w5,w7 ;en la posicion 15 (respeta el signo).Esta es la parte entera escalada.
mov _kscFrac,w4
;se da por supuesto que el signo de la parte fracionaria es 0 
;ahora se deplaza a la derecha 16-(ff1l-1)=17-ff1l=15-w5 bits para 
;ir 1 mas atras que el LSB de la parte entera.
mov #15,w3
sub w3,w5,w3
lsr w4,w3,w4
;se hace una OR para poner juntos la parte entera y decimal, el resultado en w4
ior w7,w4,w4
;Ahora para preparar la division 1/(1+ksc) se cuenta el numero de lugares decimales y
;como solo se puede hacer la division de enteros el 1 se desplaza a la izquierda tantos
;bits como el numero de bits fraccion del denomindador (equivalente a multiplicar arriba y abajo por 2^N)
;1"bit signo"+(16-w1"cuenta de ff1l-1")+N"numero de bits parte decimal" =16 "NumTotalBits" ==>N=w1-1
;luego el desplazamiento del 1 tiene que ser w1=ff1l-1
mov #0x01,w3
sl w3,w5,w3
;ahora en w3 el 1 escalado para la division y en w4 (1+ksc)
;Se hace la division fraccionaria que se guarda en ksc2=1/(1+ksc)
repeat #17
divf w3,w4 ;cociente en w0 y resto en w1. como tiene que ser < que 1 solo se coje el cociente y 
;se desprecia el resto.
cp0 w0
bra ge, noZero
bset LATA,#3
noZero:
mov w0,_ksc2
;ahora que ya se tiene ksc y ksc2 se opera para icmp=ksc2*(icontrol+ksc*in)
;NOTA: AHORA SE VA A CALCULAR ksc2*ksc y se guarda para usarlo en la rutina ADCP1Interrupt
mov _kscFrac,w4
mov _ksc2,w5
clr a
mac w4*w5, a ;kscfrac*ksc2
sac.r a,w0
;mov w4,_ksc2PORkscFrac
mov _kscEntera,w4
lac w5,b
clr a
LOOP_PARTE_ENTERA_COMPENSADOR:
dec w4,w4
bra n,FIN_PARTE_ENTERA_COMPENSADOR
add a
bra LOOP_PARTE_ENTERA_COMPENSADOR
FIN_PARTE_ENTERA_COMPENSADOR:
;_FactorCompensacion=_ksc2PORkscFrac+_ksc2PORkscEntera
lac w0,b
add a 
sac.r a,w0

mov w0,[w2+#offsetFactorComp]

bclr _valoresNuevosRampa,#0
bclr ADSTAT,	#2
bclr LATA,#3

pop CORCON
pop w7	
pop w5
pop w4
pop.s

return 
.end

