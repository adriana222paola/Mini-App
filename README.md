# Mini-App: Sistema de Turnos

## Descripcion: 
Mini‑App hecha en C++ para un sistema de turnos (citas) con roles de **administrador** y **usuario**.  
Incluye funciones de seguridad como **hashing de contraseñas con bcrypt**, **validación de entradas** para prevenir inyecciones y desbordamientos, y **manejo de errores con logging** para trazabilidad y auditoría.  

---

## Autor:
- Proyecto de **Adriana P. Perez Lopez**  
- Correo institucional: **aperez2361@interbayamon.edu**
ദ്ദി( • ᴗ - ) ✧

---

## Instrucciones de ejecución
1. **Clonar el repositorio:**
   ```bash
   git clone https://github.com/usuario/mi-proyecto-citas.git
2. Comandos a seguir para ejectura el código:
   Luego de estar en la carpeta de Mini-App favor de seguir los siguientes comandos. 
   ```bash
   cd .\Mini-App\
   cl /EHsc Mini-App.cpp bcrypt.cpp blowfish.cpp /Fe:Mini-App.exe
   .\Mini-App.exe

## Ejemplos de Entrada y Salida
- Registro/Login:
  
  Choose username: PeterLaAnguila
  
  Choose password: password123
  
  User registered successfully. 

- Create Appointment:
  
  Enter year (YYYY): 2025
  
  Enter month (MM): 12
  
  Enter day (DD): 02
  
  Enter time (HH:MM): 18:30
  
  Appointment created successfully.

## Seguridad Implementada
- Control de Acceso: Roles de Admin y User.
- Manejo de Contraseñas: Cifrado con bycrypt.
- Validación de entradas: Protección contra SQL Injection y overflow/underflow.
- Manejo de Errores: Mensajes seguros y registro en logfile.txt.

## Threat Model
El archivo (pdf) del Threat Model en donde esta las imagenes de el diagrama de seguridad y la tabla d seguridad, se encuentran dentro de /Mini-App/ThreatModel/ThreatModel.pdf

## Archivos (Los Assets):
**users.txt:** Guarda los credenciales.
  
Ejemplo: 

  'User1|$2b$10$ZaZeCgy0sC3n52uXcJgTD.GcF5bcvIhJbHid9OkuKH.4l/8dCwEc2|user'
  
**appointments.txt:** Guarda los turnos/ citas.
  
Ejemplo: 

  'User1|2025-12-01 12:00|Attended'
  
**logfile.txt:** Registra la actividad del usuario.
  
Ejemplo: 

  '2025-11-26 19:02|Admin|Choose EXIT option.'
