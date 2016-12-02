** PMAMTJ macro-model 
**
** Authors:
**	Lianhua Qu		
**mingyiweiji@sina.com
** Device Parameter **
** From 'Lanting Cheng, Sakhrat Khizroev, and Ping Liang, "3-Termonal pMTJ Reduces Critical Current and Switching Time", Journal of Magnetism and Magnetic Materials, pp 358-359, Doi: 10.1016/j.jmmm.2014.01.007 '
**  +arfa=0.01$$ damping facotr
**  +tao0=1e-9
**  +pola=0.56 $$ spin polarization factor 
**  +fai=0.4
**  +RA_F=332.2 $$ calculated from RA
**  +Vh=0.5 $$ half voltage
**  +Ms=3.25e5 $$ saturation magnetization
**  +Hk=4e5 $$ anistropy field

** Latest Update:
**>>Fri Mar 25 17:28:02 GMT+7 2016
**>>Wed Apr 20 08:23:40 GMT+7 2016
**>>Thu Sep 29 10:03:22 CST 2016

** -------------------------**
** Declaration of Sub-circuit
** -------------------------**

.SUBCKT PMAMTJ nf+ np- Diameter=40e-9 t_fre=1e-9 t_ox=1e-9 T0=300 JOULE=1 IS=1 TMR_G=1.2 IC_G=1 RA_G=1
** TMR_G, IC_G, RA_G can be used to perform Monte-Carlo simulations.
.PRINT R(GMTJ)

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
+arfa=0.01$$ damping facotr
+tao0=1e-9
+pola=0.56 $$ spin polarization factor 
+fai=0.4
+RA_F=332.2 $$ calculated from RA
+Vh=0.5 $$ half voltage
+Ms=3.25e5 $$ saturation magnetization
+Hk=4e5 $$ anistropy field
+PCAP=0.6e-15 $$ capacity of MTJ pillar
*********temperature parameter**********
.PARAM
+CV=3.47e6
+ttotal=1e-8
+lanmga=1.5
+taoth=(CV*t_ox*ttotal)/lanmga
*********variables**********
.PARAM
+area_de='3.14*pow(Diameter,2)/4'
+VF='area_de*t_fre'
+Mm='Ms*VF'
********calculated variables****
.PARAM
+EF='preme*Ms*Hk*VF*0.5'
+g_app='pola/(2*(1-pola^2))'
+g_pap='pola/(2*(1+pola^2))'
+IC_APP='(2*arfa*gama*electron*EF)/(bohrmag*g_app)*IC_G'
+IC_PAP='(-1)*(2*arfa*gama*electron*EF)/(bohrmag*g_pap)*IC_G'
+delt0='EF/(boltzmamn*T0)'
+tao='tao0*exp(delt0)'
+year='tao/365/24/3600'
+RP='(t_ox*exp(1.025*t_ox*(fai^(0.5))))/(RA_F*pwr(fai,0.5)*area_de)*RA_G'
+RAP='RP*(TMR_G+1)'
.PRINT TRAN PAR(RP) PAR(RAP) PAR(IC_APP) PAR(IC_PAP) PAR(delt0)
** -------------------------**
** MTJ Electrode Connections
** -------------------------**
*voltage controlled TMR
*~Voltage Controlled Resistor
GMTJ nf+ sensor	VCR nvctrl 0 1 MIN='RP' MAX='RAP'

*~Parasitic Capacitance
*CPARA	nf+ sensor	'PCAP'

*~Current Sampling
VSENSOR	sensor np- DC=0

*~Binary Current Sensors
EDRI_APP	ndir_app 0	VOL='exp(5*I(VSENSOR)/IC_APP)-1' MIN=0 MAX=1 
EDRI_PAP	ndir_pap 0	VOL='exp(5*I(VSENSOR)/IC_PAP)-1' MIN=0 MAX=1
*~ tempreture control
EVCHARGE nvcharge 0 VOL='(abs(V(nf+)-V(sensor))*abs(I(VSENSOR))*t_ox/area_de/lanmga)'
EVDRIVER nvdriver 0 VOL='(abs(V(nf+)-V(sensor))>0.01)?v(nvcharge):0v'
RRDRIVER nvdriver nvout 1
CCTEM nvout 0 C='taoth' 
EVDELT nvdelt 0 VOL='EF/(boltzmamn*(T0+(V(nvout)*JOULE)))' 
** -------------------------**
** Decision Circuit
** -------------------------**

*~Current mirrors
GDEC_APP	ndecv+	ndecv-	CUR='(I(VSENSOR)>0)?((abs(I(VSENSOR))<abs(IC_APP))?(pow((tao0*exp(V(nvdelt)*(1-(I(VSENSOR)/IC_APP)))),(-1))*(1e-9)):(((2/(eulerc+log(3.14*3.14*V(nvdelt)/4)))*(bohrmag*pola/(electron*Mm*(1+pola*pola)))*(I(VSENSOR)-IC_APP))*(1e-9))):0'
GDEC_PAP	ndecv-	ndecv+	CUR='(I(VSENSOR)<0)?((abs(I(VSENSOR))<abs(IC_PAP))?(pow((tao0*exp(V(nvdelt)*(1-(I(VSENSOR)/IC_PAP)))),(-1))*(1e-9)):(((2/(eulerc+log(3.14*3.14*V(nvdelt)/4)))*(bohrmag*pola/(electron*Mm*(1+pola*pola)))*(abs(I(VSENSOR))-abs(IC_PAP)))*(1e-9))):0'
GDEC_RETURN	ndecv+ ndecv- CUR='((abs(I(VSENSOR))>1u)||(v(nlimiter+)<0.001)||(v(nlimiter-)<0.001))?0:((abs(v(nlimiter+)-v(nlimiter-))>0.99)?0:((v(nf+)>v(sensor))?-10:10))'
*~Decision capacitors
CDEC-	ndecv-	0 1n IC='(IS<0)?1V:0V'
CDEC+	ndecv+	0 1n IC='(IS<0)?0V:1V'

*~Voltage limiters
RLIM1	ndecv-	nlimiter-	.01
ELIM1	nlimiter-	0	ndecv-	0	1	MIN=0 MAX=1.01 
RLIM2	ndecv+	nlimiter+	.01
ELIM2	nlimiter+	0	ndecv+	0	1	MIN=0 MAX=1.01
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
*.IC v(nbistable)='10v'
*.IC v(nfeedback)='1v'
** -------------------------**
** Curve Fitting Circuit
** -------------------------**

*~Curve fitting amplifiers
EVFIT	nvfit 	0	VOL='(((v(nbistable)+10)/20)*(TMR_G/(1+(((v(sensor)-v(nf+))^2)/(Vh^2))))+1)*RP'

*Feedback filter (avoids oscillation)
RSHUNT5 	nvfit nvctrl 1k
CSRL 	nvctrl 0 6f
.ends PMAMTJ
