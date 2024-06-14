#include "plist.h"

/**
Usage :
display process info

-h             : display the help message
-d <pid>       : display process info based on a pid
<process_name> : display process info based on the name
*/
void printHelpMessage() {
    printf("\n"
        "Usage :\n"
        "display process info\n"
        "\n"
        "\t-h             : display the help message\n"
        "\t-d <pid>       : display process info based on a pid\n"
        "\t<process_name> : display process info based on the name\n"
        "\n");
}

void printHelpMessage();

int main(int argc, char** argv) {
    // On récupère le pointeur de la méthode NtQueryInformationProcess
    pNtQueryInformationProcess = (NtQueryInformationProcessFunc)GetProcAddress(GetModuleHandleA("ntdll"), "NtQueryInformationProcess");
    if (pNtQueryInformationProcess == NULL) {
        printf("[ERROR] while getting ntquery process func\n");
        return(FALSE);
    }

    // On récupère le pointeur de la méthode NtQueryInformationThread
    pNtQueryInformationThread = (NtQueryInformationThreadFunc)GetProcAddress(GetModuleHandleA("ntdll"), "NtQueryInformationThread");
    if (pNtQueryInformationThread == NULL) {
        printf("[ERROR] while getting ntquery thread func\n");
        return(FALSE);
    }

    // Si il n'y a pas d'arguments passés on retourne la liste des processus
    if (argc < 2) {
        GetProcessList();
        return(TRUE);
    }

    char* second_arg = argv[1];

    // On vérifie si le second argument passé est un flag (un flag commence toujours par '-' ex: '-h' -> affiche le message d'erreur)
    // Si ce n'est pas un flag c'est une recherche de processus par nom
    if (*second_arg != '-') {
        GetProcessByName(argv[1]);
        return(TRUE);
    }

    char flag = *++second_arg;

    switch (flag) {
        // On affiche le detail d'un processus si le flag est '-d'
        case 'd': {
            // On vérifie qu'il y a bien le pid passé en argument sinon erreur
            if (argc != 3) {
                printf("Please provide a pid\n");
                return(FALSE);
            }
            // On converti le pid en un entier
            int pid = atoi(argv[2]);

            //printf("PID -> %d\n", pid);

            GetProcessDetails(pid);
            break;
        }
        // On affiche le message d'aide si le flag est '-h' ou si le flag passé n'est pas reconnu
        case 'h':
        default:
            printHelpMessage();
            break;
    }

    return(TRUE);
}