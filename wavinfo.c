#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Aliases for primitive data types
typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

typedef struct
{
    DWORD ckID;
    DWORD ckSize;
    //BYTE ckData[ckSize];
} broadcast_audio_extension;

int main(int argc, char *argv[])
{
    // Open audiofile
    char *filepath = "Glas.wav";
    FILE *audiofile = fopen(filepath, "r");
    if (audiofile == NULL)
    {
        printf("Could not open file %s\n", filepath);
        return 1;
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

    // Check fileSize, chunkSize = file length in bytes - 8
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
    // Consume Chunk, analyse chID and call according reader function



    // char chunkID[5];
    // chunkID[4] = '\0';
    // fread(&chunkID, sizeof(char), 4, audiofile);
    // if (strcmp(chunkID, "bext") == 0)
    // {
    //     // Bext Chunk for Broadcast Wave Format
    //     printf("\nBroadcast Wave Format Information:\n\n");
    // }
    // else if (strcmp(chunkID, "fmt ") == 0)
    // {
    //     printf("\nWave Header Information:\n\n");
    // }
    // printf("%s", chunkID);
    // return 0;


    // // Bext Chunk for Broadcast Wave Format
    // char bextChunk[5];


    // Format Chunk

    // Check formatChunk for "fmt "
    char formatChunk[5];
    formatChunk[4] = '\0';
    fread(formatChunk, sizeof(char), 4, audiofile);
    if (strcmp(formatChunk, "fmt ") != 0)
    {
        printf("Wrong format-chunk found (fmt ): %s\n", formatChunk);
        return 2;
    }
    printf("Format ID:  %s\n", formatChunk);

    // Check format length
    uint32_t fmtlength;
    fread(&fmtlength, sizeof(uint32_t), 1, audiofile);
    printf("fmt length: %i Bytes\n", fmtlength);

    // Check format tag: 0x0001 -> PCM
    uint16_t wFormatTag;
    fread(&wFormatTag, sizeof(uint16_t), 1, audiofile);
    if (wFormatTag != 1)
    {
        printf("Only PCM files are supported: wFormatTag = %i\n", wFormatTag);
        return 2;
    }
    printf("Format Tag: %i (PCM)\n", wFormatTag);

    // Get audio channels
    uint16_t wChannels;
    fread(&wChannels, sizeof(uint16_t), 1, audiofile);
    printf("Audio channels: %i\n", wChannels);

    // Get audio channels
    uint32_t dwSamplesPerSec;
    fread(&dwSamplesPerSec, sizeof(uint32_t), 1, audiofile);
    printf("Samplerate: %i\n", dwSamplesPerSec);

    // Get average bytes per second, e.g. 2 X 16 * 44100 / 8
    uint32_t dwAvgBytesPerSec;
    fread(&dwAvgBytesPerSec, sizeof(uint32_t), 1, audiofile);
    printf("Average Bytes / sec: %i\n", dwAvgBytesPerSec);

    // Get block allign
    uint16_t wBlockAlign;
    fread(&wBlockAlign, sizeof(uint16_t), 1, audiofile);
    printf("Block align: %x\n", wBlockAlign);

    // Get block allign
    uint16_t bitspersample;
    fread(&bitspersample, sizeof(uint16_t), 1, audiofile);
    printf("bitspersample: %x\n", bitspersample);

    char padChunk[5];
    padChunk[4] = '\0';
    fread(padChunk, sizeof(char), 4, audiofile);
    if (strcmp(padChunk, "fmt ") != 0)
    {
        printf("Wrong format-chunk found (fmt ): %s\n", padChunk);
        return 2;
    }
    printf("padChunk ID:  %s\n", padChunk);

}