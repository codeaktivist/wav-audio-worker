#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Aliases for primitive data types based on ms specifications
typedef uint8_t  BYTE;
typedef uint8_t  CHAR;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t  LONG;

// Chunks are the building blocks of RIFF, can be nested, optional or mandatory, ignored if unknown
// I am not using this generic struct but chunk-specific structs for extracting data from ckData
// Therefor NOT touching the data twice, 1. Read to ckData 2. extract ckData into specific struct
typedef struct ck
{
    DWORD ckID;             // Chunk type identifier
    DWORD ckSize;           // Chunk size field (size of ckData)
    BYTE ckData[];          // Chunk data, flexible array member
} ck;

// Format Chunk
typedef struct fmt
{
    WORD wFormatTag;        // Format category
    WORD nChannels;         // Number of channels
    DWORD nSamplesPerSec;   // Sampling rate
    DWORD nAvgBytesPerSec;  // For buffer estimation
    WORD nBlockAlign;       // Data block size
} fmt;

// Broadcast wave file chunk (optional extension)
typedef struct bext
{
    CHAR Description[256];          // Description of the sound sequence
    CHAR Originator[32];            // Name of the originator
    CHAR OriginatorReference[32];   // Reference of the originator
    CHAR OriginationDate[10];       // yyyy:mm:dd
    CHAR OriginationTime[8];        // hh:mm:ss
    DWORD TimeReferenceLow;         // First sample count since midnight, low word
    DWORD TimeReferenceHigh;        // First sample count since midnight, high word
    WORD Version;                   // Version of the BWF; unsigned binary number
    BYTE UMID[64];                  // Binary bytes of SMPTE UMID
    WORD LoudnessValue;             // Integrated Loudness Value of the file in LUFS (multiplied by 100)
    WORD LoudnessRange;             // Loudness Range of the file in LU (multiplied by 100)
    WORD MaxTruePeakLevel;          // Maximum True Peak Level of the file expressed as dBTP (multiplied by 100)
    WORD MaxMomentaryLoudness;      // Highest value of the Momentary Loudness Level of the file in LUFS (multiplied by 100)
    WORD MaxShortTermLoudness;      // Highest value of the Short-Term Loudness Level of the file in LUFS (multiplied by 100)
    BYTE Reserved[180];             // 180 bytes, reserved for future use, set to “NULL”
    CHAR CodingHistory[];           // History coding, flexible array member
} bext;

// Prototypes of helper functions
const char* printbyte(BYTE unprintable[], int len);
const char* printstring(CHAR *unprintable, int len);


int main(int argc, char* argv[])
{

    printf("\n========================================\n");
    printf("    wavinfo -> analysing your file    \n");
    printf("========================================\n\n");

    
    // Check command line arguments, retrieve filepath
    char *filepath;
    if (argc != 2)
    {
        printf("\nMissing or too many comand line arguments!\n");
        printf("Usage: ./wavinfo yourfile.wav\n\n");
        return 1;
    }
    else
    {
        filepath = argv[1];
    }

    // Open audiofile
    FILE *audiofile = fopen(filepath, "r");
    if (audiofile == NULL)
    {
        printf("\nCould not open file: %s\n\n", filepath);
        return 1;
    }
    else
    {
        printf("File name:  %s\n", filepath);
    }

    // Read and check file header

    // RIFF WAVE Chunk (mandatory)
    // http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/Docs/riffmci.pdf
    // https://tech.ebu.ch/docs/tech/tech3285.pdf

    // Check groupID for "RIFF"
    char groupID[5];
    groupID[4] = '\0';
    fread(&groupID, sizeof(char), 4, audiofile);
    if (strcmp(groupID, "RIFF\0") != 0)
    {
        printf("Only RIFF-WAVE-Files are supported!\n");
        printf("Chunk ID:   %s\n", groupID);
        return 2;    
    }
    printf("Chunk ID:   %s\n", groupID);

    // Check fileSize (chunkSize = file length in bytes - 8)
    uint32_t fileSize;
    fread(&fileSize, sizeof(uint32_t), 1, audiofile);
    printf("File Size:  %u kb\n", (fileSize + 8) / 1024);

    // Check riffType for "WAVE"
    char riffType[5];
    riffType[4] = '\0';
    fread(&riffType, sizeof(char), 4, audiofile);
    if (strcmp(riffType, "WAVE\0") != 0)
    {
        printf("Only RIFF-WAVE-Files are supported!\n");
        printf("Riff Type:  %s\n", riffType);
        return 2;    
    }
    printf("Riff Type:  %s\n", riffType);

    // Chunks are defined by ckID and ckSize followed by the ckData with a length of ckSize
    // Read Chunk, analyse chID and fill ckData into according data structure if fmt or bext chunks
    // ChunkIDs that do not have an associated data structure are printed as ASCII for reference

    do
    {
        char check;
        // Check for reaching End Of File
        if (feof(audiofile))
        {
            printf("\n\n");
            return 0;
        }

        // Ignoring NUL characters (padding)
        fread(&check, sizeof(CHAR), 1, audiofile);
        if (check == 0)
        {
            continue;
        }
        else
        {
            fseek(audiofile, -1, SEEK_CUR);
        }

        // Read chunkID (4 letter code + terminator)
        char ckID[5];
        ckID[4] = '\0';
        fread(&ckID, sizeof(DWORD), 1, audiofile);

        // Read chunkSize
        DWORD ckSize;
        fread(&ckSize, sizeof(DWORD), 1, audiofile);

        // Allocate memory for chunkData
        BYTE ckData[ckSize];

        // Format chunk
        if (strcmp(ckID, "fmt ") == 0)
        {
            printf("\n\nFound mandatory Format Chunk: %s\n", ckID);
            printf("---------------------------------\n");
            printf("%27u Bytes\n\n", ckSize);
            // Read fmt chunk into fmt structure
            fmt fmtCk;
            fread(&fmtCk, sizeof(fmt), 1, audiofile);
            // Print Format Chunk Information
            printf("          wFormatTag: %i\n", fmtCk.wFormatTag);
            printf("           nChannels: %i\n", fmtCk.nChannels);
            printf("      nSamplesPerSec: %i\n", fmtCk.nSamplesPerSec);
            printf("     nAvgBytesPerSec: %i\n", fmtCk.nAvgBytesPerSec);
        }
        // BEXT chunk
        else if (strcmp(ckID, "bext") == 0)
        {
            printf("\n\nFound Broadcast Wave Chunk: %s\n", ckID);
            printf("--------------------------------\n");
            printf("%26u Bytes\n\n", ckSize);
            // Read bext chunk into bext structure
            bext bextCk;
            fread(&bextCk, sizeof(bext), 1, audiofile);
            // Print BEXT Chunk Information
            printf("Description (if any):\n%s\n\n", printstring(bextCk.Description, sizeof(bextCk.Description)));
            printf("          Originator: %s\n", printstring(bextCk.Originator, sizeof(bextCk.Originator)));
            printf(" OriginatorReference: %s\n", printstring(bextCk.OriginatorReference, sizeof(bextCk.OriginatorReference)));
            printf("     OriginationDate: %s\n", printstring(bextCk.OriginationDate, sizeof(bextCk.OriginationDate)));
            printf("     OriginationTime: %s\n", printstring(bextCk.OriginationTime, sizeof(bextCk.OriginationTime)));
            printf("    TimeReferenceLow: %i\n", bextCk.TimeReferenceLow);
            printf("   TimeReferenceHigh: %i\n", bextCk.TimeReferenceHigh);
            printf("             Version: %i\n", bextCk.Version);
            printf("                UMID: %s\n", printbyte(bextCk.UMID, sizeof(bextCk.UMID)));
            printf("       LoudnessValue: %i\n", bextCk.LoudnessValue);
            printf("       LoudnessRange: %i\n", bextCk.LoudnessRange);
            printf("    MaxTruePeakLevel: %i\n", bextCk.MaxTruePeakLevel);
            printf("MaxMomentaryLoudness: %i\n", bextCk.MaxMomentaryLoudness);
            printf("MaxShortTermLoudness: %i\n", bextCk.MaxShortTermLoudness);
            printf("            Reserved: %s\n", printstring(bextCk.Reserved, sizeof(bextCk.Reserved)));
            printf("       CodingHistory: %s\n", bextCk.CodingHistory);

            // Rewind file pointer by 2 bytes to compensate offset:
            // Size according to ckSize = 602 vs. sizeof(bext) = 604
            fseek(audiofile, -2, SEEK_CUR);
        }
        // Check for End of File (EOF) reached
        else if (fread(&ckData, sizeof(BYTE), ckSize, audiofile) == 0)
        {
            printf("\n\n");
            return 0;
        }
        // data chunk
        else if (strcmp(ckID, "data") == 0)
        {
            printf("\n\nFound Data Chunk (audio samples): %s\n", ckID);
            printf("--------------------------------------\n");
            printf("%32u Bytes\n\n", ckSize);
        }  
        // ProTools chunks
        else if ((strcmp(ckID, "minf") == 0) 
            || (strcmp(ckID, "elmo") == 0)
            || (strcmp(ckID, "regn") == 0)
            || (strcmp(ckID, "ovwf") == 0)
            || (strcmp(ckID, "umid") == 0))
        {
            printf("\n\nFound ProTools Chunk: %s\n", ckID);
            printf("--------------------------\n");
            printf("%20u Bytes\n\n", ckSize);
            printf("Data as ASCII (if any):\n");
            printf("%s\n", printstring(ckData, sizeof(ckData)));
        }
        // iXML chunk (Wave Agent / BWFXML)      
        else if (strcmp(ckID, "iXML") == 0)
        {
            printf("\n\nFound iXML / BWFXLM Chunk %s\n", ckID);
            printf("------------------------------\n");
            printf("%24u Bytes\n\n", ckSize);
            printf("Data as ASCII (if any):\n");
            printf("%s\n", printstring(ckData, sizeof(ckData)));
        }
        // PAD chunk (Found in a standard WAVE ?)
        else if (strcmp(ckID, "PAD ") == 0)
        {
            printf("\n\nFound Padding Chunk: %s\n", ckID);
            printf("-------------------------\n");
            printf("%19u Bytes\n\n", ckSize);
            printf("Data as ASCII (if any):\n");
            printf("%s\n", printstring(ckData, sizeof(ckData)));
        }
        else
        // Unknown chunk
        {
            printf("\n\nFound unknown Chunk: %s\n", ckID);
            printf("-------------------------\n");
            printf("%19u Bytes\n\n", ckSize);
            printf("Data as ASCII (if any):\n");
            printf("%s\n", printstring(ckData, sizeof(ckData)));
        }
    } while (1);

    // Close file, free memory
    fclose(audiofile);
}


// Convert unprintable binary information to printable string
const char* printstring(CHAR *unprintable, int len)
{
    char* printable = malloc((sizeof(CHAR) + 1) * len);
    int last_printable = 0;
    for (int i = 0; i < len; i++)
    {
        printable[i] = unprintable[i];

        // Track position of terminating NUL or whitespace, to get rid of padding
        if (unprintable[i] != '\0' && unprintable[i] != ' ')
        {
            last_printable = i;
        }
    }
    // Append NUL to end of printable string
    printable[last_printable + 1] = '\0';

    // Return printable of empty
    if (strlen(printable) == 0)
    {
        return "... nothing here";
    }
    else
    {
        return printable;
    }
}


// Convert array of UIID to printable ASCII strings
const char* printbyte(BYTE unprintable[], int len)
{
    char* printable = malloc(len * sizeof(CHAR));
    for (int i = 0; i < len; i++)
    {
        printable[i] = unprintable[i];
    }
    printable[len] = '\0';
    return printable;
}


// ToDo:
// Free all memory, check with valgrind
// Analyse data and put up some statistics?