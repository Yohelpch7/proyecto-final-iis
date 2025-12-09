#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CURSOS 50
#define MAX_ALUMNOS 15
#define MIN_APROBATORIO 7
#define ARCHIVO_CURSOS "cursos.dat"

//
typedef struct {
    char matricula[15];
    char nombre[30];
    char apellido_p[30];
    char apellido_m[30];
    float calificacion;
} Alumno;

typedef struct {
    char nombre[30];
    char profesor[50];
    int clave;
    int total_inscritos;
    Alumno *alumnos; // arreglo dinámico
} Curso;

//
Curso *cursos = NULL;   // arreglo dinámico de cursos
int total_cursos = 0;
int capacidad_cursos = MAX_CURSOS;

// Registro de alumnos registrados
Alumno registrados[200];
int total_registrados = 0;


void cargarDatos();
void guardarDatos();
void registrarAlumno();
void altaCurso();
void inscribirAlumnoCurso();
void listarAlumnosCurso();
void promedioCurso();
void boletaAlumno();
void cursoConMasAprobados();
void pruebas();


int main() {

    cursos = (Curso*) malloc(sizeof(Curso) * capacidad_cursos);

    if (cursos == NULL) {
        printf("Error al asignar memoria.\n");
        return 1;
    }

    cargarDatos();

    int opcion = -1;
    while (opcion != 0) {
        printf("\n=== SISTEMA ESCOLAR ===\n");
        printf("1. Registrar estudiante\n");
        printf("2. Dar de alta curso\n");
        printf("3. Inscribir estudiante a curso\n");
        printf("4. Lista de estudiantes por curso\n");
        printf("5. Promedio de un curso\n");
        printf("6. Boleta de estudiante\n");
        printf("7. Curso con más aprobados\n");
        printf("8. Ejecutar pruebas\n");
        printf("0. Guardar y salir\n");
        printf("Opción: ");
        if (scanf("%d", &opcion) != 1) {
            while (getchar() != '\n');
            opcion = -1;
            continue;
        }
        while (getchar() != '\n');

        switch (opcion) {
            case 1: registrarAlumno(); break;
            case 2: altaCurso(); break;
            case 3: inscribirAlumnoCurso(); break;
            case 4: listarAlumnosCurso(); break;
            case 5: promedioCurso(); break;
            case 6: boletaAlumno(); break;
            case 7: cursoConMasAprobados(); break;
            case 8: pruebas(); break;
            case 0: guardarDatos(); break;
            default: printf("Opción no válida.\n");
        }
    }

    // liberar memoria dinámica
    for (int i = 0; i < total_cursos; i++) {
        free(cursos[i].alumnos);
    }
    free(cursos);

    return 0;
}

/* 
   1) cargarDatos() - Carga simple con fread
    */
void cargarDatos() {

    FILE *f = fopen(ARCHIVO_CURSOS, "rb");
    if (!f) {
        total_cursos = 0;
        return;
    }

    fread(&total_cursos, sizeof(int), 1, f);

    for (int i = 0; i < total_cursos; i++) {

        // leer el curso (sin alumnos)
        fread(&cursos[i], sizeof(Curso) - sizeof(Alumno*), 1, f);

        // crear arreglo dinámico para alumnos
        cursos[i].alumnos = (Alumno*) malloc(sizeof(Alumno) * MAX_ALUMNOS);

        // leer alumnos inscritos
        fread(cursos[i].alumnos, sizeof(Alumno), cursos[i].total_inscritos, f);
    }

    fclose(f);
}

/* 
   2) guardarDatos() - Guarda struct simple con alumnos
 */
void guardarDatos() {
    FILE *f = fopen(ARCHIVO_CURSOS, "wb");
    if (!f) {
        printf("No se pudo guardar.\n");
        return;
    }

    fwrite(&total_cursos, sizeof(int), 1, f);

    for (int i = 0; i < total_cursos; i++) {

        // guardamos curso (sin el puntero)
        fwrite(&cursos[i], sizeof(Curso) - sizeof(Alumno*), 1, f);

        // guardamos alumnos reales inscritos
        fwrite(cursos[i].alumnos, sizeof(Alumno), cursos[i].total_inscritos, f);
    }

    fclose(f);
    printf("Datos guardados.\n");
}

/*
   3) registrarAlumno()
*/
void registrarAlumno() {

    if (total_registrados >= 200) {
        printf("Límite alcanzado.\n");
        return;
    }

    Alumno a;

    printf("Matrícula: ");
    scanf("%14s", a.matricula);
    while (getchar() != '\n');

    for (int i = 0; i < total_registrados; i++) {
        if (strcmp(registrados[i].matricula, a.matricula) == 0) {
            printf("Ya existe esa matrícula.\n");
            return;
        }
    }

    printf("Nombre: ");
    scanf("%29s", a.nombre);
    while (getchar() != '\n');

    printf("Apellido paterno: ");
    scanf("%29s", a.apellido_p);
    while (getchar() != '\n');

    printf("Apellido materno: ");
    scanf("%29s", a.apellido_m);
    while (getchar() != '\n');

    a.calificacion = 0;

    registrados[total_registrados++] = a;

    printf("Alumno registrado.\n");
}

/* 
   4) altaCurso()
  */
void altaCurso() {

    if (total_cursos >= capacidad_cursos) {
        printf("No se pueden crear más cursos.\n");
        return;
    }

    Curso c;

    printf("Clave del curso: ");
    scanf("%d", &c.clave);
    while (getchar() != '\n');

    for (int i = 0; i < total_cursos; i++) {
        if (cursos[i].clave == c.clave) {
            printf("Esa clave ya existe.\n");
            return;
        }
    }

    printf("Nombre del curso (sin espacios): ");
    scanf("%29s", c.nombre);
    while (getchar() != '\n');

    printf("Profesor (sin espacios): ");
    scanf("%49s", c.profesor);
    while (getchar() != '\n');

    c.total_inscritos = 0;

    // ALUMNOS DINÁMICOS (requisito)
    c.alumnos = (Alumno*) malloc(sizeof(Alumno) * MAX_ALUMNOS);

    cursos[total_cursos++] = c;

    printf("Curso creado.\n");
}

/* 
   5) inscribirAlumnoCurso()
   */
void inscribirAlumnoCurso() {

    int clave;
    char mat[15];

    printf("Clave del curso: ");
    scanf("%d", &clave);
    while (getchar() != '\n');

    int ic = -1;
    for (int i = 0; i < total_cursos; i++) {
        if (cursos[i].clave == clave) { ic = i; break; }
    }

    if (ic == -1) {
        printf("Curso no encontrado.\n");
        return;
    }

    if (cursos[ic].total_inscritos >= MAX_ALUMNOS) {
        printf("Curso lleno.\n");
        return;
    }

    printf("Matrícula del estudiante: ");
    scanf("%14s", mat);
    while (getchar() != '\n');

    int ir = -1;
    for (int i = 0; i < total_registrados; i++) {
        if (strcmp(registrados[i].matricula, mat) == 0) { ir = i; break; }
    }

    if (ir == -1) {
        printf("Estudiante no registrado.\n");
        return;
    }

    // copiar datos del alumno registrado
    cursos[ic].alumnos[cursos[ic].total_inscritos] = registrados[ir];
    cursos[ic].alumnos[cursos[ic].total_inscritos].calificacion = 0;

    cursos[ic].total_inscritos++;

    printf("Estudiante inscrito.\n");
}

/* 
   6) listarAlumnosCurso()
  */
void listarAlumnosCurso() {

    int clave;
    printf("Clave del curso: ");
    scanf("%d", &clave);
    while (getchar() != '\n');

    int ic = -1;
    for (int i = 0; i < total_cursos; i++) {
        if (cursos[i].clave == clave) { ic = i; break; }
    }

    if (ic == -1) {
        printf("Curso no encontrado.\n");
        return;
    }

    if (cursos[ic].total_inscritos == 0) {
        printf("No hay alumnos.\n");
        return;
    }

    printf("Alumnos del curso %s:\n", cursos[ic].nombre);

    for (int j = 0; j < cursos[ic].total_inscritos; j++) {
        Alumno a = cursos[ic].alumnos[j];
        printf("%s %s %s [%s] Cal: %.2f\n",
            a.nombre, a.apellido_p, a.apellido_m, a.matricula, a.calificacion);
    }
}

/* 
   7) promedioCurso()
*/
void promedioCurso() {

    int clave;
    printf("Clave del curso: ");
    scanf("%d", &clave);
    while (getchar() != '\n');

    int ic = -1;
    for (int i = 0; i < total_cursos; i++) {
        if (cursos[i].clave == clave) { ic = i; break; }
    }

    if (ic == -1) {
        printf("Curso no encontrado.\n");
        return;
    }

    if (cursos[ic].total_inscritos == 0) {
        printf("No hay alumnos.\n");
        return;
    }

    float suma = 0;
    for (int j = 0; j < cursos[ic].total_inscritos; j++) {
        suma += cursos[ic].alumnos[j].calificacion;
    }

    float prom = suma / cursos[ic].total_inscritos;

    printf("Promedio: %.2f\n", prom);
}

/* 
   8) boletaAlumno()
  */
void boletaAlumno() {

    char mat[15];

    printf("Matrícula: ");
    scanf("%14s", mat);
    while (getchar() != '\n');

    int ir = -1;
    for (int i = 0; i < total_registrados; i++) {
        if (strcmp(registrados[i].matricula, mat) == 0) { ir = i; break; }
    }

    if (ir == -1) {
        printf("Estudiante no registrado.\n");
        return;
    }

    printf("Boleta de %s %s %s:\n",
           registrados[ir].nombre,
           registrados[ir].apellido_p,
           registrados[ir].apellido_m);

    int found = 0;

    for (int i = 0; i < total_cursos; i++) {
        for (int j = 0; j < cursos[i].total_inscritos; j++) {
            if (strcmp(cursos[i].alumnos[j].matricula, mat) == 0) {
                found = 1;
                float cal = cursos[i].alumnos[j].calificacion;
                printf("%s | Calificación: %.2f | %s\n",
                       cursos[i].nombre,
                       cal,
                       (cal >= MIN_APROBATORIO ? "Aprobado" : "Reprobado")
                );
            }
        }
    }

    if (!found) printf("No está inscrito en cursos.\n");
}

/*
   9) cursoConMasAprobados()
  */
void cursoConMasAprobados() {

    if (total_cursos == 0) {
        printf("No hay cursos.\n");
        return;
    }

    int mejor = -1;
    int maxA = -1;

    for (int i = 0; i < total_cursos; i++) {
        int aprob = 0;

        for (int j = 0; j < cursos[i].total_inscritos; j++) {
            if (cursos[i].alumnos[j].calificacion >= MIN_APROBATORIO)
                aprob++;
        }

        if (aprob > maxA) {
            maxA = aprob;
            mejor = i;
        }
    }

    if (mejor == -1) {
        printf("No hay aprobados.\n");
        return;
    }

    printf("Curso con más aprobados: %s (clave %d)\n",
           cursos[mejor].nombre,
           cursos[mejor].clave);
}

/* =====================================================
   10) pruebas()
   ===================================================== */
void pruebas() {
    printf("Pruebas simples:\n");
    printf("- Prueba registrar: crear alumno nuevo\n");
    printf("- Prueba alta curso: crear curso y luego listar\n");
    printf("- Prueba inscribir: registrar alumno y meterlo a curso\n");
    printf("(Solo prints, como versión sencilla.)\n");
}