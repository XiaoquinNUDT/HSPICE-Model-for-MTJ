** CPMTJ macro-model 
**
** Author:
**	Lianhua Qu		
** Email: mingyiweiji@sina.com
** Cooperators:
** Quan Deng, Haoyue Tang, and Huan Li

** Device Parameter **
** From 'Lanting Cheng, Sakhrat Khizroev, and Ping Liang, "3-Termonal pMTJ Reduces Critical Current and Switching Time", Journal of Magnetism and Magnetic Materials, pp 358-359, Doi: 10.1016/j.jmmm.2014.01.007 '
**>>   +arfa=0.01 $$ damping facotr
**>>   +tao0=1e-9
**>>   +pola=0.56 $$ spin polarization factor 
**>>   +fai=0.4
**>>   +F=332.2 $$ calculated from RA
**>>   +Vh=0.5 $$ half voltage
**>>   +Ms=3.25e5 $$ saturation magnetization
**>>   +Hk=4.0e5 $$ anistropy field

** Latest Update:
**>>Tue Dec 15 10:37:19 CST 2015
**>>Fri Mar 25 17:54:15 GMT+7 2016
**>>Wed Apr 20 08:09:40 GMT+7 2016
**>>Thu Sep 29 09:40:14 CST 2016

** -------------------------**
** Declaration of Sub-circuit
** -------------------------**
.SUBCKT CPMTJ nf+ np1- np2- Diameter=40e-9 t_fre=1e-9 t_ox=1e-9 t_spa=4e-9 T0=300 JOULE=1 Polarizer=1 IS=1 TMR_G0=1.2 TMR_G1=1.2 IC_G0=1 IC_G1=1 RA_G0=1 RA_G1=1
** TMR_G0, TMR_G1, IC_G0, IC_G1, RA_G0, and RA_G1 can be used to perform Monte-Carlo simulations.
.PRINT R(GMTJ1)
.PRINT R(GMTJ2)

** -------------------------**
** User Parameter Defaults
** -------------------------**
***********physical constant*******
.PARAM
+electron=1.6e-19
+me=9.1e-31
+planck=6.626e-34
+replanck='planck/(2*3.14)'
+preme=12.56e-7
+gama='electron/me'
+eulerc=0.577
+boltzmamn=1.38e-23
+bohrmag='electron*replanck/(2*me)'

**********parameters determinated by materials********
.PARAM
+arfa=0.01 $$ damping facotr
+tao0=1e-9
+pola=0.56 $$ spin polarization factor 
+fai=0.4
+RA_F=332.2 $$ calculated from RA
+Vh=0.5 $$ half voltage
+Ms=3.25e5 $$ saturation magnetization
+Hk=4.0e5 $$ anistropy field
+PCAP=0.6e-15 $$ capacity of MTJ pillar

*********temperature parameter**********
.PARAM
+CV=3.47e6
+t_de=1e-8
+lanmga=1.5
+taoth=(CV*t_ox*t_de)/lanmga
*********variables**********
.PARAM
+area_pin='((3.14*pow(Diameter,2)/4)-(t_spa*Diameter))/2'
+area_fre='3.14*pow(Diameter,2)/4'
+VF='area_fre*t_fre'
+Mm='Ms*VF'
********calculated variables****
.PARAM
+EF='preme*Ms*Hk*VF*0.5'
+g_app='pola/(2*(1-pola^2))'
+g_pap='pola/(2*(1+pola^2))'
+IC_APP='(2*arfa*gama*electron*EF)/(bohrmag*(Polarizer*g_app+(1-Polarizer)*g_pap))*IC_G0'
+IC_PAP='(-1)*(2*arfa*gama*electron*EF)/(bohrmag*((Polarizer*g_app)+(1-Polarizer)*g_pap))*IC_G1'
+delt0='EF/(boltzmamn*T0)'
+tao='tao0*exp(delt0)'
+year='tao/365/24/3600'
+RP_0='(t_ox*exp(1.025*t_ox*(fai^(0.5))))/(RA_F*pwr(fai,0.5)*area_pin)*RA_G0'
+RAP_0='RP_0*(TMR_G0+1)'
+RP_1='(t_ox*exp(1.025*t_ox*(fai^(0.5))))/(RA_F*pwr(fai,0.5)*area_pin*RA_G1)'
+RAP_1='RP_1*(TMR_G1+1)'
.print PAR(IC_APP) PAR(IC_PAP) PAR(RP_0) PAR(RAP_0) PAR(RP_1) PAR(RAP_1) PAR(delt0)
** -------------------------**
** MTJ Electrode Connections
** -------------------------**
*voltage controlled TMR
*~Voltage Controlled Resistor
GMTJ1 	nf+	sensor1	VCR	nvctrl1	0 1 MIN='RP_0' MAX='RAP_0'
GMTJ2 	nf+	sensor2	VCR	nvctrl2	0 1 MIN='RP_1' MAX='RAP_1'

*~Parasitic Capacitance
*CPARA	nf+ sensor	'PCAP'

*~Current Sampling
VSENSOR1	sensor1 np1-	DC=0
VSENSOR2	sensor2 np2-	DC=0

HVSENSOR1 vmirror1 0 CCVS VSENSOR1 1
HVSENSOR2 vmirror2 0 CCVS VSENSOR2 1
EVEFFECT veffect 0 VOL='V(vmirror1)-V(vmirror2)' 
GIEFFECT ieffect 0 VCCS veffect 0 1
RIEFFECT ieffect 0 100meg
*~Binary Current Sensors
EDRI_APP	ndir_app 0	VOL='exp(5*I(GIEFFECT)/IC_APP)-1' MIN=0 MAX=1 
EDRI_PAP	ndir_pap 0	VOL='exp(5*I(GIEFFECT)/IC_PAP)-1' MIN=0 MAX=1

*~ tempreture control
EVCHARGE nvcharge 0 VOL='(abs(V(nf+)-V(sensor1))*abs(V(vmirror1))+abs(V(nf+)-V(sensor2))*abs(V(vmirror2)))*t_ox/area_pin/lanmga'
*EVCHARGE nvcharge 0 VOL='(abs(V(nf+)-V(sensor1))*abs(I(sensor1))*abs(I(sensor2)))*t_ox/area_pin/lanmga'
EVDRIVER nvdriver 0 VOL='((abs(V(nf+)-V(sensor1))+abs(V(nf+)-V(sensor2)))>0.005)?v(nvcharge):0v'
RRDRIVER nvdriver nvout 1
CCTEM nvout 0 C='taoth' 
EVDELT nvdelt 0 VOL='EF/(boltzmamn*(T0+(V(nvout)*JOULE)))' 
** -------------------------**
** Decision Circuit
** -------------------------**

*~Current mirrors
GDEC_APP	ndecv+	ndecv-	CUR='(I(GIEFFECT)>0)?((abs(I(GIEFFECT))<abs(IC_APP))?(pow((tao0*exp(V(nvdelt)*(1-(I(GIEFFECT)/IC_APP)))),(-1))*(1e-9)):(((2/(eulerc+log(3.14*3.14*V(nvdelt)/4)))*(bohrmag*pola/(electron*Mm*(1+pola*pola)))*(I(GIEFFECT)-IC_APP))*(1e-9))):0'
GDEC_PAP	ndecv-	ndecv+	CUR='(I(GIEFFECT)<0)?((abs(I(GIEFFECT))<abs(IC_PAP))?(pow((tao0*exp(V(nvdelt)*(1-(I(GIEFFECT)/IC_PAP)))),(-1))*(1e-9)):(((2/(eulerc+log(3.14*3.14*V(nvdelt)/4)))*(bohrmag*pola/(electron*Mm*(1+pola*pola)))*(abs(I(GIEFFECT))-abs(IC_PAP)))*(1e-9))):0'
GDEC_RETURN	ndecv+ ndecv- CUR='((abs(I(GIEFFECT))>1u)||(v(nlimiter+)<0.001)||(v(nlimiter-)<0.001))?0:((abs(v(nlimiter+)-v(nlimiter-))>1.009)?0:((v(nbistable)>0)?-10:10))'
*~Decision capacitors
CDEC-	ndecv-	0 1n IC='(IS<0)?1V:0V'
CDEC+	ndecv+	0 1n IC='(IS<0)?0V:1V'

*~Voltage limiters
RLIM1	ndecv-	nlimiter-	.01
ELIM1	nlimiter-	0	ndecv-	0	1	MIN=0 MAX=1.001 
RLIM2	ndecv+	nlimiter+	.01
ELIM2	nlimiter+	0	ndecv+	0	1	MIN=0 MAX=1.001
RSHUNT3 ndecv- 0 10g
RSHUNT4 ndecv+ 0 10g

*~Current controlled 2:1 MUX
EMUX4 nvdec	0	VOL='v(ndecv+)*v(ndir_pap)-v(ndecv-)*v(ndir_app)'  

** -------------------------**
** Bi-stable Multivibrator
** -------------------------**

*~Amplifier w/clipping
EBISTABLE	nbistable	0	VCVS	nfeedback	0	10000	MAX=10V		MIN=-10V

*~Positive Feedback
RFB1	nvdec	nfeedback	1e3
RFB2	nfeedback	nbistable	10e3

*~Initial Conditions
CIC1	nfeedback	0	5f	IC='(IS<0)?-1V:1V'
CIC2	nbistable	0	5f	IC='(IS<0)?-10V:10V'
** -------------------------**
** Curve Fitting Circuit
** -------------------------**

*~Curve fitting amplifiers
EVFIT1	nvfit1 	0	VOL='(((v(nbistable)+10)/20)*(TMR_G0/(1+(((v(sensor1)-v(nf+))^2)/(Vh^2))))+1)*RP_0'
EVFIT2	nvfit2 	0	VOL='(((-1*v(nbistable)+10)/20)*(TMR_G1/(1+(((v(sensor2)-v(nf+))^2)/(Vh^2))))+1)*RP_1'

*Feedback filter (avoids oscillation)
RSHUNT1 	nvfit1 nvctrl1 1k
CSRL1 	nvctrl1 0 6f
RSHUNT2 	nvfit2 nvctrl2 1k
CSRL2 	nvctrl2 0 6f
.ENDS CPMTJ 
