clean:
	rm -f pan* _spin_nvr.tmp model.pml.trail

spin:
	spin -a model.pml
	gcc -o pan pan.c
	./pan -a -f -N peatonesCruzaranPrincipal
	./pan -a -f -N peatonesCruzaranSecundario
	./pan -a -f -N cochesCruzaranSecundario
	./pan -a -f -N nuncaDosSemaforosCocheVerde
	./pan -a -f -N nuncaVerdePeatonesYCochesPrincipal
	./pan -a -f -N nuncaVerdePeatonesYCochesSecundario
	./pan -a -f -N nuncaTodosRojos
	./pan -a -f -N estadoIdleEsPrincipal
