/* LTL specifications */
ltl peatonesCruzaranPrincipal {
	[]((botonPeatonesPrincipal && ([]<> deadline)) -> (<> (semaforoPeatonesPrincipal == 2)))
}

ltl peatonesCruzaranSecundario {
	[]((botonPeatonesSecundario && ([]<> deadline)) -> (<> (semaforoPeatonesSecundario == 2)))
}

ltl cochesCruzaranSecundario {
	[]((espira && ([]<> deadline)) -> (<> (semaforoCochesSecundario > 0)))
}

// ltl estadoIdleEsPrincipal {
// 	[]([](!botonPeatonesSecundario && !botonPeatonesPrincipal && !espira && ([]<>deadline)) -> (<>[]semaforoCochesPrincipal == 2))
// }

ltl nuncaDosSemaforosCocheVerde {
	[]!((semaforoCochesPrincipal > 0) && (semaforoCochesSecundario > 0))
}

ltl nuncaVerdePeatonesYCochesPrincipal {
	[]!((semaforoPeatonesPrincipal == 2) && (semaforoCochesPrincipal > 0))
}

ltl nuncaVerdePeatonesYCochesSecundario {
	[]!((semaforoPeatonesSecundario == 2) && (semaforoCochesSecundario > 0))
}

ltl nuncaTodosRojos {
	[]<>!((semaforoCochesSecundario == 0) && (semaforoCochesPrincipal == 0) && (semaforoPeatonesSecundario == 0) && (semaforoPeatonesPrincipal == 0))
} 

/* Inputs */
int botonPeatonesPrincipal;
int botonPeatonesSecundario;
int espira;
int deadline;

/* Outputs */
int semaforoPeatonesPrincipal; // 0 -> rojo, 2 -> verde
int semaforoPeatonesSecundario; // 0 -> rojo, 2 -> verde
int semaforoCochesPrincipal; // 0 -> rojo, 1 -> ambar, 2 -> verde
int semaforoCochesSecundario; // 0 -> rojo, 1 -> ambar, 2 -> verde

/* States */
/*
 * 0 -> Verde principal, rojo secundario (para coches)
 * 1 -> Ambar principal, rojo secundario (para coches)
 * 2 -> Verde secundario, rojo principal (para coches)
 * 3 -> Ambar Secundario, rojo principal (para coches)
*/

active proctype fsm(){
    /* Estado inicial */
    int state = 0;
    semaforoCochesPrincipal = 2;
    semaforoCochesSecundario = 0;
    semaforoPeatonesPrincipal = 0;
    semaforoPeatonesSecundario = 2;

    printf("Estado incial = 0\n");
    printf("Estado = %d, botonPeatonesPrincipal = %d, botonPeatonesSecundario = %d, espira = %d, deadline = %d\n",
    state, botonPeatonesPrincipal, botonPeatonesSecundario, espira, deadline);

    do
    ::if
        ::(state == 0) -> atomic {
            if
            :: (botonPeatonesPrincipal && deadline) -> state = 1; deadline = 0; botonPeatonesPrincipal = 0;
            semaforoCochesPrincipal = 1; semaforoCochesSecundario = 0; semaforoPeatonesPrincipal = 0; semaforoPeatonesSecundario = 2;
            :: (espira && deadline) -> state = 1; deadline = 0; espira = 0;
            semaforoCochesPrincipal = 1; semaforoCochesSecundario = 0; semaforoPeatonesPrincipal = 0; semaforoPeatonesSecundario = 2;

            :: (botonPeatonesSecundario && !deadline) -> state = 0; botonPeatonesSecundario = 0;
            semaforoCochesPrincipal = 2; semaforoCochesSecundario = 0; semaforoPeatonesPrincipal = 0; semaforoPeatonesSecundario = 2;
            fi
        }

        ::(state == 1) -> atomic {
            if
            :: (deadline) -> state = 2; deadline = 0;
            semaforoCochesPrincipal = 0; semaforoCochesSecundario = 2; semaforoPeatonesPrincipal = 2; semaforoPeatonesSecundario = 0;

            :: (botonPeatonesSecundario && !deadline) -> state = 1; botonPeatonesSecundario = 0;
            semaforoCochesPrincipal = 1; semaforoCochesSecundario = 0; semaforoPeatonesPrincipal = 0; semaforoPeatonesSecundario = 2;
            :: (espira && !deadline) -> state = 1; espira = 0;
            semaforoCochesPrincipal = 1; semaforoCochesSecundario = 0; semaforoPeatonesPrincipal = 0; semaforoPeatonesSecundario = 2;
            :: (botonPeatonesPrincipal && !deadline) -> state = 1; botonPeatonesPrincipal = 0;
            semaforoCochesPrincipal = 1; semaforoCochesSecundario = 0; semaforoPeatonesPrincipal = 0; semaforoPeatonesSecundario = 2;
            fi
        }

        ::(state == 2) -> atomic {
            if
            :: (deadline) -> state = 3; deadline = 0;
            semaforoCochesPrincipal = 0; semaforoCochesSecundario = 1; semaforoPeatonesPrincipal = 2; semaforoPeatonesSecundario = 0;

            :: (espira && !deadline) -> state = 2; espira = 0;
            semaforoCochesPrincipal = 0; semaforoCochesSecundario = 2; semaforoPeatonesPrincipal = 2; semaforoPeatonesSecundario = 0;
            :: (botonPeatonesPrincipal && !deadline) -> state = 2; botonPeatonesPrincipal = 0;
            semaforoCochesPrincipal = 0; semaforoCochesSecundario = 2; semaforoPeatonesPrincipal = 2; semaforoPeatonesSecundario = 0;
            fi
        }

        ::(state == 3) -> atomic {
            if
            :: (deadline) -> state = 0; deadline = 0;
            semaforoCochesPrincipal = 2; semaforoCochesSecundario = 0; semaforoPeatonesPrincipal = 0; semaforoPeatonesSecundario = 2;

            :: (botonPeatonesSecundario && !deadline) -> state = 3; botonPeatonesSecundario = 0;
            semaforoCochesPrincipal = 0; semaforoCochesSecundario = 1; semaforoPeatonesPrincipal = 2; semaforoPeatonesSecundario = 0;
            :: (espira && !deadline) -> state = 3; espira = 0;
            semaforoCochesPrincipal = 0; semaforoCochesSecundario = 1; semaforoPeatonesPrincipal = 2; semaforoPeatonesSecundario = 0;
            :: (botonPeatonesPrincipal && !deadline) -> state = 3; botonPeatonesPrincipal = 0;
            semaforoCochesPrincipal = 0; semaforoCochesSecundario = 1; semaforoPeatonesPrincipal = 2; semaforoPeatonesSecundario = 0;
            fi
        }
    fi;

    printf("Estado = %d, botonPeatonesPrincipal = %d, botonPeatonesSecundario = %d, espira = %d, deadline = %d\n",
    state, botonPeatonesPrincipal, botonPeatonesSecundario, espira, deadline);

    od

}

active proctype entorno(){
    do
    :: skip -> skip
    :: deadline = 1
    :: botonPeatonesPrincipal = 1
    :: botonPeatonesSecundario = 1
    :: espira = 1
    od
}