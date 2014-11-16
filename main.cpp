#include "huffman.h"
#include <iostream>
#include <string.h>
using namespace std;

int main(int argc, char *argv[])
{
    bool op = 0; // 0 - zip, 1 - unzip
    char *inputfile; 
    char *outputfile;
    bool useop = true; 
    bool usef = true; 
    bool useo = true;
    
    if (argc != 6) 
    {
        cout << "Should be 6 arguments \n";
        return -1;
    }
    
    for(int i=1; i<argc; i++) 
    {
        string arg(argv[i]);
        if (arg == "-c")
        { // zip
            op = 0;
            useop = 0;
        } else if (arg == "-u") { //unzip
            op = 1;
            useop = 0;
        } else if ( arg == "-f" || arg == "--file" ) {
            inputfile = argv[++i];           
            usef = 0;
        } else if (arg == "-o" || arg == "--output" ) {
            outputfile = argv[++i];
            useo = 0;
        } else {
            cout << "Syntax error. \n";
            return -1;
        }
    }

    if (useop || usef || useo)
    {
            cout << "Syntax error. \n";
            return -1;
    }

    if (!op)
    {
        encrypt(inputfile, outputfile);        
    } else {
        decrypt(inputfile, outputfile);
    }
    return 0;
}
