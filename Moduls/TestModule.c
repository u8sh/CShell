#include "../lib/CSHLib.h"
#include <stdio.h>
#include <stdbool.h>

int calculate(const char *calculation, char *result, size_t result_size) {
    int firstNum = 0;
    int secondNum = 0;
    char sign = '\0';

    for (size_t i = 0; calculation[i] != '\0'; i++) {
        if (calculation[i] == '+' || calculation[i] == '-' ||
            calculation[i] == '*' || calculation[i] == '/') {
            sign = calculation[i];
        } else if (calculation[i] >= '0' && calculation[i] <= '9') {
            if (sign == '\0') {
                firstNum = firstNum * 10 + (calculation[i] - '0');
            } else {
                secondNum = secondNum * 10 + (calculation[i] - '0');
            }
        } else {
            return 0; // Ungültiges Zeichen
        }
    }

    int value;
    switch (sign) {
        case '+': value = firstNum + secondNum; break;
        case '-': value = firstNum - secondNum; break;
        case '*': value = firstNum * secondNum; break;
        case '/':
            if (secondNum == 0) return 0;
            value = firstNum / secondNum;
            break;
        default:
            return 0;
    }

    snprintf(result, result_size, "%d", value);
    return 1;
}

char *onInit() {
    // csh_ExecuteCommand("ls", false);
    return NULL;
}

char *onTestExecution(char *cmd, char *args[], int args_count) {
    char message[10000];
    csh_printNormal("test: %s\n", cmd);

    cJSON *data = cJSON_CreateObject();
    cJSON_AddItemToObject(data, "test", cJSON_CreateString("teststring"));

    // csh_callEvent("testevent", data);
    // CSHConfig config = csh_GetConfig();
    return NULL;
}

char *onCalcExecution(char *cmd, char *args[], int args_count) {
    char result[32];

    if (calculate(args[0], result, sizeof result)) {
        csh_printNormal("%s\n\n", result);
    } else {
        csh_printNormal("Ungueltiger Ausdruck\n");
    }
    return NULL;
}

char *onShutdownExecution(char *cmd, char *args[], int args_count) {
    csh_printInfo("Shutting down...\n");
    csh_Shutdown(0);
}

char *onTestEvent(cJSON *data) {
    csh_printNormal("TestEvent: ");
    csh_printJSON(data);
    csh_printNormal("\n");
    return NULL;
}

int main(int argc, char *argv[]) {
    csh_InitModule(argv, argc);
    CSHManpage *manpage = Manpage_Init();
    ManpageParagraph *paragraph = Manpage_CreateParagraph();
    ManpageTitle *title = Manpage_CreateTitle("TestManpage");
    ManpageTitle *paragraph_title = Manpage_CreateTitle("TestParagraph");
    ManpageContent *paragraph_content = Manpage_CreateContent("Das ist ein Test Befehl!");

    Manpage_SetTitleFormat(title, COLOR_RED, true, true, true, false);
    Manpage_SetTitleFormat(title, COLOR_CYAN, false, false, false, false);
    Manpage_SetContentFormat(paragraph_content, COLOR_MAGENTA, false, false, false, false);

    Manpage_SetParagraphTitle(paragraph, paragraph_title);
    Manpage_SetParagraphContent(paragraph, paragraph_content);
    Manpage_AddParagraph(manpage, paragraph);
    Manpage_SetTitle(manpage, title);

    char *test_aliases[10] = { "te" };
    char *calc_aliases[10] = { "calc" };
    csh_RegisterCommand("test", test_aliases, 1, "Das ist ein Testbefehl.", manpage, onTestExecution);
    csh_RegisterCommand("calculate", calc_aliases, 1, "Dieser Befehl berechnet die Differenz/Summe/Quotient/Produkt zweier Zahlen aus.", NULL, onCalcExecution);
    csh_RegisterCommand("shutdown", NULL, 0, "Schaltet die Shell aus.", NULL, onShutdownExecution);

    csh_RegisterEvent("testevent", onTestEvent);

    csh_BuildModule("TestModul", "Author", "1.0", "Das ist ein Test Modul", onInit);
    return 0;
}