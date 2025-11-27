// Adriana P. Perez 
// Mini-App: Appointments

// Librerias
#include <iostream>
#include <fstream> // Archivos ifstream y ofstream
#include <sstream> // Utilizamos istringstream (leer) y ostringstream (escribir)
#include <string> // funciones como .size()
#include <ctime> // Las funciones de tiempo y fecha.  
#include <vector> // Utilizamos Vectores y funciones como .push_back()
#include <iomanip> // FUnciones setw() y setfill()
#include <limits> // Limite de datos, numeric_limits<int>::max() limitamos el valor maximo de int.
#include "bcrypt.h" // Libreria Externa de hashing

using namespace std;

// isSafeInput() para validar los inputs de una manera segura. 
// Limita la longitud de caracteres, y BLOQUEA caracteres usad os en SQL Injections. 
bool isSafeInput(const string& input, size_t maxLength = 50) {
    // Paremetros son input de string con un maximo de 50 caracteres/
    // variable: 
    string antiSQL = "'\";--/*\\%=()";
    // Estos son los caracteres contra SQL Injections. 
    if (input.length() > maxLength) {
        // Rechar inputs largos, x > 50 caracteres. 
        return false;
    }
    for (char c : input)
        if (antiSQL.find(c) != string::npos) {
            // Lee cada caracter del input .find() string::npos (se utilizan juntos)
            // Si se encuentra uno de los caraceteres se devuleve falso. 
            return false;
        }
    return true;
} 

// Tiempo: Hora y Minutos (HH:MM)
bool isValidTime(const string& timePart) {
    // Variables:
    int hour, minute; 
    char colon; // : 
    if (!isSafeInput(timePart, 5)) {
        // Parametros el input del tiempo que ingreso el usuario, y el limite max. de caracteres. 
        // 00:00
        return false;
        // Si la entrada no es valida, devuelve falso. 
    }  
    istringstream ss(timePart); 
    // Creeamos un string stream (ss) para la variable de tiempo. 
    ss >> hour >> colon >> minute;
    // Se leen los datos del tiempo en orden.
    return !ss.fail() && colon == ':' // Verificar que no hallan errores en el ss, y el uso de : para seprarar la hora y minutos. 
        && hour >= 0 && hour < 24 // Rango de hora, hora militar.
        && minute >= 0 && minute < 60; // Rango de minutos. 
}

// Fecha:
string getSafeDateInput() {
    // Variables: 
    int year, month, day;

    cout << "Enter year (YYYY): ";
    cin >> year;
    if (cin.fail() || year < 2025 || year > 2050) {
        // .fail() en caso de que no se entre un int. 
        // Verificamos el rango del año. 
        cout << "Invalid year." << endl;
        // Mensajito de error!
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        // .clear() limpiamos el input y se descartan caracteres no deseables. 
        return ""; // Se devuelve al no poder ingresar el año correcto. 
    }

    // Mes y Dia tambien esta diseñadas a funcionar de la misma manera que el AÑO. 

    cout << "Enter month (MM): ";
    cin >> month;
    if (cin.fail()  || month < 1 || month > 12) {
        cout << "Invalid month." << endl;
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return "";
    }

    cout << "Enter day (DD): ";
    cin >> day;
    if (cin.fail() || day < 1 || day > 31) {
        cout << "Invalid day." << endl;
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return "";
    }


    ostringstream dateStream; // Escribe en memoria la fecha. 
    dateStream << setw(4) << setfill('0') << year << "-"
        // setw(4) aseguramos que el año tenga 4 caracteres. 
        << setw(2) << setfill('0') << month << "-"
        << setw(2) << setfill('0') << day;
        // setw(2) asegurar que el mes y dia tenga dos caracteres. 
        // setfill(0) rellena con 0 en caso de que falten numeros.
        // se añade el guion (-) para separar la yyyy-mm-dd

    return dateStream.str(); // Conversion del flujo a un string, y se devuelve. 
}

// La funcion de userExistsla usamos en update y delete appointments. 
// Verifica que el Username ingresado sea valido. 
bool userExists(const string& username) {

    if (!isSafeInput(username)) {
        // Comprobamos que el input sea valido. 
        return false;
    }

    ifstream infile("users.txt"); // abrimos el archivo de users.txt
    string line; 
    // Loop para leer cada linea del archivo, y buscar al usuario. 
    while (getline(infile, line)) {
        stringstream ss(line);
        string existingUser, hashedPw, existingRole;
        getline(ss, existingUser, '|');
        getline(ss, hashedPw, '|');
        getline(ss, existingRole, '|');

        if (existingUser == username) {
            // True si el usuario exsite.
            return true;
        }
    }
    // False de que no exista. 
    return false;
}

// Estructura de datos del usuario.
struct User {
    // Variables: 
    string username; 
    string password;
    string role; // admin/ user
};

// Obtener fecha/hora actual
// Utilizamos la funcion de getCurrentDateTime, para luego en el logfile poder registrar las actividade del usuario, en este caso la hora y fecha de su accion. 
// COMENTAR
string getCurrentDateTime() {
    time_t now = time(0);
    tm ltm{};
#if defined(_MSC_VER)
    localtime_s(&ltm, &now);
#else
    localtime_r(&now, &ltm);
#endif
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &ltm);
    return string(buffer);
}

// Archivo de Logfile: 
// Archivo en donde regsitramos la actiidad del usuario. 
void logAction(const string& username, const string& action) {
    // Capturamos el UserName junto a su Accion. 
    ofstream logfile("logfile.txt", ios::app);
    // Abrimos el archivo de logfile.txt.
    // ios::app, cada vez que se añaden datos en el logfile, no se borra ningun dato anterior. 
    logfile << getCurrentDateTime() << "|" << username << "|" << action << endl;
    // Los datos que guardamos en el logfile son, la fecha de la actvidad, el UserName y la Accion. 
}

// 24hrs: 
// Utilizamos la funcion de isMoreThan24Hours   
bool isMoreThan24Hours(const string& datetime) {
    // Variables: 
    tm appointment = {}; // Estructura que guarda la fecha y tiempo. 

    istringstream ss(datetime); // De string a flujo. 
    ss >> get_time(&appointment, "%Y-%m-%d %H:%M"); // Se guarda el datetime dentro de appointment. 

    if (ss.fail()) { 
        // En caso de que el formato sea incorrecto, manda un mensajito de error. 
        cout << "Invalid date format." << endl;
        return false;
    }

    time_t appointmentTime = mktime(&appointment);
    // Conversion de fecha y tiempo (fecha y hora) en segundos. 

    if (appointmentTime == -1) {
        // En caso de que halla un error, como por ejemplo que ingresen que el mes sea 20
        // se envia un mensaje de error de conversion de datetime a los segundos. 
        cout << "Error converting appointment time." << endl;
        return false;
    }

    time_t now = time(0);
    // Devuelve el tiempo actual en segundos 

    if (appointmentTime > std::numeric_limits<time_t>::max() ||
        appointmentTime < std::numeric_limits<time_t>::min()) {
        // Se verifica que appointmentTime se encuentre dentro de los limites, maximo y minimo. 
        cout << "Error detected in appointment time." << endl;
        // Error de Overflow/Underflow!
        // max() overflow protege de que la fecha/tiempo en segundos no pase de maximo.
        // min() underflow protege de que la fecha/tiempo en segundos no pase de minimo.
        return false;
    }

    if (now > std::numeric_limits<time_t>::max() ||
        now < std::numeric_limits<time_t>::min()) {
        // Mismo proceso de proteccion al igual que appointmentTime pero con el tiempo actual. 
        cout << "Error detected in current time." << endl;
        // // Error de Overflow/Underflow!
        return false;
    }

    double diff = difftime(appointmentTime, now);
    // Se calcula la diferencia de los segundos entre el datetime y now.

    if (diff < 0) {
        // Si el diff es (-) entonces el turno ya paso. (underflow)
        cout << "The appointment is already in the past." << endl;
        return false;
    }

    if (diff > static_cast<double>(std::numeric_limits<int>::max())) {
        // Porteccion en caso de que el datetime sea muy largo. 
        cout << "The time difference too large." << endl;
        return false;
    }

    return diff > 86400; // 24 horas en segundos
    // Si diff es mayor que 24hrs, puedes actulizar/ borrar el turno.
    // De lo contrario no puedes. 
}

// Registro: 
void registerUser() {
    // Variables:
    string username, password;

    cout << "Choose username: ";
    cin >> username;
    if (!isSafeInput(username)) {
        // Validar input del username. 
        cout << "Invalid username." << endl;
        return;
    }

    // Validar que el usuario ya existe
    ifstream infile("users.txt");
    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        string existingUser, hashedPw, existingRole;
        getline(ss, existingUser, '|');
        getline(ss, hashedPw, '|');
        getline(ss, existingRole, '|');

        if (existingUser == username) {
            // Mensajito de error!
            cout << "Error: Username already exists. Please choose another." << endl;
            return;
        }
    }
    infile.close();

    cout << "Choose password: ";
    cin >> password;

    if (!isSafeInput(password)) {
        // Validar input de password. b  
        cout << "Invalid password." << endl;
        return;
    }

    // Asigna el rol user a los que se registren. 
    string role = "user";

    // Conversion del string password a un hash. 
    string hashedPassword = bcrypt::generateHash(password);
    ofstream file("users.txt", ios::app); // agregamos el usuario. 
    file << username << "|" << hashedPassword << "|" << role << endl;
    file.close(); 

    cout << "User registered successfully.";
    // Registrar Actividad: 
    logAction(username, "Register.");
}

// Iniciar sesion: 
User login() {
    // Variables: 
    string username, password;

    // Input de Username: 
    cout << "Username: ";
    cin >> username;

    if (!isSafeInput(username)) { 
        // Se valida con la funcion de isSafeInput el username. 
        cout << "Invalid username." << endl; 
        logAction(username, "Invalid username during Login.");
        return { "", "", "" }; 
        // En caso de que sea invalido, se devuelve un objeto vacio. 
    }

    // Input de Password:
    cout << "Password: ";
    cin >> password;

    if (!isSafeInput(password)) { 
        // Se valida con la funcion de isSafeInput el password. 
        cout << "Invalid password." << endl; 
        logAction(username, "Invalid password during Login.");
        // En caso de que sea invalido, se devuelve un objeto vacio. 
        return { "", "", "" }; 
    }

    // Variables:
    ifstream file("users.txt"); // Se abre el archivo de usuarios, lugar en donde se encunetra el username, password y el rol 
    string line; 

    while (getline(file, line)) {
        // Se lee una linea del users.txt y se guarda en line. 
        // Variables: 
        stringstream ss(line);
        string un, hashedPw, role;
        
        // '|' es el separador de los datos dentro del users.txt
        // username|password|rol 
        getline(ss, un, '|');
        getline(ss, hashedPw, '|');
        getline(ss, role, '|');
        // por lo tanto se buscan/ leen cada variable separadas por '|'

        if (un == username && bcrypt::validatePassword(password, hashedPw)) {
            // Se comparan las variables ingresadas con las que se encuentran en el archivo. 
            logAction(username, "login");
            // Se registra en el archivo de logfile que el usuario acaba de ingresar. 
            return { un, hashedPw, role };
        }
    }

    // En caso de que el login falle. 
    cout << "Invalid username or password." << endl;
    logAction(username, "Failed Login attempt");
    return { "", "", "" };
}

// Crear cita:
// Se crear la cita y se guarda en appointments.txt
void createAppointment(const string& username) {
    // Variables/Funciones: 
    string date = getSafeDateInput(); // Se ingresa la fecha. 
    string timePart;
  
    if (date.empty()) {
        cout << "Invalid date." << endl;
        logAction(username, "Invalid date during appointment creation.");
        // En caso de que la fecha sea invalida. 
        return;
    }

    // Input de tiempo. 
    cout << "Enter time (HH:MM): ";
    cin >> timePart;
    if (!isValidTime(timePart)) { 
        // Se valida con la funcion isValidTime de que la hora este correcta. 
        cout << "Invalid time format." << endl;
        logAction(username, "Invalid time during appointment creation.");
        // Mesajito de error!
        return; 
    }
    
    ofstream file("appointments.txt", ios::app); // Se abre el archivo. 
    // Se guarda la cita en appointments.txt
    file << username << "|" << date << " " << timePart << "|Hasn't Attended" << endl;
    // La cita de añade como Username|Fecha|Tiempo|Asistencia. 
    file.close(); // Se cierra el archivo.

    cout << "Appointment created successfully." << endl;
    logAction(username, "create appointment");
    // Se registra en el logfile.txt la actvidad del usuario. 
}

// Leer citas:
// Admin: Puedes leer todas las citas.
// User: Solo puede su cita. 
void readAppointments(const string& username, const string& role) {
    ifstream file("appointments.txt"); // se abre el archivo de appointments. 
    string line;

    cout << endl << "Appointments: " << endl;
    while (getline(file, line)) {
        // Lee las citas del archivo. 
        // Variables: 
        stringstream ss(line);
        string user, datetime, attended;
        getline(ss, user, '|');
        getline(ss, datetime, '|');
        getline(ss, attended, '|');

        
        string fecha = (datetime.size() >= 10) ? datetime.substr(0, 10) : datetime;
        // Se busca la fecha. 
        string hora = (datetime.size() >= 16) ? datetime.substr(11, 5) : "";
        // Se busca el tiempo.

        // Se muestran las citas segun el rol admin/user: 
        if (role == "admin" || user == username) {
            cout << "Username: " << user
                << " | Appointment Date: " << fecha
                << " & Time: " << hora
                << " | Attendance: " << attended << endl;
        }
    }

    logAction(username, "read appointments");
    // Se registra la actividad del usuario. 
}

// Actualizar cita:
// Admin: Puede actualizar la cita o marcar la asistencia del usuario.
// User: Actuliza su cita.
// Se tiene que actulizar la cita antes de 24hrs del turno. 
void updateAppointment(const string& username, const string& role) {
    // Variables: 
    string targetUser = username;

    // Control de admin: 
    if (role == "admin") {
        cout << "Enter username of appointment to update: ";
        cin >> targetUser;

        // El admin busca el usuario que quiera actualizar. 

        if (!isSafeInput(targetUser)) {
            // Verificar que el input sea valido. 
            cout << "Invalid username." << endl;
            logAction(username, "Invalid username during appointment update.");
            // Mensajito en caso de que el usario sea invalido!
            return;
        }

        if (!userExists(targetUser)) {
            // Verificamos que el Username exista.
            cout << "User not found." << endl;
            // Mensajito en caso de quen o se encuentre.
            // Registrar la actividad: 
            logAction(username, "Username not found during appointment update.");
            return;
        }
    }

    string targetDate = getSafeDateInput();
    // Se ingresa la fecha
    if (targetDate.empty()) {
        cout << "Invalid date." << endl;
        logAction(username, "Invalid date during appointment update.");
        // Se termina la funcion en caso de que la fecha sea invalida!
        return;
    }

    ifstream file("appointments.txt"); // Se abre el archivo de appointments. 
    // Variables: 
    vector<string> lines; // En un vector se almacenan los datos (por linea) del archivo de appointments.
    string line;
    bool updated = false;

    while (getline(file, line)) {
        // Loop que se encarga de leer cada linea dentro del arhivo de appointments.
        // Busca el usuario, la fecha de la cita que se quiera actualizar.
        stringstream ss(line);
        string user, datetime, attended;
        getline(ss, user, '|');
        getline(ss, datetime, '|');
        getline(ss, attended, '|');

        string storedDate = datetime.substr(0, 10); 
        // Se extrae la fecha y se compara con la ingresada.

        if (user == targetUser && storedDate == targetDate) {
            // Se procede a actulizar la cita si todos los datos coinciden. 
            if (role != "admin" && !isMoreThan24Hours(datetime)) {
                // Solo el admin puede actulizar la cita en menos de las 24hrs, no el usuario. 
                cout << "You can only update appointments more than 24 hours in advance." << endl;
                lines.push_back(line);
                continue;
            }

            // Luego de el admin/ usuario buscar los datos, se da permiso de actulizar la cita. 
            
            // Control del Admin: 
            if (role == "admin") {
                int adminChoice;
                // El admin tiene las opciones de Actulizar la cita o Marcar la Asistencia. 
                cout << endl << "Admin options:" << endl;
                cout << "1. Update appointment date & time" << endl;
                cout << "2. Mark attendance" << endl;
                cout << "Choice: ";
                cin >> adminChoice;

                // Opciones del admin: 
                if (adminChoice == 1) {
                    // 1 = Actulizar Cita. 
                    string newDate = getSafeDateInput();
                    // Se ingresa una nueva fecha para la cita. 
                    if (newDate.empty()) { 
                        lines.push_back(line); 
                        // En caso de que no se detecte una fecha salida, no se modifica la cita. 
                        break; 
                    }

                    // Mismo proceso de la fecha pero con el tiempo, HH:MM.
                    string newTime;
                    cout << "Enter new time (HH:MM): ";
                    cin >> newTime;
                    if (!isValidTime(newTime)) { 
                        cout << "Invalid time format." << endl;
                        logAction(username, "Invalid time during appointment update.");
                        lines.push_back(line); 
                        break; 
                    }

                    // Se reinicia la asistencia del usuario cada vez que se actulize la fecha. 
                    lines.push_back(user + "|" + newDate + " " + newTime + "|Hasn't Attended");
                    updated = true; // Se modifico la cita
                    // Se registra en el archivo de logfile la actvidad del admin. 
                    logAction(username, "Admin updated appointment for " + user);
                    break; 

                } else if (adminChoice == 2) {
                    // 2 = Marcar la asistencia. 
                    string mark;
                    cout << "Mark attendance (yes/no): ";
                    cin >> mark;

                    // Estado de la asistencia:
                    string status = (mark == "yes") ? "Attended" : "Hasn't Attended";
                    // Si se marca "yes" ? entonces si asistio : de lo contrario no asistio. 
                    lines.push_back(user + "|" + datetime + "|" + status);
                    updated = true; // Se actuliza la cita con los nuevos datos. 
                    logAction(username, "admin marked attendance for " + user);
                    // Se registra la actividad del admin. 
                    break;

                } else {
                    // Mensajito de Error!
                    cout << "Invalid choice." << endl;
                    logAction(username, "Invalid admin choice during appointment update.");
                    lines.push_back(line);
                    break; 
                }

                // Cada break; se utiliza para salir del loop while... 

            // Control del Usario: 
            } else {
                // Se ingresa la nueva fecha de la cita.
                // Mismo proceso que tiene el admin pero del usuario. 
                string newDate = getSafeDateInput();
                if (newDate.empty()) { 
                    cout << "Invalid date." << endl;
                    logAction(username, "Invalid date during appointment update.");
                    lines.push_back(line); 
                    break; 
                }

                // Se actuliza el tiempo. 
                string newTime;
                cout << "Enter new time (HH:MM): ";
                cin >> newTime;
                if (!isValidTime(newTime)) { 
                    cout << "Invalid time." << endl;
                    logAction(username, "Invalid time during appointment update.");
                    lines.push_back(line); 
                    break; 
                }

                lines.push_back(user + "|" + newDate + " " + newTime + "|" + attended);
                updated = true; // Se guardar la cita actualizada. 
                logAction(username, "update appointment");
                // Se registra la actividad del usuario. 
                break; 
            }
        } else {
            lines.push_back(line);
        }
    }
    file.close(); // Se cierra el archivo. 

    if (updated) {
        // Se actualiza la cita en el archivo de appointments. 
        ofstream out("appointments.txt"); 
        // Loop recorre las lineas buscando los datos correspondientes y actualkiza la cuita. 
        for (size_t i = 0; i < lines.size(); i++) {
            out << lines[i] << endl;
        }
        cout << "Appointment updated successfully." << endl;
    }
    else {
        cout << "No matching appointment found." << endl;
        logAction(username, "No matching appointment found during update.");
    }
}

// Borrar cita: 
void deleteAppointment(const string& username, const string& role) {
    string targetUser = username;
    // Por default el usuario es el que tiene acceso a ser de que sea el admin, entocnes se les da los controles especificos al mismo. 
    // Control de Admin: 
    if (role == "admin") {
        // Se pide el usuario al que se quiera borra la cita. 
        cout << "Enter username of appointment to delete: ";
        cin >> targetUser;
        if (!isSafeInput(targetUser)) {
            // Mensajito de error!
            cout << "Invalid username." << endl;
            logAction(username, "Invalid username during appointment deletion.");
            return;
        }

        if (!userExists(targetUser)) {
            //  Verificar que el usuario exista. 
            cout << "User not found." << endl;
            logAction(username, "Username not found during appointment deletion");
            return; 
        }
    }

    int year, month, day;
    // Se pide la fecha de la cita que se quiera borar. 
    cout << "Please, insert date you wan't to delete: " << endl;
    cout << "Enter year (YYYY): ";
    cin >> year;
    cout << "Enter month (MM): ";
    cin >> month;
    cout << "Enter day (DD): ";
    cin >> day;

    ostringstream dateStream;
    // Se formatea la fecha con los guioenes.
    dateStream << setw(4) << setfill('0') << year << "-"
        << setw(2) << setfill('0') << month << "-"
        << setw(2) << setfill('0') << day;
    string targetDate = dateStream.str();

    ifstream file("appointments.txt"); // Se abre el arhivo de appointments. 
    vector<string> lines; 
    string line;
    bool deleted = false;

    while (getline(file, line)) {
        // Se leen los datos en el archivo de appointments. 
        stringstream ss(line);
        string user, datetime, attended;
        getline(ss, user, '|');
        getline(ss, datetime, '|');
        getline(ss, attended, '|');

        string storedDate = datetime.substr(0, 10);
        // Se comparan las fechas. 

        if (user == targetUser && storedDate == targetDate) {
            if (role != "admin" && !isMoreThan24Hours(datetime)) {
                // Solo el admin tiene el control de borrar la cita en menos de 24hrs, no el usuario.
                // Por lo tanto se compara el tiempo, y en caso de que sea en menos de 24hrs, le da un mensajito al usuario.
                cout << "You can only delete appointments more than 24 hours in advance." << endl;
                logAction(username, "Attempted deletion less than 24h in advance.");
                lines.push_back(line);
                continue;
            }
            deleted = true; // En caso de que si pueda borrar la cita. 
            logAction(username, "deleted appointment for " + user);
            // Se registra la actividad del usuario.
        } else {
            lines.push_back(line);
        }
    }
    file.close(); // Se cierra el archivo. 

    if (deleted) {
        ofstream out("appointments.txt"); // Se abre el archivo de appointments. 
        // Se borra la cita en el arhivo. 
        for (size_t i = 0; i < lines.size(); i++) {
            out << lines[i] << endl;
        }
        cout << "Appointment deleted successfully." << endl;
    } else {
        // Mensajito de error! 
        cout << "No matching appointment found." << endl;
        logAction(username, "No matching appointment found during deletion.");
    }
}

// =================================================================
// Correr codigo en Terminal: 
// cd .\Mini-App\
// cl /EHsc Mini-App.cpp bcrypt.cpp blowfish.cpp /Fe:Mini-App.exe
// .\Mini-App.exe
// =================================================================

// Menu principal: 
int main() {
    // Variables: 
    int startChoice;
    User user;

    // Menu principal:
    // Donde el usuario se puede registrar o ingresar al sistema. 
    do {
        cout << "--- Mini-App ---" << endl;
        cout << "1. Register" << endl;
        cout << "2. Login" << endl;
        cout << "3. Exit" << endl;
        cout << "Choice: ";
        cin >> startChoice;

        // Proteccion contra errores en el input cin >> action. 
        if (cin.fail()) {
            // Utilizamos cin.fail() en caso de que el usuario no escriba un digito, ya que action es un int. 
            // Se activa un error.
            cin.clear(); // cin.clear() se encarga de limpiar el estado del error. 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            // cin.ignore() se encarga de descartar los caracteres invalidos, en caso de que el usuario ingrese letras. 
            // Protegemos que no se haga un loop infinito al ingresar caracteres que afecten la variable. 
            cout << "Invalid choice! Please enter a number." << endl;
            // Mensajito de error. 
            startChoice = -1; // Forzamos el switch en del Default. 
            // Registrar actividad: 
            logAction("Guest", "Invalid input in start menu.");
        }

        // Registramos la actividad del usuario desconocido por el momento.  
        logAction("Guest", "User selected start menu option: " + to_string(startChoice));

        switch (startChoice) {
            // Utilizamos un Switch con el input de action, para llamar las funciones que se hayan elegeido. 
        case 1:
            registerUser(); // Llama a la funcion de registracion del usuario.
            // Resgitrar actividad: 
            logAction("Guest", "Choose register option.");
            break;
        case 2:
            // Registrar actividad: 
            logAction("Guest", "Choose login option");
            user = login(); 
            // Estructura de User contiene el Username, password y el role (admin/user).
            // Se llama la funcion de Login(), la cual esta encargada manjear los datos del usuario al entrar a la cuenta. 
            
            if (!user.username.empty()) {
                // Si el usuario o el password llegan a estar incorrectos, se termina el programa.
                // De lo contrario de dirige al menu. 

                int action;

                do {
                    // Menu al entrar la cuenta, en donde el usuario tiene el control de
                    // crear, ver, actulizar y borrar su turno/ cita.
                    cout << "--- Menu ---" << endl;
                    cout << "1. Create Appointment" << endl;
                    cout << "2. View Appointments" << endl;
                    cout << "3. Update Appointment" << endl;
                    cout << "4. Delete Appointment" << endl;
                    cout << "5. Exit" << endl;
                    cout << "Choice: ";
                    cin >> action;

                    // Registrar actividad: 
                    logAction(user.username, "Selected main menu option: " + to_string(action));

                    // Proteccion contra errores en el input cin >> action. 
                    if (cin.fail()) {
                        // Utilizamos cin.fail() en caso de que el usuario no escriba un digito, ya que action es un int. 
                        // Se activa un error.
                        cin.clear(); // cin.clear() se encarga de limpiar el estado del error. 
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        // cin.ignore() se encarga de descartar los caracteres invalidos, en caso de que el usuario ingrese letras. 
                        // Protegemos que no se haga un loop infinito al ingresar caracteres que afecten la variable. 
                        cout << "Invalid choice! Please enter a number." << endl;
                        // Mensajito de error. 
                        action = -1; // Forzamos el swith en Default.
                        continue; 
                        // Registrar actvidad:
                        logAction(user.username, "Invalid input in appointment menu.");
                    }

                    switch (action) {
                    case 1: 
                        createAppointment(user.username); 
                        // (1) llama la funcion de CREAR un turno, y como parametro, pasamos el username. 
                        // Registrar actvidad:
                        logAction(user.username, "Choose create appointment option.");
                        break;
                    case 2: 
                        readAppointments(user.username, user.role); 
                        // (2) llama la funcion de LEER un turno, y como parametro, pasamos el username y se pasa el rol (admin/ user). 
                        // Ya aqui empezamos a dividir los controles de acceso por admin o el usuario. 
                        // El admin tiene acceso a TODO el contenido (turnos) de los usuarios.
                        // Mientras, que el usuario SOLO tiene acceso a su contenido. 
                        // Registrar actvidad:
                        logAction(user.username, "Choose read appointment option.");
                        break;
                    case 3: 
                        updateAppointment(user.username, user.role); 
                        // (3) llama la funcion de ACTUALIZAR un turno. 
                        // Registrar actvidad:
                        logAction(user.username, "Choose update appointment option.");
                        break;
                    case 4: 
                        deleteAppointment(user.username, user.role); 
                        // (4) llama la funcion de DELETE un turno. 
                        // Registrar actvidad:
                        logAction(user.username, "Choose delete appointment option.");
                        break;
                    case 5: 
                        cout << "Goodbye!" << endl; 
                        // SALIDA del loop. 
                        // Registrar actvidad:
                        logAction(user.username, "Choose EXIT option.");
                        break;
                    default: 
                        cout << "Invalid choice." << endl; 
                        // Mensajito de error. 
                        // Registrar actvidad:
                        logAction(user.username, "Invalid menu choice input.");
                        break;
                    }
                } while (action != 5); // Loop del menu, 5 es la SALIDA. 
            }
            break;
        case 3:
            cout << "Exiting program..." << endl;
            logAction("Guest", "Choose EXIT option.");
            break;
        default:
            cout << "Invalid choice." << endl;
            logAction("Guest", "Invalid choice input.");
            break;
        }
    } while (startChoice != 3);

    return 0;
}


// "' OR 1=1 --" o "; DROP TABLE users;"